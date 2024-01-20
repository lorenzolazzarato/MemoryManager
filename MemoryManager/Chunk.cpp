#include "Chunk.h"

void Chunk::Init(std::size_t blockSize, unsigned char blocks)
{
	pData_ = new unsigned char[blockSize * blocks];
	firstAvailableBlock_ = 0;
	blocksAvailable_ = blocks;
	unsigned char i = 0;
	unsigned char* p = pData_;
	for (; i != blocks; p += blockSize)
	{
		*p = ++i;
	}
}

void* Chunk::Allocate(std::size_t blockSize)
{
	// if no block is available return 0
	if (!blocksAvailable_)
		return 0;
	unsigned char* pResult = pData_ + (firstAvailableBlock_ * blockSize);

	// Update firstAvailableBlock_ to point to the next block
	firstAvailableBlock_ = *pResult;
	--blocksAvailable_;
	return pResult;
}

void Chunk::Deallocate(void* p, std::size_t blockSize)
{
	// if there is at least one free block
	*(unsigned char*)p = firstAvailableBlock_;
	firstAvailableBlock_ = (p - pData_) / blockSize;
	++blocksAvailable_;
}