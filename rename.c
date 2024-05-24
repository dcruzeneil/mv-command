#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include </usr/src/sys/ufs/ffs/fs.h>
#include </usr/src/sys/ufs/ufs/dinode.h>
#include </usr/src/sys/ufs/ufs/dir.h>
#include </sys/sys/mman.h>
#include </sys/sys/stat.h>

#define SUPER_BLOCK_OFFSET 65536
#define BLOCK_SIZE 32768
#define FRAGMENT_SIZE 4096
#define MAXIMUM_PATH_DEPTH 10
#define MAXIMUM_PATH_SIZE 255
#define INODE_SIZE 256
#define ROOT_LEVEL 0


/* function prototypes */
void traverseDirectBlocks(char*, struct fs*, int, int, int, int64_t*, char[MAXIMUM_PATH_DEPTH][MAXIMUM_PATH_SIZE], int*);
int64_t traverseDirectories(int, int64_t*, int64_t, int, struct fs*, char*, char[MAXIMUM_PATH_DEPTH][MAXIMUM_PATH_SIZE], int, int*);
int tokenizePath(char*, char[MAXIMUM_PATH_DEPTH][MAXIMUM_PATH_SIZE]);
struct ufs2_dinode* inodeAddress(char*, struct fs*, int);
struct direct* dirAddress(char*, int);

int 
main(int argc, char *argv[]){
    /* checking proper usage of the command */
    if(argc < 4){
        printf("USAGE: ./rename [DISK IMG] [SOURCE] [NEW NAME]\n");
    }

    /* tokenizing the SOURCE and TARGET */
    char source[MAXIMUM_PATH_DEPTH][MAXIMUM_PATH_SIZE];
    int source_depth = tokenizePath(argv[2], source);

    /* opening the raw image of the disk partition */
    int fd = open(argv[1], O_RDWR);
    if(fd == -1){
        perror("open");
        exit(1);
    }

    /* calculating the size of the image and mapping it to memory */
    struct stat file;
    if(stat(argv[1], &file) < 0){
        perror("stat");
        exit(1);
    }
    long partition_size = file.st_size;

    /* mapping the file to memory */
    char *address = mmap(NULL, partition_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if(address == (void*) -1){
        perror("mmap");
        exit(1);
    }

    /* identifying the superblock */
    struct fs *superblock = (struct fs*)(address + SUPER_BLOCK_OFFSET); 
    
    /* traversing the memory blocks of root inode to identify source */
    int file_found = 0;
    int64_t source_dir_int;
    traverseDirectBlocks(address, superblock, UFS_ROOTINO, ROOT_LEVEL, source_depth - 1, &source_dir_int, source, &file_found);
    struct direct *source_dir = (struct direct*) source_dir_int;
    strcpy(source_dir->d_name, argv[3]);

    /* freeing up the mapping */
    if(munmap(address, partition_size) == -1){
        exit(1);
    }
}

void 
traverseDirectBlocks(char *address, struct fs *superblock, int inode_number, int level, int max_depth, int64_t *target_dir_int, char path_arr[MAXIMUM_PATH_DEPTH][MAXIMUM_PATH_SIZE], int *file_found){
    struct ufs2_dinode *inode = inodeAddress(address, superblock, inode_number);
    int64_t size = inode->di_size;
    for(int i = 0; i < UFS_NDADDR; i++){
        if(size <= 0){
            break;
        }
        size -= traverseDirectories(inode->di_db[i], target_dir_int, size, max_depth, superblock, address, path_arr, level, file_found);
        if(*file_found == 1){
            break;
        }
    }
}

int64_t 
traverseDirectories(int block_number, int64_t *target_dir_int, int64_t size_left, int max_depth, struct fs *superblock, char *address, char path_arr[MAXIMUM_PATH_DEPTH][MAXIMUM_PATH_SIZE], int level, int *file_found){
    struct direct *dir_beginning = dirAddress(address, block_number);
    int64_t current_dir_int = (int64_t) dir_beginning;
    struct direct *current_dir;
    int64_t size_traverse = BLOCK_SIZE > size_left ? BLOCK_SIZE : size_left;

    for(; current_dir_int < current_dir_int + size_traverse; current_dir_int += current_dir->d_reclen){
        current_dir = (struct direct*) current_dir_int;
        // we have found the file on the current level 
        if(strcmp(current_dir->d_name, path_arr[level]) == 0){
            // we have found the final file
            if(level == max_depth){
                *target_dir_int = current_dir_int;
                *file_found = 1;
                return current_dir_int - (int64_t) dir_beginning;
            }
            traverseDirectBlocks(address, superblock, current_dir->d_ino, level + 1, max_depth, target_dir_int, path_arr, file_found);
        }
        // we found the exact directory we were looking for 
        if(*file_found == 1){
            return current_dir_int - (int64_t) dir_beginning;            
        }
    }
    return current_dir_int - (int64_t) dir_beginning;
}

int 
tokenizePath(char *path, char path_arr[MAXIMUM_PATH_DEPTH][MAXIMUM_PATH_SIZE]){
    char *delimiter = "/";
    char *token = strtok(path, delimiter);
    int path_depth = 0;
    while(token != NULL){
        strcpy(path_arr[path_depth], token);
        path_depth += 1;
        token = strtok(NULL, delimiter);
    }
    return path_depth;
}

struct ufs2_dinode* 
inodeAddress(char *address, struct fs *superblock, int inode_number){
    int inode_address = ino_to_fsba(superblock, inode_number);
    int inode_offset = ino_to_fsbo(superblock, inode_number);
    struct ufs2_dinode *inode = (struct ufs2_dinode*)(address + (inode_address * FRAGMENT_SIZE) + (inode_offset * INODE_SIZE));
    return inode;
}

struct direct*
dirAddress(char *address, int block_number){
    struct direct *dir_address = (struct direct*)(address + (block_number * FRAGMENT_SIZE));
    return dir_address;
}

