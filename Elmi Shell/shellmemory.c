//Abdullahi Elmi
//260727124

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//including necessary libraries

#include "shell.h"
#include "shellmemory.h"
//including necessary header files for public methods from other files

ShellMemory *memoryHead, *memoryTail;

void initShellMemory() {
	memoryHead = NULL;
	memoryTail = NULL;
}

void add(const char *var, const char *value) {
	ShellMemory *temp = find(var);

	// does the var already exist? if so replace, otherwise add new
	if (temp != NULL) { // overwrite
		free(temp->value);
		temp->value = strdup(value);
	}
	else {
		temp = (ShellMemory *) malloc(sizeof(ShellMemory));
		temp->var = strdup(var);
		temp->value = strdup(value);
		temp->next = NULL;

		if (memoryHead == NULL){
            memoryHead = temp;
            memoryTail = temp;
        }
		else {
			memoryTail->next = temp;
			memoryTail = temp;
		}
	}
}

char *get(const char *var) {
	ShellMemory *temp = find(var);

	if(temp != NULL){
        return temp->value;
    }
    else{
        return NULL;
    }
}

ShellMemory* find(const char *key){
    ShellMemory *temp = memoryHead;

    while(temp != NULL) {
		if (strcmp(key, temp->var) == 0){
            return temp;
        }
		temp = temp->next;
	}

	return NULL;
}

void clearShellMemory() {
    
	ShellMemory *temp = memoryHead;

	while (temp != NULL) {
		free(temp->var);
		free(temp->value);

		ShellMemory *temp2 = temp;
		temp = temp->next;
		free(temp2);
	}
}
