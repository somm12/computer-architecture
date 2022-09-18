#include "pipeline.h"
#include <unistd.h>
int fetch() {

	ifid_latch[0].branchPresig = one_LV_branch_predic();
	ifid_latch[0].inst = Readcache(pc);
	ifid_latch[0].pc4 = pc + 4;
	ifid_latch[0].cycle = cycle;
	printf("■ ■ ■ ■ ■ ■ ■ ■ ■ cycle%d ■ ■ ■ ■ ■ ■ ■ ■ ■ ■ \n",cycle);
	printf("pc: 0x%x\n",pc);
	printf("[Fetch] 0x%08x\n", ifid_latch[0].inst);
	pc = pc + 4;
	mux(ifid_latch[0].branchPresig);
	cycle++;
}
void ExHazard(int inst){
	//when Rtype instruction have data dependency
	if(exmem_latch[0].opcode == 0x00){
		if(exmem_latch[0].control.RegWrite && (idex_latch[0].rs == exmem_latch[0].rd)){
			ForwardA = 0b10;//when forwarding ALU result, forwarding signal
		}
		else if(exmem_latch[0].control.RegWrite && (idex_latch[0].rt == exmem_latch[0].rd))
			ForwardB = 0b10;//when forwarding ALU result, forwarding signal
	}
	//when I type instruction have data dependency
	if(exmem_latch[0].opcode != 0x00){
		if(exmem_latch[0].control.RegWrite && (idex_latch[0].rs == exmem_latch[0].rt)){
			if(exmem_latch[0].opcode == 0x23)LW_Hazard(inst);
			else ForwardA = 0b10;//when forwarding ALU result, forwarding signal
		}
		else if(exmem_latch[0].control.RegWrite && (idex_latch[0].rt == exmem_latch[0].rt))//if I type inst in exe stage, Rtype inst in decode stage.
		{	if(exmem_latch[0].opcode == 0x23)LW_Hazard(inst);
			else ForwardB = 0b10;//when forwarding ALU result, forwarding signal
		}
	}
}
void LW_Hazard(int inst){
	//for add nop instruction!!
	memset(&(idex_latch[0].control),0,sizeof(Contsig));
	printf("*****simple file single cycle*****\n");
	ifid_latch[0].inst = inst;
	ifid_latch[0].pc4 = pc;
	pc = pc - 4;// 중간에 nop 명령어를 넣기위해 그전단계를 유지시키기 위함.
}
void MemHazard(){
	//when R type instruction have data dependency
	if(memwb_latch[0].opcode == 0x00)
	{
		if(memwb_latch[0].control.RegWrite && (idex_latch[0].rs == memwb_latch[0].rd))
			ForwardA = 0b01;// forwarding result in memory stage

		else if(memwb_latch[0].control.RegWrite && (idex_latch[0].rt == memwb_latch[0].rd))
			ForwardB = 0b01;
		stopMemHazard(1);
	}
	//when I type instruction have data dependency
	if(memwb_latch[0].opcode !=0x00)
	{if(memwb_latch[0].control.RegWrite && (idex_latch[0].rs == memwb_latch[0].rt))//)&&
		ForwardA = 0b01;

	else if(memwb_latch[0].control.RegWrite && (idex_latch[0].rt == memwb_latch[0].rt))//&&
		ForwardB = 0b01;
	stopMemHazard(2);

	}
}
void stopMemHazard(int Case){
			if(exmem_latch[0].opcode == 0x00 &&(exmem_latch[0].rd == idex_latch[0].rt || exmem_latch[0].rd == idex_latch[0].rs))
				if(((memwb_latch[0].opcode != 0) &&(memwb_latch[0].rt == exmem_latch[0].rd))|| ((memwb_latch[0].opcode == 0) &&(memwb_latch[0].rd == exmem_latch[0].rd)))
			{	ForwardA = 0;
				ForwardB = 0;
			}

		
else if(exmem_latch[0].opcode != 0x00 &&(exmem_latch[0].rt == idex_latch[0].rt || exmem_latch[0].rt == idex_latch[0].rs))
	if(((memwb_latch[0].opcode != 0) && (memwb_latch[0].rt == exmem_latch[0].rt))|| ((memwb_latch[0].opcode == 0) &&(memwb_latch[0].rd == exmem_latch[0].rt)))
		if(exmem_latch[0].control.RegWrite == 1)
		{	ForwardA = 0;
			ForwardB = 0;
		}

}
void controlsignal(int opcode, int func) {

	if (opcode == 0x0) idex_latch[0].control.RegDst = 1;
	else idex_latch[0].control.RegDst = 0;

	if (opcode == 0x2 || opcode == 0x3)//jump type
		idex_latch[0].control.Jump = 1;
	else idex_latch[0].control.Jump = 0;

	if (opcode == 0x4 || opcode == 0x5)idex_latch[0].control.Branch = 1;//beq,bne
	else idex_latch[0].control.Branch = 0;

	if (opcode == 0x23)idex_latch[0].control.MemRead = 1;//load word
	else idex_latch[0].control.MemRead = 0;

	if (opcode == 0x2b)idex_latch[0].control.MemWrite = 1;//sw
	else idex_latch[0].control.MemWrite = 0;

	if (opcode == 0x23)idex_latch[0].control.MemtoReg = 1;//load word
	else idex_latch[0].control.MemtoReg = 0;

	if (opcode == 0x8 || opcode == 0x9 || opcode == 0xc || opcode == 0xf || opcode == 0x23 || opcode == 0xd || opcode == 0xa || opcode == 0x2b)
		idex_latch[0].control.ALUSrc = 1;
	else idex_latch[0].control.ALUSrc = 0;

	if (opcode == 0x4 || opcode == 0x5 || opcode == 0x2 || opcode == 0x3 || opcode == 0x2b)idex_latch[0].control.RegWrite = 0;
	else idex_latch[0].control.RegWrite = 1;
	if (opcode == 0x0 && func == 0x08)idex_latch[0].control.RegWrite = 0;//jr일때

	idex_latch[0].control.ALUOp = opcode;

	if(idex_latch[0].control.RegWrite == 1)numR++;
	if(idex_latch[0].control.MemRead == 1 || idex_latch[0].control.MemWrite == 1)numM++;
	if(idex_latch[0].control.Jump == 1)numJ++;
}

