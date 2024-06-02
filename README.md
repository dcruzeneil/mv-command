## Code for Renaming Files in UFS2 on FreeBSD (64-bit)
This C program operates on raw images of disk partitions, created using the `dd` tool (after unmounting the partition):
```sh
dd if=/dev/[NAME_OF_PARTITION] of=disk.img bs=4k
```
Using `bs=4k` ensures faster reading and writing by processing data in 4k chunks. Instructions for adding virtual disk images, creating partitions, and mounting partitions can be found in the FreeBSD Handbook: <a href="https://docs.freebsd.org/en/books/handbook/disks/">FreeBSD Handbook - Disks</a>.

## Program Description
This program uses the structures representing files in UFS2 (Unix File System) - including the superblock, inode structs, and direct structs - to rename a file.

#### Usage: 
```sh
./rename [PATH TO RAW IMAGE OF DISK PARTITION] [FILE/DIRECTORY TO RENAME] [NEW NAME]
```
## Limitations
<ol>
  <li>Directory Content Size Assumptions:</li>
  <ul>
    <li>Assumes all directory contents fit within the 32K $\times$ 12 chunks of memory specified within the $di_db$ (direct access blocks).</li>
    <li>Does not check indirect blocks $di_ib$ for the existence of the specified directory.</li>
  </ul>
  <li>File Name Length Change:</li>
  <ul>
    <li>Changing the name of the directory/file also changes the length of the associated direct structure.</li>
    <li>UFS2 uses the smallest amount of space needed to store the current directory name (capped at 255 characters or bytes).</li>
    <li>If the old file name is 20 bytes, using a new name which is 14 bytes is acceptable but wasteful.</li>
    <li>However, writing a new name exceeding the old name's length (e.g., writing 25 bytes, when the old size was 20 bytes) may (and most probably will) cause data corruption by overwriting parts of the subsequent direct structures (a type of unintended overflow).</li>
  </ul>
</ol>

By understanding these constraints, you can effectively use this program to rename files within a UFS2 file system on FreeBSD.
