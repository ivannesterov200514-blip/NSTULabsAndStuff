
#pragma once
#include <cstdint>

using namespace std;

struct HeapNode {
    uint64_t id;
    unsigned int priority;
    uint64_t order;
};

class Heap {
private:
    HeapNode* data;
    int size;
    int capacity;

    void heap_resize(int newcap);
    void siftUp(int idx);
    void siftDown(int idx);
    bool higherPriority(HeapNode a, HeapNode b);

public:
    Heap(int cap = 2);
    Heap(const Heap&) = delete;
    Heap& operator=(const Heap&) = delete;

    HeapNode heap_peek();
    void heap_push(HeapNode* node);
    HeapNode heap_pop();

    int heap_getsz();
    int heap_getcap();
    void swap(Heap& other) noexcept;

    ~Heap();
};