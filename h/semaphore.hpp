#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include "tcb.hpp"

class Semaphore {
public:
    Semaphore (unsigned initValue = 1) : val(initValue) {}
    ~Semaphore ();

    // void wait () { if (--val < 0) block(); }
    // void signal () { if (val++ < 0) deblock(); }

    void wait(int n = 1);
    void signal(int n = 1);

    friend void signalWait (Semaphore* s, Semaphore* w);
    int value () { return val; }

    static void* operator new(size_t size) { return MemoryAllocator::memAlloc(size); }
    static void operator delete(void* addr) { MemoryAllocator::memFree(addr); }

protected:
    void block (int n = 1);
    void deblock ();

    int val;

private:
     List<TCB> blocked;
};

#endif //SEMAPHORE_HPP