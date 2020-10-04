//Abdullahi Elmi
//260727124

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//including necessary libraries

#include "shell.h"
#include "kernel.h"
#include "ram.h"

void initRAM() {
	for (int i = 0; i < 10; i++){
        ram[i] = NULL;
    }
}

int addToRAM(FILE *p){

    int i;

    if (p == NULL){
        return -1; // error
    }

    for(i = 0; i < 10 && ram[i] != NULL; i++); // find next available space

    if (i < 10) {
		ram[i] = p;
		return i; // position in RAM
	}
	else{
        return -2; // out of memory error
    }
}

void clearRAM(int startAddressRAM) {
	ram[startAddressRAM] = NULL;
}