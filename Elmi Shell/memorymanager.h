//Abdullahi Elmi
//260727124

#include "pcb.h"

int launcher(FILE *f);
int countTotalPages(FILE *f);

FILE *findPage(int pageNumber, FILE *f);
int findFrame();
int findVictim(PCB *p);

int updateFrame(int frameNumber, int victimFrame, FILE *pg);
int updatePageTable(PCB *p, int pageNumber, int frameNumber, int victimFrame);
