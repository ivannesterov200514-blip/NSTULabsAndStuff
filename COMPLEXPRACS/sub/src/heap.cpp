#include "heap.h"
#include <stdexcept>
#include <utility>

Heap::Heap(int cap) {
    capacity = cap > 0 ? cap : 2;
    data = new HeapNode[capacity];
    size = 0;
}

void Heap::heap_resize(int newcap) {
    if (newcap <= 0 || newcap < size) {
        throw invalid_argument("resize(heap): new capacity is smaller than size");
    }
    HeapNode* newdata = new HeapNode[newcap];
    for (int i = 0; i < size; i++) newdata[i] = data[i];
    delete[] data;
    data = newdata;
    capacity = newcap;
}

void Heap::siftUp(int idx) {
    if (idx == 0) return;
    int parentIdx = (idx - 1) / 2;
    HeapNode parent = data[parentIdx];

    if (higherPriority(data[idx], parent)) {
        data[parentIdx] = data[idx];
        data[idx] = parent;
        siftUp(parentIdx);
    }
}


void Heap::siftDown(int idx) {
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;
    int best = idx;

    if (left < size && higherPriority(data[left], data[best])) {
        best = left;
    }
    if (right < size && higherPriority(data[right], data[best])) {
        best = right;
    }
    if (best == idx) return;

    HeapNode temp = data[idx];
    data[idx] = data[best];
    data[best] = temp;
    siftDown(best);
}


bool Heap::higherPriority(HeapNode a, HeapNode b) {
    if (a.priority != b.priority) {
        return a.priority > b.priority;
    }
    return a.order < b.order;
}

HeapNode Heap::heap_peek() {
    if (size == 0) {
        throw out_of_range("Куча пустая");
    }
    return data[0];
}

void Heap::heap_push(HeapNode* node) {
    if (node == nullptr) {
        throw invalid_argument("Пустой указатель");
    }
    if (size >= capacity) heap_resize(capacity*2);
    data[size] = *node;
    siftUp(size);
    size++;
}

HeapNode Heap::heap_pop() {
    if (size == 0) {
        throw out_of_range("Куча пустая");
    }

    HeapNode temp = data[0];
    data[0] = data[size-1];
    size--;
    if (size > 0) {
        siftDown(0);
    }

    return temp;
}

int Heap::heap_getsz() {
    return size;
}

int Heap::heap_getcap() {
    return capacity;
}

void Heap::swap(Heap& other) noexcept {
    std::swap(data, other.data);
    std::swap(size, other.size);
    std::swap(capacity, other.capacity);
}

Heap::~Heap() { delete[] data; }