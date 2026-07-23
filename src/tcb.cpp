#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

TCB *TCB::running = nullptr;

TCB *TCB::createThread(Body body, void* arg, uint64* stack) {
    return new TCB(body, arg, stack);
}

void TCB::yield() {
    Riscv::pushRegisters();
    dispatch();
    Riscv::popRegisters();
}

void TCB::dispatch() {
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    running = Scheduler::get();

    contextSwitch(&old->context, &running->context);
}

void TCB::threadWrapper() {
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    thread_exit();
}
