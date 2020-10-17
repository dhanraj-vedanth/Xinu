#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>
#include <lab0.h>


void printsegaddress(){

	printf("Current: etext[0x%08x]: 0x%08x, edata[0x%08x]: 0x%08x, ebss[0x%08x]: 0x%08x\n", &etext,*(&etext), &edata,*(&edata), &end,*(&end));
//	printf("\nText's end: 0x%08x\n", &etext);
//	printf("Data's end: 0x%08x\n", &edata);
	//printf("BSS segment's end: 0x%08x\n", &ebss);
//	printf("Someother end: 0x%08x\n",&end);
	
//	printf("*******************\n");
	printf("Preceding: etext[0x%08x]: 0x%08x  edata[0x%08x]: 0x%08x and ebss[0x%08x]: 0x%08x\n",&etext -1, *(&etext -1),&edata -1,  *(&edata -1),&end -1, *(&end -1) );
	printf("After: etext[0x%08x]: 0x%08x  edata[0x%08x]: 0x%08x and ebss[0x%08x]: 0x%08x\n",&etext +1, *(&etext +1),&edata +1,  *(&edata +1),&end +1, *(&end +1) );
	printf("\t***********END OF printsegaddress()******\n");
}

