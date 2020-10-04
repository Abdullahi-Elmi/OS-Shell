// Abdullahi Elmi
// 260727124

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//including the necessary libraries

#include "IOscheduler.h"
#include "DISK_driver.h"
//including the necessary files

int getBlockCount(char *data) {
// needed for the separateIntoBlocks() function which is needed for the main function of the file
    double block_count = (strlen(data) / (double) getBlockSize());
    if((int) block_count < block_count){
       return (int) block_count + 1;
    }
    else{
       return (int) block_count;
    }  
}


char **separateIntoBlocks(char *data) {
// separates the data into a block array
    int block_size = getBlockSize();
    int block_count = getBlockCount(data);
    // block size determines the size of the cells we'll be placing the data into, and block_count, the number of cells
    
    char **blocks = (char **) malloc(block_count * sizeof(char *));
    // 
    
    int ptr = 0;
    int data_length = strlen(data);

    for (int i = 0; i < block_count; i++) {
    // for each block in our new array
        blocks[i] = (char *) malloc((block_size + 1) * sizeof(char));
        // for each of these block cells we'll free up the proper space

        for (int j = 0; j < block_size; j++){
        // looping within a block cell
            if(ptr < data_length){
                blocks[i][j] = data[ptr++];
                // we write in a part of the data
            }
            else{
            // this is the case that we're at EOF
                blocks[i][j] = '\0';
            }
        }
        
        blocks[i][block_size] = '\0';
        // EOF character at the end of every block
    }
    return blocks;
}
/* ---------------------------------------------------------------------Ending Private Helper Functions--------------------------------------------------------------- */

char *IOscheduler(char *data, char *filename, int cmd) {
    
    if (cmd == 0) {
    // reading I/O request
        int file_index = openfile(filename);
        if (file_index < 0){
        // if there was an error opening the file
            return NULL;
        }
        if (readBlock(file_index, 1) < 0){
        // if trying to read the block corresponding to the file index returns an error
            return NULL;
        }
        else {
        // no errors in opening and reading
            char *data_string = (char *) malloc(100 * sizeof(char));
            char *firstBlock = returnBlock();
            strcpy(data_string, firstBlock);
            free(firstBlock);
            // get the data as string from block_buffer into the variable data_string

            int block_count = getFileLength(file_index);
            // this is the number of blocks we'll be reading
            
            for (int i = 1; i < block_count; i++) {
                if (readBlock(file_index, 0) < 0){
                    return NULL;
                    // if read returns an error
                }
                
                char *block = returnBlock();
                strcat(data_string, block);
                // append the data we get from returnBlock() into the variable we'll return
                free(block);
                // necessary for the next loop
            }
            
            return data_string;
            // we've input everything we've read into this string and can return it
        }
    } 

    else if (cmd == 1) {
    // writing I/O request
        
        char **blocks = separateIntoBlocks(data);
        int block_count = getBlockCount(data);
        // chopping the data into an array of blocks, block_count gives us the number of columns in this 2d array

        int file_index = openfile(filename);
        if (file_index < 0){
        // if there was an error opening the file
            return NULL;
        }

        if (writeBlock(file_index, blocks[0], 1) < 0){
        // if trying to write to the block corresponding to the file index returns an error
            return NULL;
        }
        
        else {
        // if there were no errors
            for (int i = 1; i < block_count; i++){
            // write into the file_index from a block in the block array
                if (writeBlock(file_index, blocks[i], 0) < 0){
                // handling errors
                    return NULL;
                }
            }
            return data;
        }
    } 

    else{
        printf("The cmd you entered was inavlid: Please enter 0 for reading requests and 1 for writing requests");
        return NULL;
    }
}
