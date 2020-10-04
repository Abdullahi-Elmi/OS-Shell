// Abdullahi Elmi
// 260727124

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//including the necessary libraries

#include "DISK_driver.h"
//including the necessary files

typedef struct PARTITION {
    int total_blocks;
    int block_size;
} Partition;
// The partition structure is defined here as it is in the pdf

typedef struct FAT {
    char filename[50];
    int file_length;
    int blockPtrs[10];
    int current_location;
} FAT;
// The FAT structure is defined here as it is in the pdf

Partition partitionStruct;
FAT fat[20];
char p_name[50];
char *block_buffer;
FILE *fp[5];
// initializing the data structures


int *blocks;
// Keeps track of all blocks
int fpFATMap[5];
// This array maps fps to fats

// Defining our Helpers: getBlockSize, getFileLength, cleanBuffer, findOpenBlock, findOpenFPCell, findOpenFATCell, getFPAtIndex

int getBlockSize() {
// gets the size of the block
    return partitionStruct.block_size;
} 

int getFileLength(int file) {
// gets the length of the inputted file
    return fat[file].file_length;
}

void cleanBuffer() {
// cleans the buffer for future iterations
    for (int i = 0; i < partitionStruct.block_size; i++){
    // loop through the entire buffer
        block_buffer[i] = '\0';
        // then write over each cell with '\0'
    }
}

void cleanDisk() {
// cleans everything within the disk that needs to be cleared for the next iteration
    free(block_buffer);
    // cleaning the buffer
    for (int i = 0; i < 5; i++){
        if (fp[i] != NULL){
            fclose(fp[i]);
            // closing any open file pointers
        }
    }     
    free(blocks);
    // cleaning the blocks
}

int findEmptyBlock() {
// returns the index for the first open block we can find
    for (int i = 0; i < partitionStruct.total_blocks; i++){
     // loop through the entire blocks array
        if (blocks[i] < 0){
            // if the cell is free, we return it's index
            return i;
        }
    }       
    return -1;
    // if we can't find anything, we use -1 as the sign that no cells are free
} 

int findOpenFPCell() {
// returns the index for the first free cell in the fp array
    for (int i = 0; i < 5; i++){
    // loop through the entire fp array
        if (fp[i] == NULL){
            // if the cell is free, we return it's index
            return i;
        }
    }   
    return -1;
    // if we can't find anything, we use -1 as the sign that no cells are free
} 

int findOpenFATCell() {
// returns the index for the first free cell in the FAT array
    for (int i = 0; i < 20; i++){
    // loop through the entire FAT array
        if (fat[i].file_length == 0){
            // if the cell is free, we return it's index
            return i;
        }
    }
    return -1;
    // if we can't find anything, we use -1 as the sign that no cells are free
} 

FILE *getFPAtIndex(int file) {
// returns the fp that matches the input within the fp array
    for (int i = 0; i < 5; i++){
    // loop through the entire fp array
        if (fpFATMap[i] == file){
            // if the fp matches the input
            return fp[i];
            // we return it, otherwise continue looping
        }
    }     
    return NULL;
    // the return typ is FILE* so we can't return -1 as our usual signal, the equivalent here is null
} 


void initIO() {
// Initializing all global data structure and variables as stated in the pdf
    for (int i = 0; i < 50; i++){
        p_name[i] = '\0';
        // create the partition name, and initialize all cells as empty
    }

    block_buffer = NULL;
    // initializing the buffer
    
    for (int i = 0; i < 5; i++) {
    // initializing the fp array, and the mapping array
        fp[i] = NULL;
        fpFATMap[i] = -1;
    }

    blocks = NULL;
    // initializing the blocks
}


