#include "TestPhysicalMemoryMap.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>

void TestPhysicalMemoryMap::setUp(void)
{
    mLocalMap = new PhysicalMemoryMap();
}


void TestPhysicalMemoryMap::tearDown(void) 
{
    delete mLocalMap;
    mLocalMap = NULL;
}


void TestPhysicalMemoryMap::testGetFreeChunk(void)
{
    struct memory_chunk *first;
    struct memory_chunk *second;

    first = mLocalMap->getFreeChunk();
    TS_ASSERT_DIFFERS(first, (struct memory_chunk*)NULL);

    first->address = 0xDEADBEEF;
    first->length = 0x1000;
    first->status = FREE_MEMORY;

    second = mLocalMap->getFreeChunk();
    TS_ASSERT_DIFFERS(second, (struct memory_chunk*)NULL);
    TS_ASSERT_DIFFERS(first, second);
    TS_ASSERT_DIFFERS(second->address, 0xDEADBEEFu);
    TS_ASSERT_DIFFERS(second->length, 0x1000u);
    TS_ASSERT_EQUALS(second->status, UNKNOWN_MEMORY);

    /* As the object is hightly verified, we need
     * to send back theses objects to avoir a failure */
    mLocalMap->addChunk(first);
    mLocalMap->addChunk(second);
}


void TestPhysicalMemoryMap::feedMemoryMap(int count, uint64_t base_address, uint64_t base_size)
{
   struct memory_chunk *descriptor;

   for (int i = 0; i < count; i++) {
      descriptor = mLocalMap->getFreeChunk();
      TS_ASSERT_DIFFERS(descriptor, (struct memory_chunk*)NULL);
      descriptor->address = base_address + 0x1000 * i;
      descriptor->length = base_size + 0x1000 * i;
      descriptor->status = FREE_MEMORY;
      mLocalMap->addChunk(descriptor);
   }
}


void TestPhysicalMemoryMap::testClear(void)
{
   bool b;
   struct memory_chunk *mc;
   
   feedMemoryMap(10, 0xDEADBEEFu, 0);
   mLocalMap->clear();

   mc = mLocalMap->current();
   TS_ASSERT_EQUALS(mc, (struct memory_chunk*)NULL);
   b = mLocalMap->next();
   TS_ASSERT_EQUALS(b, false);
   mc = mLocalMap->current();
   TS_ASSERT_EQUALS(mc, (struct memory_chunk*)NULL);
}


void TestPhysicalMemoryMap::testCurrent(void)
{
    struct memory_chunk *mc;

    mc = mLocalMap->current();
    TS_ASSERT_EQUALS(mc, (struct memory_chunk*)NULL);

    /* Create a chunk descriptor */
    mc = mLocalMap->getFreeChunk();
    TS_ASSERT_DIFFERS(mc, (struct memory_chunk*)NULL);

    mc->address = 0xBAADB00Du;
    mc->length = 0x1000;
    mc->status = FREE_MEMORY;
    mLocalMap->addChunk(mc);
    mc = (struct memory_chunk*)NULL;

    /* Get and check the created */
    mc = mLocalMap->current();
    TS_ASSERT_DIFFERS(mc, (struct memory_chunk*)NULL);
    TS_ASSERT_EQUALS(mc->address, 0xBAADB00Du);
    TS_ASSERT_EQUALS(mc->length, 0x1000u);
    TS_ASSERT_EQUALS(mc->status, FREE_MEMORY);

    /* Go to the next descriptor, it must be NULL */
    TS_ASSERT(!mLocalMap->next());
    mc = mLocalMap->current();
    TS_ASSERT_EQUALS(mc, (struct memory_chunk*)NULL);

    /* Rewind and check it's the right one */
    mLocalMap->rewind();
    mc = mLocalMap->current();
    TS_ASSERT_DIFFERS(mc, (struct memory_chunk*)NULL);
    TS_ASSERT_EQUALS(mc->address, 0xBAADB00Du);
    TS_ASSERT_EQUALS(mc->length, 0x1000u);
    TS_ASSERT_EQUALS(mc->status, FREE_MEMORY);

    /* Empty the list, now it's NULL */
    mLocalMap->clear();
    mc = mLocalMap->current();
    TS_ASSERT_EQUALS(mc, (struct memory_chunk*)NULL);
}


void TestPhysicalMemoryMap::testNext(void)
{
    /* Test parameters */
    const int descriptorCount = 10;
    const uint64_t descriptorBase = 0x0u;
    const uint64_t descriptorSize = 0x0u;
    /* Test checkers */
    int checkCount = 0;
    struct memory_chunk *mc;

    /* Feed the map with chunk descriptors */
    feedMemoryMap(descriptorCount, descriptorBase, descriptorSize);

    while (mLocalMap->next()) {
        mc = mLocalMap->current();
        TS_ASSERT_DIFFERS(mc, (struct memory_chunk*)NULL);
        TS_ASSERT_LESS_THAN_EQUALS(mc->address, descriptorBase + descriptorCount * 0x1000u);
        TS_ASSERT_LESS_THAN_EQUALS(mc->length, descriptorSize + descriptorCount * 0x1000u);
        checkCount++;
    }
    TS_ASSERT_EQUALS(checkCount + 1, descriptorCount);

    /* Empty the map and check that next behaves as expected */
    mLocalMap->clear();
    TS_ASSERT(!mLocalMap->next());
}


void TestPhysicalMemoryMap::testRewind(void)
{
    /* Test parameters */
    const int descriptorCount = 10;
    const uint64_t descriptorBase = 0x0u;
    const uint64_t descriptorSize = 0x0u;
    /* Test checkers */
    int checkCount = 0;
    struct memory_chunk *mc = NULL;

    /* Feed the map with chunk descriptors */
    feedMemoryMap(descriptorCount, descriptorBase, descriptorSize);

    /* Test rewind function multiple times */
    for (int i = 0; i < 100; i++) {
        checkCount = 0;
        mc = (struct memory_chunk*) NULL;

        /* Check list values*/
        while (mLocalMap->next()) {
            mc = mLocalMap->current();
            TS_ASSERT_DIFFERS(mc, (struct memory_chunk*)NULL);
            TS_ASSERT_LESS_THAN_EQUALS(mc->address, descriptorBase + descriptorCount * 0x1000u);
            TS_ASSERT_LESS_THAN_EQUALS(mc->length, descriptorSize + descriptorCount * 0x1000u);
            checkCount++;
        }
        TS_ASSERT_EQUALS(checkCount + 1, descriptorCount);

        /* Rewind the head of the list */
        mLocalMap->rewind();
        TS_ASSERT_DIFFERS(mLocalMap->current(), (struct memory_chunk*)NULL);
    }

    /* Check we don't lose anything */
    mLocalMap->clear();
    TS_ASSERT(!mLocalMap->next());
}

