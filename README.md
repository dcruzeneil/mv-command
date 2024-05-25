Implementing a C program that uses the structures used to represent a file in UFS2 (Unix File System) - superblock, inode structs, and direct structs - to rename a file.

Usage:
./rename [PATH TO RAW IMAGE OF DISK PARTITION] [FILE/DIRECTORY TO RENAME] [NEW NAME]

Limitations:
1. Assumes that all the contents of a directory will fit within the 32K * 12 chunks of memory specified within the di_db (direct access) blocks.
That is, the code does not check the indirect blocks to see if the specified directory exists there. 
2. Does not account for the fact that changing the name of the directory/file also changes the length of the associated direct structure because UFS2 will use the smallest amount
of space needed to store the current directory - (although the name has a hard cap of 255 characters or 255 bytes). 

