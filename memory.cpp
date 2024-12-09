#include <vector>
#include "helpers.h"

typedef struct {
	byte core[2048];
	byte key = 0;
} block;

class memory{
	public:
		memory(int blknum){
			for (int i=0;i<blknum;i++){
				block nblk;
				blocks.push_back(nblk);
			}
		}

	private:
		std::vector<block> blocks;
		//Returns the block of a physical memory address
		inline int getBlkAddr(word memaddr){
			return memaddr / (1 << 11);
		}
		//Returns the position within the block of the physical memory address
		inline int getBlkPos(word memaddr){
			return memaddr % (1 << 11);
		}
};

