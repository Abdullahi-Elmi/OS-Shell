//Abdullahi Elmi
//260727124

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
//including the necessary libraries

#include "kernel.h"
#include "shell.h"
#include "interpreter.h"
#include "shellmemory.h"
//including necessary header files for public methods from other files

int parse(char buffer[], char buf0[], char buf1[], char buf2[], char buf3[]) {
	int i = 0, j;

	// skip white space
	
	while(i < 1000 && buffer[i] == ' '){
		i++;
	}
	// extract argo0
	
	j = 0;

	while(i < 1000 && buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\r'){
		buf0[j] = buffer[i];
		j++; 
		i++;
	}

	buf0[j] = '\0';

	while(i < 1000 && buffer[i] == ' ' && buffer[i] != '\n' && buffer[i] != '\r'){
		i++;
	}
		
	// extract buf1
	
	j = 0;

	while(i < 1000 && buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\r'){
		buf1[j] = buffer[i];
		j++; 
		i++;
	}

	buf1[j] = '\0';

	while(i < 1000 && buffer[i] == ' ' && buffer[i] != '\n' && buffer[i] != '\r'){
		i++;
	}
		
	// extract buf2
	
	j = 0;

	if (buffer[i] == '[') {
		i++;
		while (i < 1000 && buffer[i] != ']' && buffer[i] != '\n' && buffer[i] != '\r') {
		// multiple word case
			buf2[j] = buffer[i];
			j++;
			i++;
		}
		i++;
	} 
	else {
		while (i < 1000 && buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\r') {
		// single word case (not considering the '[]')
			buf2[j] = buffer[i];
			j++;
			i++;
		}
	}

	buf2[j] = '\0';

	while(i < 1000 && buffer[i] == ' ' && buffer[i] != '\n' && buffer[i] != '\r'){
		i++;
	}
		
	// extract buf3
	
	j = 0;

	while(i < 1000 && buffer[i] != ' ' && buffer[i] != '\n' && buffer[i] != '\r'){
		buf3[j] = buffer[i];
		j++; 
		i++;
	}

	buf3[j] = '\0';

	while(i < 1000 && buffer[i] == ' ' && buffer[i] != '\n' && buffer[i] != '\r'){
		i++;
	}
		
	// debug code: printf("[%s] [%s] [%s]\n", buf0, buf1, buf2);

	// check if there is more data (illegal)
	
	if (i == 1000 || buffer[i] == '\n' || buffer[i] == '\r' || buffer[i] == '\0'){
		return 1; // completed successfully (true)
	}
	else{
		return 0;
	}
}

int prompt(char buffer[]) {

	char buf0[100], buf1[100], buf2[100], buf3[100];
	int result = 0;

	result = parse(buffer, buf0, buf1, buf2, buf3);

	if (!result){
		printf("Error: Command %s has too many characters\n",buffer);
	}
	
	result = interpreter(buf0, buf1, buf2, buf3);

	if (result == 99){
		return 99;
	}
	if (result == 98){
		printf("Command does not exist\n");
	}
	if (result ==  1){
		printf("Wrong number of set parameters\n");
	}
	if (result ==  2){
		printf("Wrong number of print parameters\n");
	}
	if (result ==  3){
		printf("Run is  missing filename\n");
	}
	if (result ==  4){
		printf("Script filename not found\n");
	}
	if (result ==  5){
		printf("The exec progam was not found\n");
	}
	if (result == 6){
		printf("Wrong number of mount parameters\n");
	}
	if (result == 7){
		printf("Wrong number of write parameters\n");
	}	
	if (result == 8){
		printf("Wrong number of read parameters\n");
	}
	return 0;
}

int shell() {
	char buffer[1000];
	int done = 0;
	int result = 0;

	initShellMemory();

	printf("Welcome to the Elmi shell!\n");
	printf("Version 4.0 Created April 2019\n");

	while(!done) {
		
		if (isatty(0)) {
			printf("$ ");
		}
		
		if (fgets(buffer, 999, stdin) == NULL && isatty(0) == 0) {
			freopen("/dev/tty", "r", stdin);
		} 
		else {
			result = prompt(buffer);
			if (result == 99){
				done = 1;
			}
		}
	}

	printf("Good bye.\n");

	return 0;
}
