#include<errno.h>
#include<stdio.h>
/*
 project 2 : single cycle calculator
 name: An somin
 student ID: 32182490 */

void single_cycle(FILE* fp);

int Memory[0x100000/4];//Memory
int pc = 0;//program counter
int reg[32];//cpu register

int main(){
	FILE *fp = NULL;
	
	//simple file single cycle
	fp = fopen("simple.bin", "rb");
	if(fp == NULL){
		perror("no such input file");
	}
	printf("*****simple file single cycle*****\n");
	single_cycle(fp);
/*
	//simple2 file single cycle
	fp = fopen("simple2.bin","rb");
	if(fp == NULL){
		perror("no such input file");
	}
	printf("*****simple2 file single cycle*****\n");
	single_cycle(fp);

	//simple3 file single cycle
	fp = fopen("simple3.bin","rb");
	if (fp == NULL) {
		perror("no such input file");
	
	}
	printf("*****simple3 file single cycle*****\n");
	single_cycle(fp);

	//simple4 file single cycle
	 fp = fopen("simple4.bin","rb");
	 if (fp == NULL) {
		perror("no such input file");
		}
	printf("*****simple4 file single cycle*****\n");
	 single_cycle(fp);

	//gcd file single cycle
	 fp = fopen("gcd.bin","rb");
	 if (fp == NULL) {
		 perror("no such input file");
	 }
	 printf("*****gcd file single cycle*****\n");
	 single_cycle(fp);
	
	//fib file single cycle
	 fp = fopen("fib.bin","rb");
	 if (fp == NULL) {
		perror("no such input file");
	 }
	 printf("*****fib file single cycle*****\n");
	 single_cycle(fp);

	//input4 file single cycle
	fp = fopen("input4.bin", "rb");
	if(fp == NULL){
		 perror("no such input file");
	}
	printf("*****input4 file single cycle*****\n");
	single_cycle(fp);
	*/

	//fib2 file single cycle ==> to implement optional instruction 'jalr'
/*	 fp = fopen("fib2.bin","rb");
	 if (fp == NULL) {
		perror("no such input file");
	 }
	 printf("*****fib2 file single cycle*****\n");
	 single_cycle(fp);
*/
	return 0;
}

