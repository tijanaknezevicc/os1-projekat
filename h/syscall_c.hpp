#ifndef SYSCALL_C_HPP
#define SYSCALL_C_HPP

#include "../lib/hw.h"

void* mem_alloc (size_t size);
int mem_free (void* addr);

class _thread;
typedef _thread* thread_t;
int thread_create (
        thread_t* handle,
        void(*start_routine)(void*),
        void* arg
);
int thread_exit ();
void thread_dispatch ();

// class _sem;
// typedef _sem* sem_t;
// int sem_open (
//         sem_t* handle,
//         unsigned init
// );//
// int sem_close (sem_t handle);
// int sem_wait (sem_t id);
// int sem_signal (sem_t id);
// int sem_wait_n(sem_t id, unsigned n);
// int sem_signal_n(sem_t id, unsigned n);
//
// typedef unsigned long time_t;
// int time_sleep (time_t);
//
const int EOF = -1;
char getc ();
void putc (char);

#endif //SYSCALL_C_HPP
