#include "FixedAllocator.h"
#include <cassert>

#define CHUNKS_SIZE 2048 // how big should each chunk be in bytes?

FixedAllocator::FixedAllocator(std::size_t blockSize)
{
	blockSize_ = blockSize;
	allocChunk_ = deallocChunk_ = 0;
	numBlocks_ = static_cast<unsigned char>(CHUNKS_SIZE / blockSize);
}

void* FixedAllocator::Allocate()
{
	if (allocChunk_ == 0 || allocChunk_->blocksAvailable_ == 0)
	{
		Chunks::iterator i = chunks_.begin();
		for (;; ++i)
		{
			if (i == chunks_.end())
			{
				chunks_.reserve(chunks_.size() + 1);
				Chunk newChunk;
				newChunk.Init(blockSize_, numBlocks_); // create a new chunk big blockSize_ * numBlocks_ bytes
				chunks_.push_back(newChunk);
				allocChunk_ = &chunks_.back();
				deallocChunk_ = &chunks_.back();
				break;
			}
			if (i->blocksAvailable_ > 0)
			{
				allocChunk_ = &*i;
				break;
			}
		}
	}
	assert(allocChunk_ != 0);
	assert(allocChunk_->blocksAvailable_ > 0);
	return allocChunk_->Allocate(blockSize_);
}

void FixedAllocator::Deallocate(void* p)
{
	// double linear search to find the chunk containing p block
	const std::size_t chunkSize = numBlocks_ * blockSize_;

	Chunk* leftIter = deallocChunk_;
	Chunk* rightIter = deallocChunk_ + 1;

	// Special case: deallocChunk_ is the last in the array
	if (rightIter == &chunks_.back() + 1) rightIter = 0;

	for (;;)
	{
		if (leftIter)
		{
			if (p >= leftIter->pData_ && p < leftIter->pData_ + chunkSize)
			{
				deallocChunk_ = leftIter;
				break;
			}
			if (leftIter == &chunks_.front()) leftIter = 0;
			else --leftIter;
		}

		if (rightIter)
		{
			if (p >= rightIter->pData_ && p < rightIter->pData_ + chunkSize)
			{
				deallocChunk_ = rightIter;
				break;
			}
			if (++rightIter == &chunks_.back() + 1) rightIter = 0;
		}
	}

	assert(deallocChunk_); // to check it's not null
	deallocChunk_->Deallocate(p, blockSize_);

	// now deallocChunk_ points to the chunk which contained block p

	if (deallocChunk_->blocksAvailable_ == numBlocks_)
	{
		// deallocChunk_ is completely free, should we release it? Only if we have 2 empty chunks (optimization)

		Chunk& lastChunk = chunks_.back();

		if (&lastChunk == deallocChunk_)
		{
			// check if we have two last chunks empty

			if (chunks_.size() > 1 && deallocChunk_[-1].blocksAvailable_ == numBlocks_)
			{
				// Two free chunks, discard the last one
				delete[] lastChunk.pData_;
				chunks_.pop_back();
				allocChunk_ = deallocChunk_ = &chunks_.front();
			}
			return;
		}

		if (lastChunk.blocksAvailable_ == numBlocks_)
		{
			// Two free blocks, discard one
			delete[] lastChunk.pData_;
			chunks_.pop_back();
			allocChunk_ = deallocChunk_;
		}
		else
		{
			// move the empty chunk to the end
			std::swap(*deallocChunk_, lastChunk);
			allocChunk_ = &chunks_.back();
		}
	}
}

