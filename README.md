Implementing a C program that uses the structures used to represent a file in UFS2 (Unix File System) - superblock, inode structs, and direct structs - to rename a file.

Usage:
./rename [PATH TO RAW IMAGE OF DISK PARTITION] [FILE/DIRECTORY TO RENAME] [NEW NAME]

Limitations:
Assumes that all the contents of a directory will fit within the 32K * 12 chunks of memory specified within the di_db (direct access) blocks.
That is, the code does not check the indirect blocks to see if the specified directory exists there. 



