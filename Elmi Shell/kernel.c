//Abdullahi Elmi
//260727124

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
//including the necessary libraries

#include "kernel.h"
#include "cpu.h"
#include "ram.h"
#include "shell.h"
#include "pcb.h"
#include "memorymanager.h"
#include "shellmemory.h"
#include "DISK_driver.h"

int myinit(FILE *p){
    PCB* pcb;
    
	int index_Pg1 = addToRAM(p);
	int index_Pg2 = -1;
    int maxPages = countTotalPages(p);
    
	if (maxPages >= 2) {
		FILE *pPage2 = findPage(1, p);
        //pPage2 is pointing to the first character of the desired page
		index_Pg2 = addToRAM(pPage2);
	}

	if (index_Pg1 >= 0) {

		pcb = makePCB(p, index_Pg1);

		if (pcb != NULL) {

			pcb->PC = p;
            //the pointer was passed in to the myinit function
			pcb->pageTable[0] = index_Pg1;
            //the 1st page is placed in the first position of the page table
			if (index_Pg2 >= 0){
				pcb->pageTable[1] = index_Pg2;
                //if we found the 2nd page in the earlier if statement, place it in the next position of the page table
            } 
            pcb->PC_page = 0;
			pcb->PC_offset = 0;
			pcb->pages_max = maxPages;
            //setting all of the values for the pcb if there's a page

			addToReady(pcb);

			/*
			if (index_Pg2 >= 0){
				pcb->pageTable[1] = index_Pg2;
                //if we found the 2nd page in the earlier if statement, place it in the next position of the page table
            } Might be an issue with this, editing out for now*/

			
            //adding the now filled out pcb the ready queue
			return 1;
		}
	}

	return 0;
}

void terminate(PCB *p) {
	clearRAM(p->start);
	free(p);
}

void boot() {
	initRAM();
	initCPU();
    //initialize the ram and cpu when booting

	system("rm -r BackingStore");
	system("mkdir BackingStore");
    // Recreating the backing store
}

void clear() {
	clearShellMemory();
	clearCPU();
    //clear shell memeory and the cpu
	cleanDisk();
	system("rm -rf BackingStore/*");
    // Emptying the backing store
}

int scheduler(){

    PCB *pcb;
	int result;

	while (getHead() != NULL) {

		pcb = getFromReady();

		if (pcb != NULL) {

			setCPU(pcb->PC, pcb->PC_offset);
			result = runCPU(2);

			if (result == 99) {
				terminate(pcb);
				return 99;
				//terminate and quit the program if that's the code returned from the result.
			}

			else if (result == 96 || result == 97) {
				pcb->PC_page++;
				//move to the next page, as the result might've been (switch page)
				
				if (pcb->PC_page >= pcb->pages_max) {
					terminate(pcb);
					//terminate if we're at the end of the total pages
					continue;
					//and skip the rest of the while loop, it's no longer relevant in this scenario
				}
				
				int frameNumber = pcb->pageTable[pcb->PC_page];

				if (frameNumber < 0) {
					//in the case where a page fault occurs

					FILE *pg = findPage(pcb->PC_page, pcb->PC);
					frameNumber = findFrame();

					int victimNumber;

					if(frameNumber == -1){
						victimNumber = findVictim(pcb);
					}
					else{
						victimNumber = -1;
					}

					updatePageTable(pcb, pcb->PC_page, frameNumber, victimNumber);
					updateFrame(frameNumber, victimNumber, pg);

					if (frameNumber < 0){
						frameNumber = victimNumber;
					}
				}
				
				pcb->PC = ram[frameNumber];
				pcb->PC_offset = 0;
			} 
            
            else{
				pcb->PC_offset += 2;
            }

			addToReady(pcb);
		}
    }

	for (int i = 0; i < 10; i++){
		ram[i] = NULL;
	}
	
	return 0;
}

int main(int argc, const char* argv[]){
    boot();

	printf("Kernel 4.0 loaded!\n");

	int result = shell();
	clear();

	return result;
}