int partition(char *name, int block_size, int total_blocks) {
/* This must be called first. Creates a directory called PARTITION if it does not already exist within the directory. Then we create and format a file within it
   as instructed within the pdf, also we return 1 to signal success and 0 otherwise.*/
    system("mkdir -p PARTITION");
    // Creating the PARTITION directory

    char path[100] = {'\0'};
    sprintf(path, "./PARTITION/%s", name);
    FILE *f = fopen(path, "wb");
    // p will be the file we want to create within PARTITION
    if (f == NULL) {
    // if we failed to create the file f properly
        printf("Error: The partition \"%s\" was not created.\n", name);
        return 0;
        // we return 0 as the error signal, and print a message about the error
    }

    Partition tmpPartition = {total_blocks, block_size};
    // create a temporary partition from the inputted parameters
    fwrite(&tmpPartition, sizeof(Partition), 1, f);
    // writing data from the temporary partition to the created file f
    strncpy(p_name, name, strlen(name));
    // copy the inpuuted name to p_name data structure created near the beginning of this file

    for (int i = 0; i < 50; i++){
        fwrite(&p_name[i], sizeof(char), 1, f);
    }
    // the beginning of the file is filled with the information from struct partition
    
    FAT tmpFAT[20];

    for (int i = 0; i < 20; i++) {
        strcpy(tmpFAT[i].filename, "");
        tmpFAT[i].file_length = 0;

        for (int j = 0; j < 10; j++){
            tmpFAT[i].blockPtrs[j] = -1;
        }

        tmpFAT[i].current_location = -1;
        // initializing all factors of tmpFAT
    }

    fwrite(tmpFAT, sizeof(FAT), 20, f);
    // the next part of the file is filled with the information from fat[20]
    
    
    int pDataArea = total_blocks * block_size;
    // defined within the pdf
    char zero = '0';
    
    for (int i = 0; i < pDataArea; i++){
        fwrite(&zero, sizeof(char), 1, f);
    }
    // appending the partition data area  by writing total_blocks*block_size number of ‘0’ (character zero) to the end of the file

    fclose(f);
    return 1;
    // If we've made it to the end of the function, we know we've run it successfully and can return 1
}

int mount(char *name) {
// this loads information into partition and FAT. It also creates a buffer

    char path[100] = {'\0'};
    sprintf(path, "./PARTITION/%s", name);
    FILE *f = fopen(path, "rb");
    // same start as partition() but we're reading from PARTITION this time and now writing
    
    if (f == NULL) {
    // if we failed to create the file f properly
        printf("Error: The partition \"%s\" was not found and/or read.\n", name);
        return 0;
        // we return 0 as the error signal, and print a message about the error
    }

    fread(&partitionStruct, sizeof(Partition), 1, f);
    
    for (int i = 0; i < 50; i++){
        fread(&p_name[i], sizeof(char), 1, f);
    }
    // reading the beginning of the file (info from struct partition) into our partition data structure

    
    fread(fat, sizeof(FAT), 20, f);
    // reading the next portion of the file (info from FAT) into our FAT data structure

    blocks = (int *) malloc(partitionStruct.total_blocks * sizeof(int));
    // malloc-ing block_size

    for (int i = 0; i < partitionStruct.total_blocks; i++){
        blocks[i] = -1;
    }
    // clearing the blocks in order to refresh them with values from the current FAT (that contains info from file f)
        
    for (int i = 0; i < 20; i++){
        for (int j = 0; j < 10; j++){
            if (fat[i].blockPtrs[j] != -1){
                blocks[fat[i].blockPtrs[j]] = i;
            }
        }
    }
    // refreshing the blocks array with the block pointers from FAT
        
    block_buffer = (char *) malloc(sizeof(char) * partitionStruct.block_size);
    // now creating the block_buffer (to eventually assign the block_size bytes to it)
    
    for (int i = 0; i < 5; i++){
        fpFATMap[i] = -1;
    }
    // we need to clear the fpFATMap area in order to refresh the mappings

    fclose(f);
    return 1;
    // If we've made it to the end of the function, we know we've run it successfully and can return 1
}

int openfile(char *name) {
// locates the input filename and returns the index of the corresponding FAT cell or creates a new entry for it if it doesn't exist

    int fpIndex = findOpenFPCell();
    if (fpIndex < 0){
        return -1;
    }
    // finding an open fp cell, and returning an error signal if none are open
        

    char path[100] = {'\0'};
    sprintf(path, "./PARTITION/%s", p_name);

    for (int i = 0; i < 20; i++) {
    // we'll loop through all of FAT to try and find the input filename
        if (strcmp(fat[i].filename, name) == 0) {
            // if we do find a match for the file name, we want to match an fp to it
            for (int index = 0; index < 5; index++){
            // We'll loop through the whole fp array
                if (fpFATMap[index] == i){
                     return i;
                    // and if we find the corresponding fp, we can return the index of the fat cell
                }
            }

            FILE *f = fopen(path, "r+b");
            // if we haven't returned an fp above, then we open the partition
            if (f == NULL) {
                printf("Error: The partition \"%s\" was not found and/or read.\n", name);
                return -1;
            }
            // similar error check as the last 2 functions
            
            fseek(f, sizeof(Partition) + 50 * sizeof(char) + 20 * sizeof(FAT), SEEK_SET);
            fp[fpIndex] = f;
            fpFATMap[fpIndex] = i;
            // then we shift the pointer to the file's initial block, and store it in the global fp array, finally returning the index of the corresponding FAT cell
            return i;
        }
    }

    int newFAT = findOpenFATCell();
    if (newFAT < 0){
        return -1;
    }
    // if we haven't returned anything within the for loop above, it means we couldn't find the filename, and must create a new entry in the table

    strcpy(fat[newFAT].filename, name);
    // fills the new entry's filename with the input filename

    FILE *f = fopen(path, "r+b");
    // opening the partition as instructed
    if (f == NULL) {
        printf("Error: The partition \"%s\" was not found and/or read.\n", name);
        return -1;
    } // same error checks as before

    fp[fpIndex] = f;
    fpFATMap[fpIndex] = newFAT;
    
    fseek(f, sizeof(Partition) + 50 * sizeof(char) + newFAT * sizeof(FAT), SEEK_SET);
    // fseek'ing to the block as instructed
    fwrite(&fat[newFAT], sizeof(FAT), 1, f);
    // writing the info from the new entry to the opened partition
    
    fflush(f);
    fseek(f, sizeof(Partition) + 50 * sizeof(char) + 20 * sizeof(FAT), SEEK_SET);
    
    return newFAT;
}

