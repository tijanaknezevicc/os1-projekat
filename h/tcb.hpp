#ifndef CCB_HPP
#define CCB_HPP

#include "memoryAllocator.hpp"
#include "scheduler.hpp"

// Thread Control Block
class TCB {
public:
    ~TCB() { MemoryAllocator::memFree(stack); }

    bool isFinished() const { return finished; }
    void setFinished(bool value) {
        if (value && !finished) { activeCnt--; }
        finished = value;
    }

    void block() { blocked = true; };
    void deblock() { blocked = false; }
    bool isBlocked() { return blocked; }

    int waiting;
    int wakeupReason; // 0 - regularno, -1 - greska, semafor zatvoren

    void markIdle() {
        if (!idle) {
            idle = true;
            activeCnt--;
        }
    }

    static int getActiveCount() { return activeCnt; }

    using Body = void (*)(void*);

    static TCB *createThread(Body body, void* arg, uint64* stack);

    static void yield();
    static void dispatch();

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
            finished(false),
            blocked(false),
            idle(false)
    {
        activeCnt++;
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
    bool blocked;
    bool idle;

    static int activeCnt;

    friend class Riscv;

    static void threadWrapper();

    static void contextSwitch(Context *oldContext, Context *runningContext);
};

#endif //TCB_HPP
