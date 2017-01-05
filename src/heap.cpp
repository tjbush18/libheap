#include "heap.h"

Heap::Heap()
{
	
}

Heap::Heap(void* location, uint32_t size)
{
	this->location = location;
	this->availableList = location + 4;
	this->size = size;
	
	uint32_t* heapStart = (uint32_t*) availableList;
	heapStart[0] = BLOCK_FREE;
	heapStart[1] = size - 8;
	heapStart[2] = 0;
	heapStart[3] = 0;
	
	uint32_t* heapEnd = (uint32_t*) (location + size - 4) - 8;
	heapEnd[0] = size - 8;
	heapEnd[1] = BLOCK_FREE;
	
	((uint32_t*) location)[0] = BLOCK_RESERVED;
	((uint32_t*) (location + size - 4))[0] = BLOCK_RESERVED;
}

void* Heap::allocate(uint32_t size)
{
	uint32_t requiredSize = size + 12;
	MemoryBlock iterator = MemoryBlock(availableList);
	MemoryBlock block;
	while(iterator.exists())
	{
		if(iterator.getSize() >= requiredSize)
		{
			if(iterator.getSize() - requiredSize < 24)
			{
				block = iterator;
				block.getLast().linkNext(block.getNext());
				block.writeTags(BLOCK_RESERVED);
			}
			else
			{
				block = MemoryBlock(iterator.getLocation() + iterator.getSize() - requiredSize);
				block.resize(requiredSize);
				iterator.resize(iterator.getSize() - requiredSize);
			}
			return block.getLocation() + 8;
		}
		iterator = iterator.getNext();
	}
	return block.getLocation() + 8;
}

void Heap::free(void* block)
{
	MemoryBlock freedBlock(block - 8);
	MemoryBlock leftBlock = freedBlock.getLeftBlockIfFree();
	MemoryBlock rightBlock = freedBlock.getRightBlockIfFree();
	if(leftBlock.exists())
	{
		leftBlock.resize(leftBlock.getSize() + freedBlock.getSize());
		if(rightBlock.exists())
		{
			leftBlock.linkNext(rightBlock.getNext());
			rightBlock.getNext().linkLast(leftBlock);
			leftBlock.resize(leftBlock.getSize() + rightBlock.getSize());
		}
	}
	else if(rightBlock.exists())
	{
		freedBlock.writeTags(BLOCK_FREE);
		rightBlock.getLast().linkNext(freedBlock);
		rightBlock.getNext().linkLast(freedBlock);
		freedBlock.linkLast(rightBlock.getLast());
		freedBlock.linkNext(rightBlock.getNext());
		freedBlock.resize(freedBlock.getSize() + rightBlock.getSize());
	}
	else
	{
		MemoryBlock avail = MemoryBlock(availableList);
		freedBlock.writeTags(BLOCK_FREE);
		freedBlock.linkLast(avail);
		freedBlock.linkNext(avail.getNext());
		avail.getNext().linkLast(freedBlock);
		avail.linkNext(freedBlock);
		avail.linkLast(freedBlock);
	}
}

uint32_t Heap::getFreeSpace()
{
	uint32_t totalSize = 0;
	for(MemoryBlock iterator(availableList); iterator.exists(); iterator = iterator.getNext())
	{
		totalSize += iterator.getSize();
	}
	return totalSize;
}
