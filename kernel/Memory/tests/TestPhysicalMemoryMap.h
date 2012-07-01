#ifndef _TEST_PHYSICAL_MEMORY_MAP__H_
#define _TEST_PHYSICAL_MEMORY_MAP__H_

#include "CxxTest/TestSuite.h"
#include "Memory/PhysicalMemoryMap.h"

class TestPhysicalMemoryMap: public CxxTest::TestSuite {
    private:
        PhysicalMemoryMap *mLocalMap;
        /**
         * Fill the mLocalMap with <count> chunk descriptors. They'll have
         * the provided address as base + count * 0x1000. The size will be
         * computed on the same model. Chunk type will be set at FREE_MEMORY.
         *
         * @param count the number of chunks to provide
         * @param base_address the base address for the computation described
         *                     above
         * @param base_size idem
         */
        void feedMemoryMap(int count, uint64_t base_adress, uint64_t base_size);

    public:
        void setUp(void);
        void tearDown(void);

        void testGetFreeChunk(void);
        void testClear(void);
        void testCurrent(void);
        void testNext(void);
        void testRewind(void);
};

#endif /* _TEST_PHYSICAL_MEMORY_MAP__H_ */
