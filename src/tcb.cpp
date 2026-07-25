#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

// #include "../h/gutenberg.hpp"

TCB *TCB::running = nullptr;
int TCB::activeCnt = 0;

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

    if (!old->isFinished() && !old->isBlocked()) { Scheduler::put(old); }
    running = Scheduler::get();

    if (old != running) { contextSwitch(&old->context, &running->context); }
}

void TCB::threadWrapper() {
    Riscv::mc_sstatus(Riscv::SSTATUS_SPP);
    Riscv::popSppSpie();
    running->body(running->arg);
    running->setFinished(true);
    thread_exit();
}
