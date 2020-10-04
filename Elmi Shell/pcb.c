//Abdullahi Elmi
//260727124

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
//including the necessary libraries

#include "pcb.h"

PCB *head = NULL, *tail = NULL;

PCB *makePCB(FILE *f, int startAddressRAM){

	if (f == NULL){
        return NULL;
    }

    PCB *pcb = (PCB *) malloc(sizeof(PCB));
    //create the pcb the function wants
	
	if (pcb == NULL){
        return NULL;
    }

	pcb->PC = f;
	pcb->start = startAddressRAM;
    pcb->next = NULL;
    //initialize the pcb with all of the values now, start and file pointer are passed in by the function call, and the next pcb is initialized as null
	
	for (int i = 0; i < 10; i++){
        pcb->pageTable[i] = -1;
    }
	//initialize the page table with an int not pointing to a frame, signifying it's currently empty.

}

void addToReady(PCB *p) {

	p->next = NULL;

	if (head == NULL) {
		head = p;
		tail = p;
	} 
    else {
		tail->next = p;
		tail = p;
	}
}

PCB *getHead() {
	return head;
}

PCB *getFromReady() {

    if (head == NULL){
        return NULL;
    }
	
	PCB *temp = head;
	head = head->next;
	temp->next = NULL;

	if (head == NULL){
        tail = NULL;
    }

	return temp;
}

void printPCB() {
	PCB *temp = head;

	printf("Addr:");

	while(temp!=NULL) {
		printf(" %d",temp->start);
		temp = temp->next;
	}

	printf("\n");
}
