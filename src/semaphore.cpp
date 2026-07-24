#include "../h/semaphore.hpp"

void Semaphore::wait(int n) {
    val -= n;
    if (val < 0) {
        block(-val);
        val = 0;
    }
}

void Semaphore::signal(int n) {
    for (int i = 0; i < n; i++) {
        TCB* head = blocked.peekFirst();
        if (head) {
            if (--head->waiting == 0) { deblock(); }
        }
        else { val++; }
    }
}

void Semaphore::block(int n) {
    TCB::running->waiting = n;
    blocked.addLast(TCB::running);
    TCB::switchTo(Scheduler::get());
}

void Semaphore::deblock() {
    TCB* thr = blocked.removeFirst();
    Scheduler::put(thr);
}

Semaphore::~Semaphore () {
    while (TCB* thr = blocked.removeFirst()) {
        Scheduler::put(thr);
    }
}