void single_cycle(FILE* fp){
	int h1, h2, h3, h4;
	int ret;
	int inst;
	int data =0;
	int index = 0;
	int jumping_op;//opcode that can jump to another instruction => can pass one struction because of pc = pc+4, signal not jumping instruction.
	pc = 0;
	reg[31] = 0xFFFFFFFF;//r31 == ra register
	reg[29] = 0x100000;//r29 == sp stack pointer
									    
	while(1){
		h1 = 0;
		h2 = 0;
		h3 = 0;
		h4 = 0;
		inst = 0;
		// reading file to the eof
		ret = fread(&data, sizeof(data),1, fp);
		if(ret == 0) break;
		
		h1 = ((data & 0xff) << 24);
		h2 = ((data & 0xff00) << 8);
		h3 = ((data & 0xff0000) >> 8) & 0xff00;
		h4 = ((data & 0xff000000) >> 24) & 0xff;

		inst = h1 | h2 | h3 | h4;

		Memory[index/4] = inst;
		printf("[0x%08X] 0x%08x\n",index, Memory[index/4]);
		index = index +4;

	}

	fclose(fp);

	 int numofinst = 0;
	 int numR = 0;//number of R type instructions
	 int numI = 0;//number of I type instructions
	 int numJ = 0;//number of J type instructions
	 int numM = 0;//number of Memory access instructions
	 int numB = 0;//number of taken branches instructions

	while(1){
		//fetch from memory
		inst =  Memory[pc/4];
		printf("----------------------------------------------------------------------\n");
		printf("[Fetch] 0x%08x\n", inst);
		//decode
		int opcode = (inst & 0xfc000000) >> 26;
		int rs 	   = (inst & 0x03e00000) >> 21;
		int rt     = (inst & 0x001f0000) >> 16;
		int rd     = (inst & 0x0000f800) >> 11;
		int shamt = (inst & 0x000007c0) >> 6;
		int func   = (inst & 0x0000003f) >> 0;
		int imm    = (inst & 0x0000ffff) >> 0;
		int simm   = (imm >> 15) ? (0xffff0000| imm) : imm;
		int address = (inst & 0x03ffffff) >> 0;
		jumping_op = 0;

		printf("[Decode] opcode(%x) rs: %d rt: %d rd: %d func(0x%x) simm(%d) ", opcode, rs, rt, rd, func,simm);

		switch(opcode){// elements that can be changed in this program: pc, reg, mem value..
			case 0x9://addiu
				printf("<addiu>\n");
				 printf("pc : 0x%x\n",pc);
				printf("reg[%d] = reg[%d]+simm(0x%x)\n",rt, rs,simm);
				printf("reg[%d] : 0x%x ----> ",rt, reg[rt]);// print if the value is changed
				reg[rt] = reg[rs] + simm;//execute, write back
				printf("reg[%d] : 0x%x, %d\n",rt, reg[rt],reg[rt]);
				printf("\n");
				numI++;
				break;
			case 0x8://addi
				  printf("<addi>\n");
				   printf("pc : 0x%x\n",pc);
				  printf("reg[%d] = reg[%d]+simm(0x%x)\n",rt, rs,simm);
				  printf("reg[%d] : 0x%x ----> ",rt, reg[rt]);// print if the value is changed
				  reg[rt] = reg[rs] + simm;//execute, write back
		          printf("reg[%d] : 0x%x (%d)\n",rt, reg[rt],reg[rt]);
				  printf("\n");
				  numI++;
				  break;
			case 0x2b://sw
				printf("<sw>\n");
				 printf("pc : 0x%x\n",pc);
				printf("Memory[reg[%d]+0x%x] = reg[%d]\n",rs,simm,rt);
				printf("Memory[0x%08x] : 0x%x ----> ",(reg[rs]+simm)/4, Memory[(reg[rs]+simm)/4] );
				Memory[(reg[rs]+simm)/4] = reg[rt];//access memory
				printf("Memory[0x%08x] : reg[%d](0x%x)\n",(reg[rs]+simm)/4,rt,reg[rt] );
				printf("\n");
				numI++;
				numM++;
				break;
			case 0://addu, add , sub, subu, jr , and ,nor, or ,slt, sltu, sll, srl
				if(func==0x21)
				{
					printf("<addu>\n");//addu
					 printf("pc : 0x%x\n",pc);
					printf("reg[%d] = reg[%d]+reg[%d]\n",rd,rt,rs);
					printf("reg[%d] : 0x%x ----> ",rd, reg[rd]);
					reg[rd] = reg[rt]+reg[rs];//execute, write back
					printf("reg[%d] :0x%x (%d)\n", rd, reg[rd],reg[rd]);
					numR++;
				}
				else if(func==0x20){//add
					   printf("<add>\n");
					    printf("pc : 0x%x\n",pc);
					   printf("reg[%d] = reg[%d]+reg[%d]\n",rd,rt,rs);
					   printf("reg[%d] : 0x%x ----> ",rd, reg[rd]);
				       reg[rd] = reg[rt]+reg[rs];//execute, write back
				       printf("reg[%d] : 0x%x (%d)\n", rd, reg[rd],reg[rd]);
					   numR++;
				}
				else if(func==0x22||func==0x23)//sub, subu
				{
					   if(func==0x22) printf("<sub>\n");
					   else printf("<subu>\n");
					    printf("pc : 0x%x\n",pc);
					   printf("reg[%d] = reg[%d]-reg[%d] ----> ",rd,rs,rt);
					   printf("reg[%d] : 0x%x\n",rd, reg[rd]);
					   reg[rd] = reg[rs]-reg[rt];//execute, write back
					   printf("reg[%d] : 0x%x (%d)\n", rd, reg[rd],reg[rd]);
					   numR++;
				}
				else if(func == 0x08){//jr
					printf("<jr>\n");
					printf("pc : 0x%x\n",pc);
					printf("pc = reg[%d]\n",rs);
					printf("pc : 0x%x ----> ",pc);
					pc = reg[rs];// jump to the ra pc value
					printf("pc : 0x%x\n",reg[rs]);
					jumping_op = 1;
					numR++;

	             }
				else if(func == 0x24){//and
					printf("<and>\n");
					printf("pc : 0x%x\n",pc);
					printf("reg[%d] = reg[%d] & reg[%d]\n",rd, rs,rt);
					printf("reg[%d] : 0x%x ----> ",rd, reg[rd]);
					reg[rd] = reg[rs]&reg[rt];//execute, write back
					printf("reg[%d] : 0x%x (%d)\n", rd, reg[rd],reg[rd]);
					numR++;
				}
				else if(func == 0x27)//nor
				{
					printf("<nor>\n");
					printf("pc : 0x%x\n",pc);
					printf("red[%d] = ~(reg[%d]|reg[%d])\n",rd,rs,rt);
					printf("reg[%d] : 0x%x ----> ",rd, reg[rd]);
					reg[rd] = ~(reg[rs]|reg[rt]);
					printf("reg[%d] : 0x%x (%d)\n",rd,reg[rd],reg[rd]);
					numR++;
				}
				  else if(func == 0x25)//or
				 {
					 printf("<or>\n");
					 printf("pc : 0x%x\n",pc);
					 printf("reg[%d] = reg[%d]|reg[%d]\n",rd,rs,rt);
					 printf("reg[%d] : 0x%x ----> ",rd, reg[rd]);
					 reg[rd] = reg[rs]|reg[rt];
					 printf("reg[%d] : 0x%x (%d)\n",rd,reg[rd],reg[rd]);
					 numR++;
				 }
				else if(func == 0x2a||func == 0x2b)//slt,sltu
				{
					if(func==0x2a) printf("<slt>\n");
					else printf("<sltu>\n");
					printf("pc : 0x%x\n",pc);
					printf("reg[%d] = (reg[%d]<reg[%d])? 1:0\n",rd,rs,rt);
					printf("reg[%d] : 0x%x ----> ",rd, reg[rd]);
					reg[rd] = (reg[rs]<reg[rt])? 1:0;
					printf("reg[%d] : 0x%x (%d)\n",rd,reg[rd],reg[rd]);
					numR++;
				}
				else if(func == 0x00){//sll
					printf("<sll>\n");
					printf("pc : 0x%x\n",pc);
					printf("reg[%d] = reg[%d]<<shamt(0x%x)\n",rd,rt,shamt);
					printf("reg[%d] : 0x%x ----> ",rd, reg[rd]);
					reg[rd] = reg[rt]<<shamt;
					printf("reg[%d] : 0x%x (%d)\n",rd,reg[rd],reg[rd]);
					numR++;
				}
				   else if(func == 0x02){//srl
					   printf("<srl>\n");
					   printf("pc : 0x%x\n",pc);
					   printf("reg[%d] = reg[%d]>>shamt(0x%x)\n",rd,rt,shamt);
					   printf("reg[%d] : 0x%x ----> ",rd, reg[rd]);
					   reg[rd] = reg[rt]>>shamt;
					   printf("reg[%d] : 0x%x (%d)\n",rd,reg[rd],reg[rd]);
					   numR++;
				 }
				else if(func == 0x9)// jalr ----- optional
				{
					printf("<jalr>\n");
					printf("pc : 0x%x\n",pc);
					printf("reg[31] = pc+4 , pc = reg[%d]*4\n",rs);
					printf("reg[31] : 0x%x   pc : 0x%x ----> ", reg[31],pc);
					reg[31] = pc +4;
					pc = reg[rs] * 4;
					printf("reg[31] : 0x%x   pc : 0x%x\n", reg[31],pc);
					jumping_op = 1;
					numJ++;
				}
				printf("\n");
				break;
			case 0xa://slti
					printf("<slti>\n");
					printf("pc : 0x%x\n",pc); 
				    printf("reg[%d] = (reg[%d]<simm(0x%x))? 1:0\n",rt,rs,simm);
					printf("reg[%d] : 0x%x ----> ",rt, reg[rt]);
					reg[rt] = (reg[rs]<simm)?1:0;
					printf("reg[%d] : 0x%x (%d)\n",rt,reg[rt],reg[rt]);
		            printf("\n");
					numI++;
					break;
			case 0xd://ori
				{
					 printf("<ori>\n");
					 printf("pc : 0x%x\n",pc);   
					 printf("red[%d] = reg[%d]|simm(0x%x)\n",rt,rs,imm);
				     printf("reg[%d]: 0x%x ----> ",rt, reg[rt]);
					 reg[rt] = reg[rs]|imm;
					 printf("reg[%d] : 0x%x ,%d\n",rt,reg[rt],reg[rt]);

				}
				printf("\n");
				numI++;
				break;
			case 0xc://andi
				{
					 printf("<andi>\n");
					 printf("pc : 0x%x\n",pc);
					 printf("reg[%d] = reg[%d] & imm(0x%x)\n",rt, rs,imm);
					 printf("reg[%d] : 0x%x ----> ",rd, reg[rd]);
					 reg[rt] = reg[rs]&imm;//execute, write back
				     printf("reg[%d] : 0x%x (%d)\n", rt, reg[rt],reg[rt]);
				}
				printf("\n");
				numI++;
				break;
			case 0x2://jump
				printf("<j>\n");
				printf("pc : 0x%x\n",pc);
				printf("pc = 4*address(0x%x)\n",address);
				printf("pc : 0x%x ----> ",pc);
				pc = 4*address;
				printf("pc : 0x%x\n",pc);
				printf("\n");
				jumping_op = 1;
				numJ++;
				break;
			case 0x3://jal
				printf("<jal>\n");//put next instruction address to ra register, and jump
				printf("pc : 0x%x\n",pc);
				printf("reg[31](ra) = pc(0x%x) + 4, jump to pc = 4*address(0x%x)\n",pc,address);
				printf("reg[31] : 0x%x ----> ",reg[31]);
				reg[31] = pc+ 4;
				printf("reg[31] : 0x%x\n",reg[31]);
				printf("pc : 0x%x ----> ",pc);
				pc = address*4;
				printf("reg[31] : 0x%x\n",reg[31]);
				printf("pc : 0x%x\n",pc);
				printf("\n");
				jumping_op = 1;
				numJ++;
				break;
			case 0xf://lui
				printf("<lui>\n");
				printf("pc : 0x%x\n",pc);
				printf("reg[rt] = (imm<<16)\n");
				printf("reg[%d] : 0x%x ----> ",rt, reg[rt]);
				reg[rt] = imm << 16;
				printf("reg[%d] : 0x%x, %d\n",rt, reg[rt],reg[rt]);
				printf("\n");
				numI++;
				break;
			case 0x23://lw
				printf("<lw>\n");
				printf("pc : 0x%x\n",pc);
				printf("reg[%d] = Memory[reg[%d]+0x%x]\n",rt, rs,simm);
				printf("reg[%d] : 0x%x ----> ",rt, reg[rt]);
				reg[rt] = Memory[(reg[rs]+simm)/4];//memory access, write back
				printf("reg[%d] : 0x%x, %d\n", rt, reg[rt],reg[rt]);
				printf("\n");
				numI++;
				numM++;
				break;
			case 0x4://beq: branch on equal
				printf("<beq>\n");
				printf("pc : 0x%x\n",pc);
				printf("if (reg[%d] == reg[%d]) pc=pc+4+simm*4\n",rs,rt);
				printf("pc : 0x%x ----> ",pc);
				if(reg[rs]==reg[rt])
				{
					pc = pc + 4+ simm*4;
					jumping_op = 1;
				}
				printf("pc : 0x%x\n",pc);
				printf("\n");
				numI++;
				numB++;
				break;
			case 0x5://bne
				printf("<bne>\n");
				printf("pc : 0x%x\n",pc);
				printf("if (reg[%d] != reg[%d]) pc=pc+4+simm*4\n",rs,rt);
				printf("pc : 0x%x ----> ", pc);
				if(reg[rs]!=reg[rt])
				{
					pc = pc + 4+ simm*4;
					jumping_op = 1;
				}
				printf("pc : 0x%x\n",pc);
				printf("\n");
				numI++;
				numB++;
				break;
			default:
				printf("there is no that kind of opcode\n");
			}
		numofinst++;
		if(pc==0xFFFFFFFF)break;// if pc value is -1 exit.
		
		if(jumping_op == 0)pc = pc + 4;// update pc value
	}
	printf("******************result*********************\n");
	printf("reg[2] (final return)value: %d\n",reg[2]);
	printf("number of instructions: %d\n",numofinst);
	printf("number of R type instructions: %d\n",numR);
	printf("number of I type instructions: %d\n",numI);
	printf("number of J type instructions: %d\n",numJ);
	printf("number of memory access instructions: %d\n",numM);
	printf("number of taken branches instructions: %d\n",numB);
	printf("*********************************************\n");	
}

