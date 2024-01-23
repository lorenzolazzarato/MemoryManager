#include "MemoryManager.h"

SmallObjectAllocator MemoryManager::soa = SmallObjectAllocator(CHUNKS_SIZE, MAX_OBJ_SIZE);

void* MemoryManager::Allocate(std::size_t size)
{
    return soa.Allocate(size);
}

void MemoryManager::Deallocate(void* p, std::size_t size)
{
    soa.Deallocate(p, size);
}
