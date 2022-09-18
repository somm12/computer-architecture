#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
int check;
int index_cache;//this is for where to store tag in cache( but fully associative cache)
int oldest;//this is for oldest value(oldest rank) if the value is small, that is the oldest one, work with sca bit when select victim

int Memory[0x400000];//Memory
int pc;
int Reg[32];
int cycle;
int ForwardA;//for data forwarding in rs register
int ForwardB;//for data forwarding in rt register
int miss;//check for acurrcy branch prediction.
int already_pc4;// this is stop for twice branch in fetch, execute stage
int already_branch;
int PHT[0x100000/4];//2bit counter
int numofinst;
int numM;
int numR;
int numB;
int numNB;
int numJ;
int cache_hit;
int cache_miss;

typedef struct IFIDLATCH
{
	int pc4;
	int inst;
	int branchPresig;//the signal that predict branch(prepare the miss prediction)
	int cycle;//to see cycle's every stage 
} IFID_latch;


typedef struct Controlsignal
{
	int RegDst;
	int Branch;
	int Jump;
	int MemtoReg;
	int MemRead;
	int MemWrite;
	int ALUOp;
	int ALUSrc;
	int RegWrite;
}Contsig;

typedef struct IDEXLATCH
{
	int pc4;
	int Inst;//for lw dependency
	int branchPresig;
	int v1;
	int v2;
	int WriteReg;
	int opcode;
	int rs;
	int rt;
	int rd;
	int simm;
	int ZeroExtImm;
	int shamt;
	int func;
	int cycle;
	Contsig control;
}IDEX_latch;
typedef struct EXMEMLATCH
{
	int branchPresig;
	int v3;
	int v2;
	int WriteReg;
	int pc4;
	int rs;
	int rt;
	int rd;
	int cycle;
	int opcode;
	Contsig control;
} EXMEM_latch;

typedef struct MEMWBLATCH
{
	int pc4;
	int v5;
	int WriteReg;
	int rs;
	int rt;
	int rd;
	int opcode;
	int cycle;
	Contsig control;
}MEMWB_latch;

typedef struct Branch_t_b
{
	int pc;
	int target_address;

}B_T_B;

typedef struct cache
{
	int tag;
	int valid;
	int data[64];//line size:64byte
	int dirty;
	int SCA;
	int oldest;
}CACHE;

CACHE cache[4];//data store size:256B, line store size:64B -> # of entries: 4.

B_T_B BTB[0x100000/4];
IFID_latch ifid_latch[2];
IDEX_latch idex_latch[2];
EXMEM_latch exmem_latch[2];
MEMWB_latch memwb_latch[2];

Contsig signal;

void updatelatch();
void setting();
void initializeMemory();

//cache
int Readcache(int address);
int Writecache(int address,int value);
void write_data(int vic,int value, int tag, int offset,int where);
void updateMem(int where,int vic);
int max_oldest();

int fetch();
void controlsignal(int opcode, int func);
void decode(IFID_latch ifid_input);
void execute(IDEX_latch idex_input);
void memoryaccess(EXMEM_latch exmem_input);
void writeback(MEMWB_latch memwb_input);

void ExHazard(int inst);
void LW_Hazard(int inst);
void MemHazard();//in load word inst
void stopMemHazard();

void jump_instruction(int target);
void flush1(int simm);//branch miss1- actually branch
void flush2();//branch miss2- actually no branch

void branch1(int simm);//branch prediction correct- do branch
void branch2();//branch prediction correct- do pc+4
void beq_con(int simm, int Presig);
void bne_con(int simm, int Presig);

int one_LV_branch_predic();
void mux(int num);
void PHT_update(int branch);
void fill_BTB(int index,int simm);
int BTB_search(int pc_value);
void WBHazard(int wb_Regwrite, int wb_rd, int wb_rt, int wb_opcode);

void printout();

