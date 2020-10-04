// Abdullahi Elmi
// 260727124

int getBlockSize();
int getFileLength(int file);
void initIO();
int partition(char *name, int block_size, int total_blocks);
int mount(char *name);
int openfile(char *name);
int readBlock(int file, int beginningFlag);
char *returnBlock();
int writeBlock(int file, char *data, int overwriteFlag);
void cleanDisk();
