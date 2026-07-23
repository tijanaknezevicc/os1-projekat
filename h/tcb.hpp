#ifndef CCB_HPP
#define CCB_HPP

#include "memoryAllocator.hpp"
#include "../lib/hw.h"
#include "scheduler.hpp"

// Thread Control Block
class TCB {
public:
    ~TCB() { MemoryAllocator::memFree(stack); }

    bool isFinished() const { return finished; }
    void setFinished(bool value) { finished = value; }

    using Body = void (*)(void*);

    static TCB *createThread(Body body, void* arg, uint64* stack);

    static void yield();

    static TCB *running;

    static void* operator new(size_t size) { return MemoryAllocator::memAlloc(size); }
    static void operator delete(void* ptr) { MemoryAllocator::memFree(ptr); }

private:
    explicit TCB(Body body, void* arg, uint64* stack) :
            body(body),
            arg(arg),
            stack(stack),
            context({body != nullptr ? (uint64) &TCB::threadWrapper : 0,
                     stack != nullptr ? (uint64) ((char*)stack + DEFAULT_STACK_SIZE) : 0
                    }),
            finished(false)
    {
        if (body != nullptr) { Scheduler::put(this); }
    }

    struct Context {
        uint64 ra;
        uint64 sp;
    };

    Body body;
    void* arg;
    uint64 *stack;
    Context context;
    bool finished;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);

    static void dispatch();
};

#endif //TCB_HPP
