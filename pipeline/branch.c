#include "pipeline.h"

void fill_BTB(int index ,int simm){//set pc to index of BTB
	BTB[index/4].pc = index;//store branch inst in BTB. at the decode stage, the pc value already updated.so pc-4
	BTB[index/4].target_address = index + 4 + simm*4;
}
void PHT_update(int branch){
	//fetch에서 브렌치를 X 신호를 보냄,실행 단계에서 만약 진짜 안한다고 나오면 그대로 두고
	//아니면 +1
	int location;
	location = exmem_latch[0].pc4-4;
	if(branch==1){// when branch!
		if(PHT[location/4]==0b11)PHT[location/4]=0b11;//when state is 0b11(highest state)keep it.
		else
			PHT[location/4] = PHT[location/4] + 1;// else - > +1
	}
	else if(branch==0)//when not branch!i
	{
		if(PHT[location/4]==0b00) PHT[location/4]=0b00;//when state is ob00(lowest state)keep it.
		else 
			PHT[location/4] = PHT[location/4] - 1;
	}
}




int one_LV_branch_predic(){//need in fetch

	int hit;
	hit = BTB_search(pc);

	if(hit && (PHT[pc/4] > 1))// 11 10 01 00
		return 1;
	else return 0;

}
void mux(int num){

	if(num==1)
	{
		pc = BTB[(pc-4)/4].target_address;
		already_branch = 1;
	}
	else {
		pc = pc;//already update pc + 4 in previous cycle.
		already_pc4 = 1;
	}
}

int BTB_search(int pc_value){

	if(BTB[pc_value/4].pc == pc)//if the pc is in BTB.
		return 1;
	else return 0;
}

void beq_con(int simm, int Presig){
	numB++;
	if(exmem_latch[0].v3 == 0)
	{
		PHT_update(1);//1 means do the branch inst
		if(Presig == 0){// when did not predict branch inst, but actually branch.
			flush1(simm);
		}
		else 
			if(already_branch !=1)branch1(simm);
		already_branch = 0;
	}
	else{
		numNB++;
		PHT_update(0);//0 means do not the branch inst
		if(Presig==1)// when  predicted branch, but not actually.
		{
			flush2();
		}
		else 
			if(already_pc4 != 1)branch2();
		already_pc4 = 0;
	}

}

void bne_con(int simm, int Presig){
	numB++;
	if(exmem_latch[0].v3 != 0){
		PHT_update(1);
		if(Presig==0){//prediction signal ==0 -> predicted no branch
			flush1(simm);//case: branch, but predicted no branch
		}
		else 
			if(already_branch !=1)branch1(simm);
		already_branch = 0;
	}
	else {
		numNB++;
		PHT_update(0);
		if(Presig==1){
			flush2();//case: no branch, but predicted branch
		}
		else 
			if(already_pc4 != 1)branch2();
		already_pc4 = 0;
	}

}
void flush1(int simm){//branch when did not expect.

	pc = exmem_latch[0].pc4 + simm*4;
	memset(&(ifid_latch[0]),0,sizeof(IFID_latch));
	memset(&(idex_latch[0]),0,sizeof(IDEX_latch));
	memset(&(exmem_latch[0]),0,sizeof(EXMEM_latch));
}
void flush2(){//no branch but expected branch

	pc = exmem_latch[0].pc4;
	memset(&(ifid_latch[0]),0,sizeof(IFID_latch));
	memset(&(idex_latch[0]),0,sizeof(IDEX_latch));
	memset(&(exmem_latch[0]),0,sizeof(EXMEM_latch));
}

void branch1(int simm){
	pc = exmem_latch[0].pc4 + simm*4;//when do the branch inst & prediction correction

}
void branch2(){
	pc = exmem_latch[0].pc4;//when do the general inst(pc+4) & prediction correction
}

