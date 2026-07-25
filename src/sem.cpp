#include "../h/sem.hpp"
// #include "../h/gutenberg.hpp"

int Sem::wait(int n) {
    val -= n;
    if (val < 0) {
        int deficit = -val; // !!!!!!!!!!!!!!!!!!!!! ubcs
        val = 0;
        block(deficit);
        if (TCB::running->wakeupReason == -1) {
            return -1;
        }
    }
    return 0;
}

void Sem::signal(int n) {

    // print_str("SIGNAL this="); print_x((uint64)this);
    // print_str(" n="); print_x(n);
    // print_str(" val="); print_x(val); print_str("\n");

    for (int i = 0; i < n; i++) {
        TCB* head = blocked.peekFirst();
        if (head) {
            if (--head->waiting == 0) { deblock(); }
        }
        else { val++; }
    }
}

void Sem::block(int n) {

    // print_str("BLOCK this="); print_x((uint64)this);
    // print_str(" thr="); print_x((uint64)TCB::running);
    // print_str(" n="); print_x(n); print_str("\n");
    // print_str("val="); print_x(val); print_str("\n");

    TCB::running->waiting = n;
    TCB::running->block();
    blocked.addLast(TCB::running);
    TCB::yield();
}

void Sem::deblock() {
    TCB* thr = blocked.removeFirst();

    // print_str("DEBLOCK this="); print_x((uint64)this);
    // print_str(" thr="); print_x((uint64)thr); print_str("\n");

    thr->wakeupReason = 0; // regularno odblokirana
    thr->deblock();
    Scheduler::put(thr);
}

Sem::~Sem () {
    while (TCB* thr = blocked.removeFirst()) {
        thr->wakeupReason = -1; // vracena u scheduler jer je semafor zatvoren
        thr->deblock();
        Scheduler::put(thr);
    }
}