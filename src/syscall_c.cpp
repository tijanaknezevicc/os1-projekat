#include "../h/syscall_c.hpp"

static uint64 syscall(uint64 code, uint64 arg1 = 0, uint64 arg2 = 0, uint64 arg3 = 0, uint64 arg4 = 0) {
    register uint64 a0 asm("a0") = code;
    register uint64 a1 asm("a1") = arg1;
    register uint64 a2 asm("a2") = arg2;
    register uint64 a3 asm("a3") = arg3;
    register uint64 a4 asm("a4") = arg4;
    asm volatile("ecall"
        : "+r" (a0)
        : "r" (a1), "r" (a2), "r" (a3), "r" (a4)
        : "t0", "memory");
    return a0;
}

void *mem_alloc(size_t size) {
    size = (size + MEM_BLOCK_SIZE - 1) / MEM_BLOCK_SIZE;
    return (void *)syscall(0x01, size);
}

int mem_free(void *addr) {
    return (int)syscall(0x02, (uint64)addr);
}

int thread_create(thread_t *handle, void (*start_routine)(void *), void *arg) {
    void* stack = mem_alloc(DEFAULT_STACK_SIZE);
    if (stack == nullptr) { return -1; }
    return (int)syscall(0x11, (uint64)handle, (uint64)start_routine, (uint64)arg, (uint64)stack);
}

int thread_exit() {
    return (int)syscall(0x12);
}

void thread_dispatch() {
    syscall(0x13);
}

char getc() {
    return (char)syscall(0x41);
}

void putc(char c) {
    syscall(0x42, (uint64)c);
}
