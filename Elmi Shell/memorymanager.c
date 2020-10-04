//Abdullahi Elmi
//260727124

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
//including the necessary libraries

#include "kernel.h"
#include "shell.h"
#include "ram.h"
#include "pcb.h"
//including the necessary .h files

int fileIndex = 0;

int launcher(FILE *f) {

    char filePath[50];
    sprintf(filePath, "./BackingStore/p%d.txt", fileIndex++);
    //rename the file with p + same index as the mirrored file
    FILE *backingStore = fopen(filePath, "w+");
    //this is to write into the new p(i) file

    if (backingStore == NULL) {
        //there was an error, the file wasn't created within the backingsotre
        printf("Error! There was an issue copying the file into the BackingStore\n");
        return 0;
    }

    char line[1000] = {'\0'};
    char content = fgetc(f);
    //copy the content from the testfile into the array

    while (content != EOF) {
    //until we reach the end of the file
        fputc(content, backingStore);
        content = fgetc(f);
        //loop through and copy all of the content from testfile(i) to p(i)
    }

    rewind(backingStore);
    //function native to c. Resets the file position to the beginning
    fclose(f);

    return myinit(backingStore);
}

int countLine(FILE *f) {
//helper function require to count the lines (a feature we need for countTotalPages())

    int count = 0;
    if (feof(f)){
        return count;
        //file's empty in this case, so lines = 0
    }
    else{
        count++;
        //we know lines >= 1 now
    }

    for (char c = fgetc(f); c != EOF; c = fgetc(f)) {
        if (c == '\n' && fgetc(f) != EOF) {
            count++;
            //fgetc(f) != EOF is to avoid double counting the final line
            fseek(f, -1, SEEK_CUR);
            //resetting the "fgetc(f) != EOF" by 1 if this isn't the final line
        }
    }

    rewind(f);
    //same use as in launcher() that I explained in there
    return count;
}

int countTotalPages(FILE *f) {

    int lineNumber = countLine(f);
    //using the helper function to count the number of lines
    
    if(lineNumber % 4 == 0){
        return lineNumber / 4;
    }
    else{
        return lineNumber / 4 + 1;
    }
    //just calculating what to return as described in the pdf, the page and line numbers are related by a factor of 4, hence the modulo 4
}

FILE *findPage(int pageNumber, FILE *f) {

    char fName[50];
    char link[50];
    
    sprintf(link, "/proc/self/fd/%d", fileno(f));
    ssize_t length;
    //had issues with the single line in the pdf to duplicate, found a different style for using fileno() to duplicate with a link
    if ((length = readlink(link, fName, 49)) != -1){
        fName[length] = '\0';
    }
    
    FILE *fp2 = fopen(fName, "rt");
    //fp2 is the duplicate
    char line[1000];
    
    for (int i = 0; i < 4 * pageNumber; i++){
        fgets(line, 999, fp2);
        //moving the pointer to the location specified by the pdf
    }
    
    return fp2;
}

int findFrame() {

    for (int i = 0; i < 10; i++){
        if (ram[i] == NULL){
            return i;
        }
    }
    //generic "for loop" for FIFO
    
    return -1; //specified by the pdf
}

int findVictim(PCB *p) {

    srand(time(NULL));
    int frameNumber = rand() % 10;
    //random number generator gives us a frame number

    int found = 0;
    int exists = 0;
    //the two integers keep track of the status' we're interested in

    while (!found) {

        for (int i = 0; i < 10; i++) {
            if (p->pageTable[i] == frameNumber) {
                exists = 1;
                //iterate through the page table to try and find the matching frame number, and set the status appropriately indicating
                //it exists
                break;
                //we're done with the for loop at this point
            }
        }
        
        if (exists) {
            //if we do find the frame number exists from the above for loop
            int newFrameNumber = (frameNumber + 1) % 10;
            frameNumber = newFrameNumber;
            //iteratively incrementing modulo-wise as indicated by the pdf since the frame number DOES exist in the page table
            exists = 0;
            //resetting for next iteration of the while loop
        } 
        else{
            found = 1;
        }
    }

    return frameNumber;
    //the framenumber doesn't belong to the page table so we return it
}

int updateFrame(int frameNumber, int victimFrame, FILE *page) {
    if(frameNumber == -1){
        ram[victimFrame] = page;
    }
    else{
        ram[frameNumber] = page;
    }
    return 0;
    //done exactly as described in the pdf
}

int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame) {
    if(frameNumber == -1){
        p->pageTable[pageNumber] = victimFrame;
    }
    else{
        p->pageTable[pageNumber] = frameNumber;
    }
    return 0;
    //done exactly as described in the pdfs
}
