#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/memoryAllocator.hpp"
#include "../h/syscall_c.hpp"
#include "../h/gutenberg.hpp"

void userMain();

void userMainWrapper(void* arg) {
    (void) arg;
    userMain();
}

void idleBody(void* arg) {
    (void) arg;
    while (TCB::getActiveCount() > 0) {
        for (volatile int i = 0; i < 10000; i++) { }
        // print_str(".");
        thread_dispatch();
    }
    *(volatile uint32*) 0x100000 = 0x5555; // gotove sve korisnicke niti
}

extern "C" void main() {
    MemoryAllocator::memInit();
    asm volatile("csrw stvec, %0" :: "r"(&Riscv::supervisorTrap));
    Riscv::ms_sie(Riscv::SIE_SEIE);
    Riscv::mc_sie(Riscv::SIE_SSIE);

    TCB::running = TCB::createThread(nullptr, nullptr, nullptr);

    thread_t idle_h;
    thread_create(&idle_h, idleBody, nullptr);
    ((TCB*)idle_h)->markIdle();

    thread_t h;
    thread_create(&h, userMainWrapper, nullptr);

    TCB::running->setFinished(true);
    thread_dispatch();

    while (1) {}
}