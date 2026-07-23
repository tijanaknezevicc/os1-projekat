#ifndef MEMORYALLOCATOR_HPP
#define MEMORYALLOCATOR_HPP

#include "../lib/hw.h"

class MemoryAllocator {
public:
    static void *memAlloc(size_t size);
    static int memFree(void *addr);
    static void memInit();

private:
    static int *memMap;
    static int numOfBlocks;
    static uint64 beg;
    static int fragmentSize(int k);
    static void mergeWithNext(int blk);
    static int alloc(int size);

};

#endif //MEMORYALLOCATOR_HPP
