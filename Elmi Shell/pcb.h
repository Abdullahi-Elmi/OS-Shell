//Abdullahi Elmi
//260727124

#ifndef PCB_H
#define PCB_H

typedef struct PCB {
    FILE *PC;
    struct PCB *next;
    int PC_offset;
    int start;
    int PC_page;
	int pageTable[10];
	int pages_max;
} PCB;

PCB *makePCB(FILE *p, int startAddressRAM);
void addToReady(PCB *p);
PCB *getHead();
PCB *getFromReady();
void printPCB();

#endif
