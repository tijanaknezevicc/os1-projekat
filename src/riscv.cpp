#include "../h/riscv.hpp"
#include "../h/memoryAllocator.hpp"
#include "../h/tcb.hpp"
#include "../lib/console.h"
#include "../h/sem.hpp"
#include "../h/gutenberg.hpp"

void Riscv::popSppSpie() {
    __asm__ volatile("csrw sepc, ra");
    __asm__ volatile("sret");
}

void Riscv::handleSupervisorTrap(uint64* regs) {

    uint64 scause = r_scause();
    uint64 code = regs[10]; // a0

    if (scause == 0x0000000000000008UL || scause == 0x0000000000000009UL) {
        // interrupt: no; cause code: environment call from U-mode(8) or S-mode(9)
        uint64 volatile sepc = r_sepc() + 4;
        uint64 volatile sstatus = r_sstatus();

        switch (code) {
            case 0x01: { // mem_alloc
                regs[10] = (uint64)MemoryAllocator::memAlloc(regs[11] * MEM_BLOCK_SIZE);
                break;
            }

            case 0x02: { // mem_free
                regs[10] = (uint64)MemoryAllocator::memFree((void*)regs[11]);
                break;
            }

            case 0x11: { // thread_create
                uint64* handleP = (uint64*)regs[11];
                TCB::Body startRoutine = (TCB::Body)regs[12];
                void* arg = (void*)regs[13];
                uint64* stack = (uint64*)regs[14];

                TCB* newThread = TCB::createThread(startRoutine, arg, stack);

                // if (newThread == nullptr) print_str("THREAD_CREATE FAILED\n"); // brisi

                *handleP = (uint64)newThread;
                regs[10] = 0;
                break;
            }

            case 0x12: { // thread_exit
                TCB::running->setFinished(true);
                TCB::yield();
                regs[10] = 0;
                break;
            }

            case 0x13: { // thread_dispatch
                TCB::yield();
                break;
            }

            case 0x21: { // sem_open
                uint64* handleP = (uint64*)regs[11];
                Sem* newSem = new Sem((unsigned)regs[12]);
                *handleP = (uint64)newSem;
                regs[10] = 0;
                break;
            }

            case 0x22: { // sem_close
                Sem* sem = (Sem*)regs[11];
                delete sem;
                regs[10] = 0;
                break;
            }

            case 0x23: { // sem_wait
                Sem* sem = (Sem*)regs[11];
                regs[10] = sem->wait();
                break;
            }

            case 0x24: { // sem_signal
                Sem* sem = (Sem*)regs[11];
                sem->signal();
                regs[10] = 0;
                break;
            }

            case 0x25: { // sem_wait_n
                Sem* sem = (Sem*)regs[11];
                regs[10] = sem->wait((unsigned)regs[12]);
                break;
            }

            case 0x26: { // sem_signal_n
                Sem* sem = (Sem*)regs[11];
                sem->signal((unsigned)regs[12]);
                regs[10] = 0;
                break;
            }

            case 0x41: { // getc
                // print_str("GETC start\n");
                regs[10] = (uint64) __getc();
                // print_str("GETC done\n");
                break;
            }

            case 0x42: { // putc
                __putc((char)regs[11]);
                break;

            }
            default: {
                regs[10] = (uint64)-1;
                break;
            }
        }

        // TCB::timeSliceCounter = 0;
        // TCB::dispatch();
        w_sstatus(sstatus);
        w_sepc(sepc);
    }

    // else if (scause == 0x8000000000000001UL) {
    //     // interrupt: yes; cause code: supervisor software interrupt (CLINT; machine timer interrupt)
    //     mc_sip(SIP_SSIP);
    //     TCB::timeSliceCounter++;
    //
    //     if (TCB::timeSliceCounter >= TCB::running->getTimeSlice()) {
    //         uint64 volatile sepc = r_sepc();
    //         uint64 volatile sstatus = r_sstatus();
    //         TCB::timeSliceCounter = 0;
    //         TCB::dispatch();
    //         w_sstatus(sstatus);
    //         w_sepc(sepc);
    //     }
    // }

    else if (scause == 0x8000000000000009UL) {
        // interrupt: yes; cause code: supervisor external interrupt (PLIC; could be keyboard)
        console_handler();
    }
    else {
        // unexpected trap cause
        print_str("PANIC\nscause = ");
        print_x(scause);
        print_str("\nsepc = ");
        print_x(r_sepc());
        print_str("\nstval = ");
        print_x(r_stval());
        print_str("\n");

        *(volatile uint32*) 0x100000 = 0x5555; // kraj
    }
}