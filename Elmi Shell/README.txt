The OS I used was mimi.cs.mcgill.ca

Changes from A3: 
- I fixed the error from my old assignment 3 (roughly 4 lines of change from my assignment 3). The only other changes to assignment 3 files are
in kernel and interpreter to account for the 3 new commands we're adding.

DISK_driver.c: 
- I have 8 helper functions (getBlockSize, getFileLength, cleanBuffer, cleanDisk, findOpenBlock, findOpenFPCell, findOpenFATCell, getFPAtIndex) most called within 
this class, except for cleanDisk(). 

- The file system is **non-contiguous** (really need those bonus points after the last assignment)/

IOscheduler.c:
- I have 3 helper functions (ceiling, getBlockCount, and separateIntoBlocks)

- 1 parameter in the main function was changed (wasn't utilized).