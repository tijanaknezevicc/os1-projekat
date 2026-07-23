#ifndef LIST_HPP
#define LIST_HPP

#include "memoryAllocator.hpp"

template<typename T>
class List {
private:
    struct Node {
        T *data;
        Node *next;

        Node(T *data, Node *next) : data(data), next(next) {}

        static void* operator new(size_t size) { return MemoryAllocator::memAlloc(size); }
        static void operator delete(void* ptr) { MemoryAllocator::memFree(ptr); }
    };

    Node *head, *tail;

public:
    List() : head(0), tail(0) {}

    List(const List<T> &) = delete;

    List<T> &operator=(const List<T> &) = delete;

    void addFirst(T *data) {
        Node *node = new Node(data, head);
        head = node;
        if (!tail) { tail = head; }
    }

    void addLast(T *data) {
        Node *node = new Node(data, 0);
        if (tail) {
            tail->next = node;
            tail = node;
        } else {
            head = tail = node;
        }
    }

    T *removeFirst() {
        if (!head) { return 0; }

        Node *node = head;
        head = head->next;
        if (!head) { tail = 0; }

        T *ret = node->data;
        delete node;
        return ret;
    }

    T *peekFirst() {
        if (!head) { return 0; }
        return head->data;
    }

    T *removeLast() {
        if (!head) { return 0; }

        Node *prev = 0;
        for (Node *curr = head; curr && curr != tail; curr = curr->next) {
            prev = curr;
        }

        Node *node = tail;
        if (prev) { prev->next = 0; }
        else { head = 0; }
        tail = prev;

        T *ret = node->data;
        delete node;
        return ret;
    }

    T *peekLast() {
        if (!tail) { return 0; }
        return tail->data;
    }
};

#endif //LIST_HPP