int readBlock(int file, int beginningFlag) {
// return block data as string from buffer_block with the FAT index from openFile()
    if (file < 0 || file >= 20){
    // checking for an invalid index number
        printf("Error: Invalid Index Number.\n");
        return -1;
    }

    else if (fat[file].current_location < 0){
    // checking if the file at the FAT index is empty
        printf("Error: The file you're searching for is empty.\n");
        return -1;
    }
    
    else if ((fat[file].blockPtrs[fat[file].current_location] < 0) && !beginningFlag){
    // checking if the file at the FAT index is at EOF
        printf("Error: The file is already at EOF.\n");
        return -1;
    }
    
    else {
        FILE *f = getFPAtIndex(file);
        // getting the file pointer in fp[] corresponding to the file we're working with 
        
        if (f == NULL) {
        // if getFPAtIndex() returns null, we haven't found the file
            printf("Error: The file you're searching for was not found.\n");
            return -1;
        }
        
        if (beginningFlag){
            fat[file].current_location = 0;
        }
        // the current_location is used to load the block_buffer with the data from the block, if not at end of file

        long block_position = partitionStruct.block_size * fat[file].blockPtrs[fat[file].current_location];
        // current_location is used as an index for blockPtrs as descriped in the pdf, to then get the position of the block
        fseek(f, block_position, SEEK_CUR);
        // setting the file position of f to the calculated position of our block of interest
        cleanBuffer();
        
        fread(block_buffer, sizeof(char), partitionStruct.block_size, f);
        // reads data form file f into buffer
        fat[file].current_location = (fat[file].current_location + 1) % 10;
        // icrementing the current location as instructed by the pdf
        
        fseek(f, sizeof(Partition) + 50 * sizeof(char) + 20 * sizeof(FAT), SEEK_SET);
        // then we shift the pointer to the file's initial block
        return 0;
    }
}

char *returnBlock() {
// return block data as string from block_buffer
    char *block_string = (char *) malloc((partitionStruct.block_size + 1) * sizeof(char));
    // intitializing and creating space for the string we want to return
    for (int i = 0; i <= partitionStruct.block_size; i++){
        block_string[i] = '\0';
        // initializing all of the cells as empty
    }
        
    for (int i = 0; i < partitionStruct.block_size; i++) {
    //looping through the block_string to update all of its cells
        if (block_buffer[i] == '0') {
        // checking if we're at EOF
            int eof = 1;
            for (int j = i + 1; eof && j < partitionStruct.block_size; j++){
                if (block_buffer[j] != '0'){
                    eof = 0;
                    // checking if there is another cell after the current one, and updating the eof status depending on what we find
                }
            }
            if (eof){
                break;
                // must end the loop if we're at eof
            }  
        }
        block_string[i] = block_buffer[i];
        // otherwise we can continue the loop and fill in the current block_string cell with data from the block_buffer
    }
    
    return block_string;
    // return the filled out string
}

