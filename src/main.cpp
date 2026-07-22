#include "../lib/hw.h"

extern "C" void trapEntry();

extern "C" void main() {
    asm volatile("csrw stvec, %0" :: "r"(&trapEntry));

    register uint64 a0 asm("a0") = 0x01;
    register uint64 a1 asm("a1") = 5;
    asm volatile("ecall" : "+r"(a0) : "r"(a1) : "t0", "memory");

    while (1) {}
}
