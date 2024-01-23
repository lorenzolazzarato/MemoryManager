#include "SmallObjectAllocator.h"

SmallObjectAllocator::SmallObjectAllocator(std::size_t chunkSize, std::size_t maxObjectSize)
{
	chunkSize_ = chunkSize;
	maxObjectSize_ = maxObjectSize;
	pLastAlloc_ = pLastDealloc_ = 0;
}

void* SmallObjectAllocator::Allocate(std::size_t numBytes)
{
    if (numBytes > maxObjectSize_) return operator new(numBytes);

    // if last used allocator is of the right size return it, else search it in the pool vector
    if (pLastAlloc_ && pLastAlloc_->GetBlockSize() == numBytes)
    {
        return pLastAlloc_->Allocate();
    }

    // perform a binary search to find the right allocator
    std::vector<FixedAllocator>::iterator i = std::lower_bound(pool_.begin(), pool_.end(), numBytes,
        [](const FixedAllocator &allocator, std::size_t bytes)->bool {return allocator.GetBlockSize() < bytes;}); // cool little lambda I did :)
    if (i == pool_.end() || i->GetBlockSize() != numBytes)
    {
        i = pool_.insert(i, FixedAllocator(numBytes));
        pLastDealloc_ = &*pool_.begin();
    }
    pLastAlloc_ = &*i;
    return pLastAlloc_->Allocate();
}

void SmallObjectAllocator::Deallocate(void* p, std::size_t numBytes)
{
    if (numBytes > maxObjectSize_) return operator delete(p);

    if (pLastDealloc_ && pLastDealloc_->GetBlockSize() == numBytes)
    {
        pLastDealloc_->Deallocate(p);
        return;
    }
    std::vector<FixedAllocator>::iterator i = std::lower_bound(pool_.begin(), pool_.end(), numBytes,
        [](const FixedAllocator& allocator, std::size_t bytes)->bool {return allocator.GetBlockSize() < bytes;});

    pLastDealloc_ = &*i;
    pLastDealloc_->Deallocate(p);
}

