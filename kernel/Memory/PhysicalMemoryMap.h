/*
 * Copyright (C) 2012, the Simple Object Kernel project.
 * by Damien Dejean <djod4556@yahoo.fr>
 *
 * PhysicalMemoryMap.h: provides a description of physical memory regions and
 * their status.
 */

#ifndef _PHYSICAL_MEMORY_MAP_H_
#define _PHYSICAL_MEMORY_MAP_H_

#include "stdint.h"
#include "BootstrapAllocator.h"

/** Status of a physical memory chunk */
enum chunk_status {
    FREE_MEMORY,
    RESERVED_MEMORY,
    UNKNOWN_MEMORY
};

/** Descriptor of a physical memory chunk */
struct memory_chunk {
    uint64_t            address;
    uint64_t            length;
    enum chunk_status   status;
};

/** Forward declaration for chained chunks */
struct chained_memory_chunk;

/**
 * Provides a list of physical memory chunks. The class implements two
 * interfaces. One designed for the factory which will build the object,
 * the second one designed for the user of the memory map, presented as
 * an iterator.
 */
class PhysicalMemoryMap {
    private:
        /** The allocator we'll get the descriptors */
        BootstrapAllocator *mAllocator;

        /** Head of the descriptor list */
        struct chained_memory_chunk *mList;

        /** Cursor to the current analyzed chunk */
        struct chained_memory_chunk *mCurrent;

        /** Count of the number off allocated descriptors */
        int mCount;

    public:
        PhysicalMemoryMap(void);
        ~PhysicalMemoryMap(void);

        /**
         * Obtain a chunk descriptor.
         * @return an empty memory chunk descriptor.
         */
        struct memory_chunk *getFreeChunk(void);
        
        /**
         * Add a filled chunk descriptor to the descriptor list.
         * @param chunk the chunk to add.
         */
        void addChunk(struct memory_chunk *chunk);

        /**
         * Empty the descriptor list.
         */
        void clear(void);

        /**
         * Get the current chunk descriptor of the iterator.
         * @return a memory chunk descriptor pointed by the iterator,
         *         NULL if we are at the end of the list or if the list
         *         is empty.
         */
        struct memory_chunk *current(void);

        /**
         * Move the iterator cursor forward, to the next chunk descriptor.
         * @return true if the referenced descriptor is valid, false if we are
         *         at the end of the list (corollary: a call to current() will
         *         return NULL). 
         */
        bool next(void);

        /**
         * Reset the iterator reinding the cursor at the head of the list.
         */
        void rewind(void);
};

#endif /*_PHYSICAL_MEMORY_MAP_ */
