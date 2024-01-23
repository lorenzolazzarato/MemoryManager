#pragma once
#include <vector>
#include "Chunk.h"

class FixedAllocator
{
public:
	FixedAllocator(std::size_t blockSize);
	void* Allocate();
	void Deallocate(void* p);
	std::size_t GetBlockSize() const;
private:
	std::size_t blockSize_;
	unsigned char numBlocks_;
	typedef std::vector<Chunk> Chunks;
	Chunks chunks_;
	Chunk* allocChunk_;
	Chunk* deallocChunk_;
};
