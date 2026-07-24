#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/memoryAllocator.hpp"
#include "../h/syscall_c.hpp"

sem_t mutex;
volatile int shared = 0;

void printLine(char threadLetter, char action, int value) {
    putc(threadLetter);
    putc(':');
    putc(action);
    putc('=');
    if (value < 0) { putc('-'); value = -value; }
    putc('0' + value);
    putc('\n');

}

void workerA(void* arg) {
    (void) arg;
    for (int i = 0; i < 5; i++) {
        putc('A'); putc(':'); putc('w'); putc('a'); putc('i'); putc('t'); putc('\n');
        sem_wait(mutex);
        putc('A'); putc(':'); putc('w'); putc('o'); putc('k'); putc('e'); putc('\n');   // <-- NOVO
        shared++;
        printLine('A', '+', shared);
        thread_dispatch();
        sem_signal(mutex);
        thread_dispatch();
    }
}

void workerB(void* arg) {
    (void) arg;
    for (int i = 0; i < 5; i++) {
        putc('B'); putc(':'); putc('w'); putc('a'); putc('i'); putc('t'); putc('\n');
        sem_wait(mutex);
        putc('B'); putc(':'); putc('w'); putc('o'); putc('k'); putc('e'); putc('\n');   // <-- NOVO
        shared--;
        printLine('B', '-', shared);
        thread_dispatch();
        sem_signal(mutex);
        thread_dispatch();
    }
}

extern "C" void main() {
    MemoryAllocator::memInit();
    asm volatile("csrw stvec, %0" :: "r"(&Riscv::supervisorTrap));
    Riscv::ms_sie(Riscv::SIE_SEIE);
    Riscv::mc_sie(Riscv::SIE_SSIE);

    TCB::running = TCB::createThread(nullptr, nullptr, nullptr);

    sem_open(&mutex, 1);

    thread_t h1, h2;
    thread_create(&h1, workerA, nullptr);
    thread_create(&h2, workerB, nullptr);

    for (int i = 0; i < 30; i++) {
        putc('.');
        thread_dispatch();
    }

    putc('F'); putc('I'); putc('N'); putc('A'); putc('L'); putc('=');
    if (shared < 0) { putc('-'); shared = -shared; }
    putc('0' + shared);
    putc('\n');

    while (1) {}
}