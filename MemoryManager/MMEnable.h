#pragma once
#include "MemoryManager.h"

#define MM_ENABLED 1

#ifdef MM_ENABLED
void* operator new(size_t size) {
	return MemoryManager::Allocate(size);
}

void operator delete(void* p, size_t size) {
	return MemoryManager::Deallocate(p, size);
}
#endif

