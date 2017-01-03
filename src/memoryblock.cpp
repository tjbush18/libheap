#include "memoryblock.h"
#include "heap.h"

MemoryBlock::MemoryBlock()
{
	this->location = 0;
}

MemoryBlock::MemoryBlock(void* location)
{
	this->location = location;
}

bool MemoryBlock::exists()
{
	return (location != 0);
}

bool MemoryBlock::isFree()
{
	if(((uint32_t*) location)[0] == BLOCK_FREE)
	{
		return true;
	}
	return false;
}

uint32_t MemoryBlock::getSize()
{
	return ((uint32_t*) location)[1];
}

uint32_t MemoryBlock::getType()
{
	return ((uint32_t*) location)[0];
}

void* MemoryBlock::getLocation()
{
	return location;
}

MemoryBlock MemoryBlock::getLeftBlockIfFree()
{
	if(((uint32_t*) (location - 4))[0] == BLOCK_FREE)
	{
		return MemoryBlock(location - ((uint32_t*) (location - 8))[0]);
	}
	else
	{
		return MemoryBlock();
	}
}

MemoryBlock MemoryBlock::getRightBlockIfFree()
{
	if(((uint32_t*) (location + ((uint32_t*) location)[1]))[0] == BLOCK_FREE)
	{
		return MemoryBlock(location + ((uint32_t*) location)[1]);
	}
	else
	{
		return MemoryBlock();
	}
}

MemoryBlock MemoryBlock::getNext()
{
	if(getType() == BLOCK_FREE && exists())
	{
		return MemoryBlock(((uint32_t*) location)[2]);
	}
	else
	{
		return MemoryBlock();
	}
}

MemoryBlock MemoryBlock::getLast()
{
	if(getType() == BLOCK_FREE && exists())
	{
		return MemoryBlock(((uint32_t*) location)[3]);
	}
	else
	{
		return MemoryBlock();
	}
}

void MemoryBlock::writeTags(uint32_t tag)
{
	((uint32_t*) location)[0] = tag;
	((uint32_t*) (location + ((uint32_t*) location)[1] - 4))[0] = tag;
}

void MemoryBlock::resize(uint32_t size)
{
	((uint32_t*) location)[1] = size;
	writeTags(getType());
	if(getType() == BLOCK_FREE)
	{
		((uint32_t*) (location + ((uint32_t*) location)[1] - 8))[0] = getSize();
	}
}

void MemoryBlock::linkNext(MemoryBlock next)
{
	if(getType() == BLOCK_FREE && exists())
	{
		((uint32_t*) location)[2] = next.getLocation();
	}
}

void MemoryBlock::linkLast(MemoryBlock last)
{
	if(getType() == BLOCK_FREE && exists())
	{
		((uint32_t*) location)[2] = last.getLocation();
	}
}
