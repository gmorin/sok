/*
 * Copyright (C) 2012, the Simple Object Kernel project.
 * by Damien Dejean <djod4556@yahoo.fr>
 *
 * PhysicalMemoryMap.cpp: Implementation of a list that describes the 
 * physical memory map of a computer. This list is made of memory chunk
 * descriptors that are chained together. Internally they are bigger than
 * the structure described in the header file. There's a chain field and
 * a magic value to ensure that the chunk have not been corrupted by
 * bad manipulation.
 *
 * Chunks are allocated into the bootstrap allocator by us, and freed by
 * us too. If a memory corruption is detected an assertion is raised.
 */

#include "assert.h"
#include "string.h"
#include "stdio.h"
#include "PhysicalMemoryMap.h"

/** Internal struct to be able to chain chunks between them */
struct chained_memory_chunk {
    struct memory_chunk base;
    uint32_t magic;
    struct chained_memory_chunk *next;
};

/** Magic to control integrity */
#define     CHUNK_MAGIC     0xCAFEBABEu


PhysicalMemoryMap::PhysicalMemoryMap()
{
    mAllocator = BootstrapAllocator::getInstance();
    mList = (struct chained_memory_chunk*) NULL;
    mCurrent = (struct chained_memory_chunk*) NULL;
    mCount = 0;
}

PhysicalMemoryMap::~PhysicalMemoryMap()
{
    clear();
    mAllocator = 0;
}


struct memory_chunk* PhysicalMemoryMap::getFreeChunk(void)
{
    struct chained_memory_chunk *cmc;

    /* Prepare chunk */
    cmc = (struct chained_memory_chunk*) mAllocator->alloc(sizeof(*cmc));
    assert(cmc != NULL);
    memset(cmc, 0, sizeof(*cmc));
    cmc->magic = CHUNK_MAGIC;
    cmc->base.status = UNKNOWN_MEMORY;

    /* Count one more */
    mCount++;

    return (struct memory_chunk*)cmc;
}


void PhysicalMemoryMap::addChunk(struct memory_chunk *chunk)
{
    struct chained_memory_chunk *cmc;

    /* Check chunk */
    assert(chunk != NULL);
    cmc = (struct chained_memory_chunk*) chunk;
    assert(cmc->magic == CHUNK_MAGIC);

    /* Chain it as head of the list */
    cmc->next = mList;
    mList = cmc;
    mCurrent = mList;
}


void PhysicalMemoryMap::clear(void)
{
    /* Empty the list */
    for (mCurrent = mList; 
         mList != NULL; 
         mCurrent = mList->next, mList = mList->next)
    {
        assert(mCurrent->magic == CHUNK_MAGIC);
        mAllocator->free(mCurrent, sizeof(*mCurrent));
        mCount--;
    }
    mList = 0;
    mCurrent = 0;
    /* Check there's no memory leak */
    assert(mCount == 0);
}


struct memory_chunk* PhysicalMemoryMap::current(void)
{
   if (mCurrent != NULL) {
       assert(mCurrent->magic == CHUNK_MAGIC);
   }
   return (struct memory_chunk*) mCurrent;
}


bool PhysicalMemoryMap::next(void)
{
    if (mCurrent != NULL) {
        assert(mCurrent->magic == CHUNK_MAGIC);
        mCurrent = mCurrent->next;
        if (mCurrent == NULL) {
            return false;
        } else {
            return true;
        }
    } else {
        return false;
    }
}


void PhysicalMemoryMap::rewind(void)
{
    mCurrent = mList;
}

