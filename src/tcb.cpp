#include "../h/tcb.hpp"
#include "../h/riscv.hpp"
#include "../h/syscall_c.hpp"

TCB *TCB::running = nullptr;

TCB *TCB::createThread(Body body, void* arg, uint64* stack) {
    return new TCB(body, arg, stack);
}

void TCB::switchTo(TCB* next) { // zbog semafora
    TCB* old = running;
    running = next;
    Riscv::pushRegisters();
    contextSwitch(&old->context, &running->context);
    Riscv::popRegisters();
}

void TCB::yield() {
    TCB *old = running;
    if (!old->isFinished()) { Scheduler::put(old); }
    switchTo(Scheduler::get());
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