int writeBlock(int file, char *data, int overwriteFlag) {
// sensitive to block size, write data to disk at blockID

    if (file < 0 || file >= 20) {
    // checking for an invalid index number
        printf("Error: Invalid Index Number.\n");
        return -1;
    }
    
    else if (strlen(data) > partitionStruct.block_size) {
    // there exists an error if the input data is larger than the size of the block (available space)
        printf("Error: The data is too large for the block of your current partition \"%s\".\n", p_name);
        return -1;
    }
    
    else if (fat[file].current_location < 0) {
    // if the file is empty
        int index = findEmptyBlock();
        // trying to find an open block
        
        if (index < 0) {
        // in the case where we fail to find one
            printf("Error: Failed to find any open blocks within the current partition \"%s\".\n", p_name);
            return -1;
        } 
        else {
            char path[100] = {'\0'};
            // initializing the path
            sprintf(path, "./PARTITION/%s", p_name);
            FILE *f = fopen(path, "r+b");
            // opening the partition
            
            if (f == NULL) {
            // if we failed to create the file f properly
                printf("Error: The partition \"%s\" was not found and/or read.\n", p_name);
                return -1;
                // we return -1 as the error signal, and print a message about the error
            }

            blocks[index] = file;
            
            long block_start_position = sizeof(Partition) + 50 * sizeof(char) + 20 * sizeof(FAT);
            long block_offset = partitionStruct.block_size * index;
            // two values to shift the position upon the disk
            fseek(f, block_start_position + block_offset, SEEK_SET);
            // shifting
            
            for (int i = 0; i < partitionStruct.block_size; i++) {
            // Writes data to the disk and updates the current location
                char curr_location;
                if(i >= strlen(data)){
                    curr_location =  '0';
                }
                else{
                    curr_location =  data[i];
                }
                // refreshing our current location
                fwrite(&curr_location, sizeof(char), 1, f);
                // and writing input data to it
            }
            
            fat[file].file_length = 1;
            fat[file].current_location = 1;
            fat[file].blockPtrs[0] = index;
            // filling out the other parameters of the fat index
            
            
            long entry_position = sizeof(Partition) + 50 * sizeof(char) + file * sizeof(FAT);
            // used to shift the pointer in file f to write to the proper fat entry
            
            fseek(f, entry_position, SEEK_SET);
            fwrite(&fat[file], sizeof(FAT), 1, f);
            // write the information to the entry in the FAT table
            fclose(f);
            return 0;
        }
    } 
    
    else {
    // remember, this is in the case where there's no errors, and the file IS NOT empty
        FILE *f = getFPAtIndex(file);
        if (f == NULL) {
        // getting the index returned nothing
            printf("Error: The file you're searching for could not be found.\n");
            return -1;
        }
        if (overwriteFlag) {
        // this is in the case where we overwrite a block with the data
            fat[file].current_location = 0;
            int firstFileIndex = 0;
            
            for (int i = 0; i < partitionStruct.total_blocks; i++) {
                if (blocks[i] == file) {
                    firstFileIndex = i;
                    // trying to find the matching index within the blocks to overwrite
                    break;
                }
            }
            for (int i = 0; i < partitionStruct.total_blocks; i++){
                if (blocks[i] == file){
                    blocks[i] = -1;
                    // we empty any block matching our input
                }    
            }
                
            blocks[firstFileIndex] = file;
            // and overwrite it with the input information
        }
        else {
        // in the case where we're not overwriting a block
            int index = findEmptyBlock();
            // we find an empty one
            blocks[index] = file;
            // place the file in this empty index
            fat[file].blockPtrs[fat[file].current_location] = index;
            // and mirror the update within FAT as well
        }
        
        long block_position = partitionStruct.block_size * fat[file].blockPtrs[fat[file].current_location];
        // current_location is used here as described in the pdf, to write the data arg into the partition
        fseek(f, block_position, SEEK_CUR);
        // shifting the pointer to the position of our block of interest
        
        for (int i = 0; i < partitionStruct.block_size; i++) {
            char current;
            if(i >= strlen(data)) {
                current = '0';
            }
            else{
                current = data[i];
            }
            fwrite(&current, sizeof(char), 1, f);
            // writing the data into the disk
        }
        
        fat[file].file_length = (overwriteFlag) ? 1 : fat[file].file_length + 1; 
        // updating FAT
        fat[file].current_location = (fat[file].current_location + 1) % 10;
        // incrementing current_location as instructed
        
        // Finally, writes all information to the FAT table entry
        long entry_position = sizeof(Partition) + 50 * sizeof(char) + file * sizeof(FAT);
        fseek(f, entry_position, SEEK_SET);
        // shifting the pointer to the position of the FAT table entry we're interested in writing to
        fwrite(&fat[file], sizeof(FAT), 1, f);
        // now write the information to it
        fflush(f);
        
        fseek(f, sizeof(Partition) + 50 * sizeof(char) + 20 * sizeof(FAT), SEEK_SET);
        // reset the pointer
        return 0;
    }
}
