#include "pipeline.h"
#include <unistd.h>
void updatelatch() {

	ifid_latch[1] = ifid_latch[0];
	idex_latch[1] = idex_latch[0];
	exmem_latch[1] = exmem_latch[0];
	memwb_latch[1] = memwb_latch[0];
}
void setting() {

	pc = 0;
	Reg[31] = 0xFFFFFFFF;//r31 == ra register
	Reg[29] = 0x100000;//r29 == sp stack pointer
	cycle = 0;
	miss = 0;
	//using memset functon(in string.h) to initialize latch to 0 at first time.
	memset(ifid_latch,0,sizeof(IFID_latch));
	memset(idex_latch,0,sizeof(IDEX_latch));
	memset(exmem_latch,0,sizeof(EXMEM_latch));
	memset(memwb_latch,0,sizeof(MEMWB_latch));
	numofinst = 0;
	numM = 0;
	numR = 0;
	numB = 0;
	numNB = 0;
	numJ = 0;
}
void initializeMemory() {

	FILE *fp = NULL;
	fp = fopen("input4.bin", "rb");
	if (fp == NULL) {
		perror("no such input file");
	}

	int h1, h2, h3, h4;
	int ret;
	int inst;
	int data = 0;
	int index = 0;

	while (1) {
		h1 = 0;
		h2 = 0;
		h3 = 0;
		h4 = 0;
		inst = 0;
		// reading file to the eof
		ret = fread(&data, sizeof(data), 1, fp);
		if (ret == 0) break;

		h1 = ((data & 0xff) << 24);
		h2 = ((data & 0xff00) << 8);
		h3 = ((data & 0xff0000) >> 8) & 0xff00;
		h4 = ((data & 0xff000000) >> 24) & 0xff;


		inst = h1 | h2 | h3 | h4;

		Memory[index / 4] = inst;
		numofinst++;
		printf("[0x%08X] 0x%08x\n", index, Memory[index / 4]);
		index = index + 4;
	}
	fclose(fp);
}

int main() {

	setting();
	initializeMemory();
	while (1) {

		if(pc == 0xFFFFFFFF){
			//when -1 come in at fetch stage, remain cycles are mem write update write.
			printf("■ ■ ■ ■ ■ ■ ■ ■ ■ cycle%d ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ \n",cycle);
			writeback(memwb_latch[1]);
			printf("pc: 0x%x\n",pc);
			printf("********************terminate*****************\n");
			printout();
			break;
		}
		writeback(memwb_latch[1]);
		fetch();
		decode(ifid_latch[1]);
		execute(idex_latch[1]);
		memoryaccess(exmem_latch[1]);
		updatelatch();
	}

}
void printout()
{
	printf("******************result*********************\n");
	printf("reg[2] (final return)value: %d\n",Reg[2]);
	printf(" # of instructions: %d\n",numofinst);
	printf(" # of memory operation instructions: %d\n",numM);
	printf(" # of register operation instructions: %d\n",numR);
	printf(" # of branch instructions: %d\n",numB);
	printf(" # of not-taken branches: %d\n",numNB);
	printf(" # of jump instructions: %d\n",numJ);
	printf("*********************************************\n");	
}




