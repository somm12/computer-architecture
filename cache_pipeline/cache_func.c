#include "pipeline.h"

int Readcache(int address)
{
	//pc value input or address input
	int tag = ((address/4) & 0xFFFFFFC0) >> 6;//26bit
	int offset = ((address/4) & 0x0000003F);//64byte(6bit)
	int where = ((address/4) & 0xFFFFFFC0);
	int miss = 1;
	int result;
	int des;

	for(int i = 0; i < 4; i++){

		if(cache[i].valid == 1 && cache[i].tag == tag)//if there the address in the cache
		{//cache hit
			miss = 0;
			cache[i].oldest = oldest;
			oldest++;
			cache[i].SCA = 1;//when cache hit, value is
			cache_hit++;
			return cache[i].data[offset];
		}
	}
	if(miss == 1){
		int sig = 0;
		for(int i = 0; i < 4; i++){// but if cache is full(capacity miss) --> make victim
			if(cache[i].valid == 0){
				sig = 1;//if cache is not full
			}

		}
		if(sig == 0){//if cache is full-capacity miss
			des = Writecache(address,Memory[address/4]);
			cache_miss++;
			return cache[des].data[offset];
		}
		//coldmiss
		else{
			cache_miss++;
			cache[index_cache].valid = 1;
			cache[index_cache].tag = tag;
			cache[index_cache].oldest = oldest;
			cache[index_cache].dirty = 0;
			cache[index_cache].SCA = 0;
			for(int i = 0; i < 64; i ++){

				cache[index_cache].data[i] = Memory[where+i];
			}
			result =  cache[index_cache].data[offset];
			index_cache++;
			oldest++;
			return result;
		}
	}
}

int Writecache(int address,int value){//use write back method

	int tag = ((address/4) & 0xFFFFFFC0) >> 6;
	int offset = ((address/4) & 0x0000003F);
	int where = ((address/4) & 0xFFFFFFC0);
	int miss = 1;
	int hit = 0;
	int locate;
	int victim;
	int vic;
	int coldmiss = 0;
	int least;

	for(int i = 0; i < 4; i++)
	{
		if(cache[i].valid == 1 && cache[i].tag == tag)
		{	
			locate = i;
			hit = 1;
			miss = 0;
		}
	}
	if(hit == 1)//when cache hit
	{
		cache[locate].data[offset] = value;// write the value
		cache[locate].dirty = 1;
		cache[locate].oldest = oldest;
		oldest++;
		cache[locate].SCA = 1;
		cache_hit++;
	}
	else{//when cache miss
		for(int k = 0; k < 4; k++){//cold miss
			if(cache[k].valid ==0)coldmiss = 1;}

		if(coldmiss ==1){
			for(int k = 0; k < 64; k ++){

				cache[index_cache].data[offset] = Memory[where+k];
			}
			cache[index_cache].valid = 1;
			cache[index_cache].tag = tag;
			cache[index_cache].oldest = oldest;
			cache[index_cache].data[offset] = value;//write the value
			cache[index_cache].dirty = 1;
			cache[index_cache].SCA = 0;
			index_cache++;
			oldest++;
			cache_miss++;
		}
		else{//capacity miss
			//select victim
			cache_miss++;
			least = -1;
			int old = max_oldest();
			victim = max_oldest();//give the oldest one to victim
			
			while(1){
				int i;
				for(i = 0; i < 4 ; i++){
					if((victim > cache[i].oldest) && (cache[i].oldest > least))
					{
						victim = cache[i].oldest;
						vic = i;
					}
				}
				if(cache[vic].SCA == 1){//if sca ==1, have to choose another victim(the next oldest victim)
					cache[vic].SCA = 0;
					least = victim;
					victim = old;
					continue;
				}
				else break;
			}//while
			check = 1;
			if(cache[vic].dirty == 1)//have to write memory
			{
				updateMem(where,vic);
			}
			write_data(vic,value,tag,offset,where);//write the value
			printf("vic: %d\n",vic);
			return vic;
		}

	}
}

	void write_data(int vic,int value, int tag, int offset,int where){
		cache[vic].valid = 1;
		cache[vic].SCA = 0;
		cache[vic].oldest = oldest;
		cache[vic].dirty = 1;
		cache[vic].tag = tag;
		oldest++;
		for(int k = 0; k < 64; k++)
		{
			cache[vic].data[k] = Memory[where+k];
		}

		cache[vic].data[offset] = value;

	}
	void updateMem(int where,int vic)
	{
		//sholud give the replacement place in memory 
		for(int k = 0; k < 64; k++)
		{
			Memory[(cache[vic].tag << 6)+k] = cache[vic].data[k];
		}
	}

int max_oldest(){

	int max_old;
	max_old = cache[0].oldest;
	for(int k = 0; k < 4 ; k++){
		if(max_old < cache[k].oldest){
		
			max_old = cache[k].oldest;
		}
	}
	return max_old;

}