void decode(IFID_latch ifid_input) {


	int opcode = (ifid_input.inst & 0xfc000000) >> 26;
	int rs = (ifid_input.inst & 0x03e00000) >> 21;
	int rt = (ifid_input.inst & 0x001f0000) >> 16;
	int rd = (ifid_input.inst & 0x0000f800) >> 11;
	int func = (ifid_input.inst & 0x0000003f) >> 0;
	int shamt = (ifid_input.inst & 0x000007c0) >> 6;
	int imm = (ifid_input.inst & 0x0000ffff) >> 0;
	int simm = (imm >> 15) ? (0xffff0000 | imm) : imm;
	int address = (ifid_input.inst & 0x03ffffff) >> 0;

	printf("-----------------Decode-----------------------\n");
	printf("pc: 0x%x\n",ifid_input.pc4-4);
	printf("opcode: 0x%x rs: %d rt: %d rd: %d func: 0x%x\n",opcode,rs,rt,rd,func);
	printf("Reg[%d]: 0x%x,Reg[%d]: 0x%x,Reg[%d]: 0x%x\n",rs, Reg[rs], rt,Reg[rt],rd,Reg[rd]);
	printf("simm: %d\n",simm);

	controlsignal(opcode, func);
	if((opcode == 0x4||opcode == 0x5)&&!BTB_search(ifid_input.pc4-4))fill_BTB(ifid_input.pc4-4,simm);// for branch, fill BTB

	idex_latch[0].v1 = Reg[rs];
	idex_latch[0].v2 = Reg[rt];

	if (opcode == 0xc || opcode == 0xd || opcode == 0xf)idex_latch[0].ZeroExtImm = imm;
	else idex_latch[0].simm = simm;

	idex_latch[0].pc4 = ifid_input.pc4;

	if (idex_latch[0].control.RegDst == 1) idex_latch[0].WriteReg = rd;
	else idex_latch[0].WriteReg = rt;

	if(idex_latch[0].control.Jump == 1){
		if(opcode==0x3) Reg[31] = ifid_input.pc4;//when do jump and link
		jump_instruction(address);//when jump
	}
	idex_latch[0].shamt = shamt;
	idex_latch[0].func = func;
	idex_latch[0].rs = rs;
	idex_latch[0].rt = rt;
	idex_latch[0].rd = rd;
	idex_latch[0].cycle = ifid_input.cycle;
	idex_latch[0].opcode = opcode;
	idex_latch[0].Inst = ifid_input.inst;//for lw hazard
	idex_latch[0].branchPresig = ifid_input.branchPresig;

	if(ifid_input.inst == 0x00000000)memset(&idex_latch[0].control,0,sizeof(Contsig));//this is for terminate cycle
}
void jump_instruction(int target){

	pc = ((pc&0xf0000000)|(target*4));
}
void execute(IDEX_latch idex_input) {

	int v4;
	int simm;//for calculate branch target address
	int condition_bne = 0;
	int condition_beq = 0;
	simm = idex_input.simm;
	exmem_latch[0].pc4 = idex_input.pc4;//for branch in branch.c - branch()

	if (idex_input.control.ALUSrc == 1) v4 = idex_input.simm;
	else v4 = idex_input.v2;//rt

	switch (idex_input.control.ALUOp) {// elements that can be changed in this program: pc, reg, mem value..
		case 0x9://addiu
			exmem_latch[0].v3 = idex_input.v1 + v4;//execute, write back
			break;
		case 0x8://addi
			exmem_latch[0].v3 = idex_input.v1 + v4;//execute, write back
			break;
		case 0x2b://sw
			exmem_latch[0].v3 = idex_input.v1 + v4;
			break;
		case 0://addu, add , sub, subu, jr , and ,nor, or ,slt, sltu, sll, srl
			if (idex_input.func == 0x21 || idex_input.func == 0x20)
			{
				exmem_latch[0].v3 = idex_input.v1 + v4;//execute, write back
			}
			else if (idex_input.func == 0x22 || idex_input.func == 0x23)//sub, subu
			{
				exmem_latch[0].v3 = idex_input.v1 - v4;;//execute, write back
			}
			else if (idex_input.func == 0x24) {//and

				exmem_latch[0].v3 = idex_input.v1 & v4;//execute, write back

			}
			else if (idex_input.func == 0x27)//nor
			{
				exmem_latch[0].v3 = ~(idex_input.v1 | v4);

			}
			else if (idex_input.func == 0x25)//or
			{

				exmem_latch[0].v3 = idex_input.v1 | v4;

			}
			else if (idex_input.func == 0x2a || idex_input.func == 0x2b)//slt,sltu
			{
				exmem_latch[0].v3 = (idex_input.v1 < v4) ? 1 : 0;

			}
			else if (idex_input.func == 0x00) {//sll

				exmem_latch[0].v3 = v4 << idex_input.shamt;
			}
			else if (idex_input.func == 0x02) {//srl

				exmem_latch[0].v3 = v4 << idex_input.shamt;

			}
			else if(idex_input.func == 0x08) pc = Reg[idex_input.rs];//jr
			break;
		case 0xa://slti

			exmem_latch[0].v3 = (idex_input.v1 < v4) ? 1 : 0;
			break;
		case 0xd://ori
			{
				v4 = idex_input.ZeroExtImm;
				exmem_latch[0].v3 = idex_input.v1 | v4;
			}
			break;
		case 0xc://andi
			{
				v4 = idex_input.ZeroExtImm;
				exmem_latch[0].v3 = idex_input.v1 & v4;
			}
			break;
		case 0xf://lui
			{
				v4 = idex_input.ZeroExtImm;
				exmem_latch[0].v3 = v4 << 16;
			}
			break;
		case 0x23://lw
			exmem_latch[0].v3 = idex_input.v1 + v4;
			break;
		case 0x4://beq
			{
				exmem_latch[0].v3 = idex_input.v1-v4;
				condition_beq = 1;
			}
			break;
		case 0x5://bne
			{
				exmem_latch[0].v3 = idex_input.v1-v4;
				condition_bne = 1;
			}
			break;
		default:
			printf("there is no that kind of opcode\n");
	}
	exmem_latch[0].rs = idex_input.rs;// check for data forwarding in execution hazard.
	exmem_latch[0].rt = idex_input.rt;// check for data forwarding in execution hazard.
	exmem_latch[0].rd = idex_input.rd;
	exmem_latch[0].v2 = idex_input.v2;
	exmem_latch[0].WriteReg = idex_input.WriteReg;
	exmem_latch[0].control = idex_input.control;
	exmem_latch[0].cycle = idex_input.cycle;
	exmem_latch[0].opcode = idex_input.opcode;

	ForwardA = 0b00;
	ForwardB = 0b00;
	ExHazard(idex_input.Inst);

	if(ForwardA == 0b10)//rs register..
		idex_latch[0].v1 = exmem_latch[0].v3;
	else if(ForwardB == 0b10)
		idex_latch[0].v2 = exmem_latch[0].v3;

	printf("-----------------Execute----------------------\n");
	printf("pc: 0x%x\n",idex_input.pc4-4);
	printf("v1: 0x%x, v2: 0x%x, simm: 0x%x\n", idex_input.v1, idex_input.v2,idex_input.simm);
	printf("v4: 0x%x\n",v4);
	printf("v3: 0x%x\n",exmem_latch[0].v3);
	if(condition_beq==1)beq_con(simm,idex_input.branchPresig);//for branch - initialize all of exmemlatch to 0, so this line is at the end.
	else if(condition_bne==1)bne_con(simm,idex_input.branchPresig);//for branch
}

