#include "../h/riscv.hpp"
#include "../h/tcb.hpp"
#include "../h/memoryAllocator.hpp"
#include "../h/syscall_c.hpp"

extern "C" void main() {
    MemoryAllocator::memInit();
    asm volatile("csrw stvec, %0" :: "r"(&Riscv::supervisorTrap));
    // Riscv::ms_sie(Riscv::SIE_SEIE);

    TCB::running = TCB::createThread(nullptr, nullptr, nullptr);

    char c = getc();
    putc(c);
    putc('\n');

    while (1) {}
}
