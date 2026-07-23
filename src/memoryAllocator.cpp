#include "../h/memoryAllocator.hpp"

int *MemoryAllocator::memMap = nullptr;
int MemoryAllocator::numOfBlocks = 0;
uint64 MemoryAllocator::beg = 0;

int MemoryAllocator::fragmentSize(int k) {
    if (k < 0)
        return -k;
    return k;
}

void MemoryAllocator::memInit() {
    int totalBlocks = ((uint64)HEAP_END_ADDR - (uint64)HEAP_START_ADDR) / MEM_BLOCK_SIZE; // svi blokovi, zajedno sa map nizom
    int mapBytes = totalBlocks * sizeof(int); // velicina map niza u bajtovima
    int mapBlocks = (mapBytes + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE; // ceil(mapBytes / MEM_BLOCK_SIZE)

    numOfBlocks = totalBlocks - mapBlocks;
    memMap = (int *)HEAP_START_ADDR;
    beg = (uint64)HEAP_START_ADDR + mapBlocks * MEM_BLOCK_SIZE;

    for (int i = 0; i < numOfBlocks; i++) {
        memMap[i] = 0;
    }

    memMap[0] = memMap[numOfBlocks - 1] = numOfBlocks; // prvi i poslednji blok segmenta u sebi imaju velicinu
}

int MemoryAllocator::alloc(int size) {
    size = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE; // iz bajtova u blokove
    if (size <= 0 || size > numOfBlocks)
        return -1;

    int bestBlk = -1; // indeks
    int bestSize = 0; // najmanja velicina u koju staje size

    for (int i = 0; i < numOfBlocks; i += fragmentSize(memMap[i])) {
        if (memMap[i] >= size) {
            if (memMap[i] < bestSize || bestBlk == -1) {
                bestBlk = i;
                bestSize = memMap[i];
            }
            if (bestSize == size) // perfect fit
                break;
        }
    }

    if (bestBlk == -1)
        return -1;

    if (bestSize > size) {
        int left = bestSize - size;
        memMap[bestBlk + size] = memMap[bestBlk + size + left - 1] = left; // ostatak slobodne memorije u segmentu
    }

    memMap[bestBlk] = memMap[bestBlk + size - 1] = -size;
    return bestBlk;
}

void *MemoryAllocator::memAlloc(size_t size) {
    int blk = alloc(size); // indeks

    if (blk == -1)
        return nullptr;

    return (void *)(beg + blk * (uint64)MEM_BLOCK_SIZE); // adresa
}

void MemoryAllocator::mergeWithNext(int blk) {
    int size = memMap[blk];
    if (size <= 0 || blk + size >= numOfBlocks || memMap[blk + size] <= 0)
        return;

    int newSize = size + memMap[blk + size];
    memMap[blk + size - 1] = memMap[blk + size] = 0; // stare granice
    memMap[blk] = memMap[blk + newSize - 1] = newSize;
}

int MemoryAllocator::memFree(void *addr) {
    if ((uint64)addr < beg)
        return -1;

    int blk = ((uint64)addr - beg) / (uint64)MEM_BLOCK_SIZE; // indeks
    if (blk >= numOfBlocks)
        return -1;

    int size = memMap[blk];
    if (size > 0)
        return -1;

    size = -size;
    memMap[blk] = memMap[blk + size - 1] = size;

    mergeWithNext(blk);

    if (blk > 0 && memMap[blk - 1] > 0) {
        int prev = blk - memMap[blk - 1];
        mergeWithNext(prev);
    }

    return 0;
}