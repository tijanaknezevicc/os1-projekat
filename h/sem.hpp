#ifndef SEM_HPP
#define SEM_HPP

#include "tcb.hpp"
#include "memoryAllocator.hpp"

class Sem {
public:
    Sem (unsigned initValue = 1) : val(initValue) {}
    ~Sem ();

    // void wait () { if (--val < 0) block(); }
    // void signal () { if (val++ < 0) deblock(); }

    int wait(int n = 1);
    void signal(int n = 1);

    friend void signalWait (Sem* s, Sem* w);
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

#endif //SEM_HPP