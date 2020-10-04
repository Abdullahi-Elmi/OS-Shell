//Abdullahi Elmi
//260727124

#include <stdio.h>
#include <stdlib.h>
//including the necessary libraries

#include "kernel.h"
#include "shell.h"
//including the necessary files

typedef struct CPU {
	FILE *IP;
	char IR[1000];
	int offset;
	int quanta;
} CPU;
CPU *cpu;

void initCPU() {
//method to initialize the CPU
	cpu = (CPU *) malloc(sizeof(CPU));

	cpu->IP = NULL;
	cpu->IR[0] = '\0';
	
	cpu->offset = 0;
	cpu->quanta = 2;
	//very basic running, just set base values for all 4 fields
}

void setCPU(FILE *PC, int offset) {
//method to allow you to set changes to the CPU
	cpu->IP = PC;
	cpu->IR[0] = '\0';
	cpu->offset = offset;
	//we change the offset and pointer, the array is reset
}

void clearCPU() {
	free(cpu);
	//method native to C, to deallocate the memory assigned in cpu
}

int runCPU(int quanta){

	cpu->quanta = quanta;
	//set the inputted quanta

	while (cpu->quanta > 0) {
		
		fgets(cpu->IR, 999, cpu->IP);
		//IR and IP should be set previously, and so fgets should get the necessary text

		int result = prompt(cpu->IR);
		if (result != 0){
			return result;
		}
		//same from ass2 solutions

		if (feof(cpu->IP)){
			return 97;
		}
		else {
			if (fgetc(cpu->IP) == EOF){
				return 97;
			}
			fseek(cpu->IP, -1, SEEK_CUR);
			//EOF and SEEK_CUR native to C. Common else handle
		}
		//added, not in the ass2 solutions. Testing the end-of-file indicator in the stream

		cpu->quanta--;
		cpu->offset++;
		//the offset is basically the usual pointer in loops in this case, this loop should only run twice

		if (cpu->offset >= 4){
			return 96;	
			//the page must be changed if the offset exceeds the size of the pages
		}
	}

	return 0;
}