void memoryaccess(EXMEM_latch exmem_input) {

	if (exmem_input.control.MemtoReg == 1 && exmem_input.control.MemRead == 1)//lw
	{
		memwb_latch[0].v5 = Readcache(exmem_input.v3);
	}
	else if (exmem_input.control.MemWrite == 1 && exmem_input.control.MemRead == 0)//sw
	{
		Writecache(exmem_input.v3,exmem_input.v2);
	}
	else memwb_latch[0].v5 = exmem_input.v3;//ALU result

	memwb_latch[0].pc4 = exmem_input.pc4;
	memwb_latch[0].rd = exmem_input.rd;
	memwb_latch[0].rs = exmem_input.rs;// check for data forwarding in memory hazard.
	memwb_latch[0].rt = exmem_input.rt;// check for data forwarding in memory hazard.
	memwb_latch[0].WriteReg = exmem_input.WriteReg;

	memwb_latch[0].control = exmem_input.control;
	memwb_latch[0].cycle = exmem_input.cycle;
	memwb_latch[0].opcode = exmem_input.opcode;

	MemHazard();
	if(ForwardA == 0b01)//rs register
		idex_latch[0].v1 = memwb_latch[0].v5;
	else if(ForwardB == 0b01)//rt register
	{
		idex_latch[0].v2 = memwb_latch[0].v5;
	}
	ForwardA = 0b00;
	ForwardB = 0b00;
	printf("----------------memoryaccess------------------\n");
	printf("pc: 0x%x\n",exmem_input.pc4-4);
	printf("opcode: 0x%x\n",exmem_input.opcode);
	printf("memory address or ALU result: 0x%x\n",exmem_input.v3/4);
	printf("write data in memory :rt -> 0x%x\n",exmem_input.v2);
	printf("value to write in Reg memory(ALU result): 0x%x\n",memwb_latch[0].v5);
}

void writeback(MEMWB_latch memwb_input) {
	int original;
	original = Reg[memwb_input.WriteReg];
//	WBHazard(memwb_input.control.RegWrite, memwb_input.rd,memwb_input.rt,memwb_input.opcode);//
	if(ForwardA == 0b10)//rs register..
		idex_latch[0].v1 = memwb_input.v5;
	else if(ForwardB == 0b10)//rt register
		idex_latch[0].v2 = memwb_input.v5;

	printf("-----------------writeback--------------------\n");
	printf("pc: 0x%x\n",memwb_input.pc4-4);
	if (memwb_input.control.RegWrite == 1)
	{	Reg[memwb_input.WriteReg] = memwb_input.v5;
		printf("Reg[%d]: 0x%x ----> 0x%x\n",memwb_input.WriteReg,original,Reg[memwb_input.WriteReg]);
	}
	else printf("no write back\n");
}

