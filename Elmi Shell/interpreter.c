//Abdullahi Elmi
//260727124

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//including the necessary libraries

#include "kernel.h"
#include "memorymanager.h"
#include "shell.h"
#include "shellmemory.h"
#include "DISK_driver.h"
#include "IOscheduler.h"
//including the necessary files

int run(char *filename) {
	
	FILE *ptr = fopen(filename,"rt");
	char buffer[1000];
	int result = 0;
	
	if (ptr == NULL){
		return 4;
	}
	
	do {
		fgets(buffer, 999, ptr);

		if (strlen(buffer) > 1){
			result = prompt(buffer);
		}
		if (result == 99){
			break;
		}
	} while (!feof(ptr));
	//I don't normally use do-while, but a normal while loop seemed to cause issues where the last line of the txt files weren't being executed,
	//and the ordering of fgets and feof seemed to be the root of the issue

	fclose(ptr);
	if (result == 99){
		return 99;
	}
	
	return 0;
}

int exec(char *file1, char *file2, char *file3) {

	int result = 0;
	FILE *p1, *p2, *p3;

	if (strlen(file1) < 1){
		file1 = NULL;
	}
		
	if (strlen(file2) < 1){
		file2 = NULL;
	}
		
	if (strlen(file3) < 1){
		file3 = NULL;
	}

	// Add to ready queue each  unique programs
	if (file1 != NULL) {
		p1 = fopen(file1, "rt");
		result = launcher(p1);
	}
	if (file2 != NULL) {
		p2 = fopen(file2, "rt");
		result = launcher(p2);
	}
	if (file3 != NULL) {
		p3 = fopen(file3, "rt");
		result = launcher(p3);
	}

	if (result > 0){
		return scheduler();
	}
	else{
		return 0;
	}
}

int Mount(char *partition_name, char *input_block_count, char *input_block_size) {
	// Checks if the given partition exists
	char path[100] = {'\0'};
	sprintf(path, "./PARTITION/%s", partition_name);
	
	if (access(path, F_OK) == -1) {
		int block_size = atoi(input_block_size);
		int block_count = atoi(input_block_count);
		// convert the block variables to integers and check the partition's existence
		
		if (!partition(partition_name, block_size, block_count)){
		// in the case where the partition doesn't exist
			return 9;
			// partition error code
		}
	}

	if (!mount(partition_name)){
		return 10;
		// mounting error code
	}

	return 0;
}

int Write(char *filename, char *contents) {
	
	char *result = IOscheduler(contents, filename, 1);
	
	if (result == NULL) {
		printf("Error: Failed to write to the file \"%s\".\n", filename);
		return 11;
		// writing error code
	}

	return 0;
}

int Read(char *filename, char *variable) {
	
	char *data = IOscheduler(NULL, filename, 0);
	
	if (data == NULL) {
		printf("Error: Failed to read from the file \"%s\".\n", filename);
		return 12;
		// reading error code
	}
	
	add(variable, data);
	free(data);

	return 0;
}



int interpreter(char buf0[], char buf1[], char buf2[], char buf3[]) {
	
	int result = 0; 

	if (strcmp(buf0, "help") == 0) {
		printf("Legal commands:\n");
		printf("help            		 ||  Displays all the commands\n");
		printf("quit            		 ||  Exits / terminates the shell with \"Bye!\"\n");
		printf("set VAR STRING  		 ||  Assigns a value to shell memory\n");
		printf("print VAR       		 ||  Prints the STRING assigned to VAR\n");
		printf("run SCRIPT.TXT  		 ||  Executes the file SCRIPT.TXT\n");
		printf("exec p1 p2 p3   		 ||  Executes concurrent programs\n");
		printf("Mount partition_name block_count block_size  	 ||  Creates a new partition and mount\n");
		printf("Write filename [data]    ||  Writes data to the file\n");
		printf("Read filename variable   ||  Reads data from the file\n");

		result = 0;
	} 
	else if (strcmp(buf0,"quit")==0) {
		result = 99; // exit shell code
	}
	else if (strcmp(buf0,"set")==0) {
		if (strlen(buf1)<1 || strlen(buf2)<1) return 1; // set error

		add(strdup(buf1), strdup(buf2));
	}
	else if (strcmp(buf0,"print")==0) {
		if (strlen(buf1)<1) return 2; // print error

		char *valToPrint = get(buf1);
		
		if (valToPrint == NULL){
			printf("Error! The variable you'd like to print cannot be found");
		}	
		else{
			printf("%s\n", valToPrint);
		}
	}
	else if (strcmp(buf0,"run")==0) {
		if (strlen(buf1)<1) return 3; // run error

		result = run(buf1);
	}
	else if (strcmp(buf0,"exec")==0) {
		if (strlen(buf1)<1) return 5; // exec error
		result = exec(buf1, buf2, buf3);
	}
	else if (strcmp(buf0, "Mount") == 0) {
		if (strlen(buf1) < 1 || strlen(buf2) < 1 || strlen(buf3) < 1){
			return 6;
		}
		result = Mount(buf1, buf2, buf3);
	} 
	else if (strcmp(buf0, "Write") == 0) {
		if (strlen(buf1) < 1 || strlen(buf2) < 1){
			return 7;
		}
		result = Write(buf1, buf2);
	} 
	else if (strcmp(buf0, "Read") == 0) {
		if (strlen(buf1) < 1 || strlen(buf2) < 1){
			return 8;
		}
		result = Read(buf1, buf2);
	}
	else {
		result = 98; // command does not exist
	}

	return result;
}
