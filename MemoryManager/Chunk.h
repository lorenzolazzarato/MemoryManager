#pragma once
#include <cstddef>

struct Chunk
{
	void Init(std::size_t blockSize, unsigned char blocks);
	void* Allocate(std::size_t blockSize);
	void Deallocate(void* p, std::size_t blockSize);

	unsigned char* pData_; // represents the bytes composing the various blocks in the chunk
	unsigned char
		firstAvailableBlock_,
		blocksAvailable_;
};
