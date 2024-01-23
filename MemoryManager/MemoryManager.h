#pragma once
#include <cstdlib>
#include "SmallObjectAllocator.h"

#define CHUNKS_SIZE 2048 // how big should each chunk be in bytes?
#define MAX_OBJ_SIZE 64

class MemoryManager
{
public:
	static void* Allocate(std::size_t size);
	static void Deallocate(void* p, std::size_t size);
private:
	static SmallObjectAllocator soa;
};

