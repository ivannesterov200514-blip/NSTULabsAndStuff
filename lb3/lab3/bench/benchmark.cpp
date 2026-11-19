#include "array.h"
#include "ulist.h"
#include "dlist.h"
#include "stack.h"
#include "queue.h"
#include "avltree.h"
#include "hashtable.h"

#include <chrono>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

const int REPEATS = 3;
unsigned long long checksum = 0; 

void show_result(const string& name, const string& operation,
                 int count, long long time_ns) {
    double average = (double)time_ns / REPEATS / count;
    cout << name << "," << operation << "," << count << "," << average << "\n";
}

void bench_array(const vector<string>& keys) {
    int n = (int)keys.size();
    long long insert_time = 0;
    long long search_time = 0;
    for (int repeat = 0; repeat < REPEATS; repeat++) {
        Arr a;
        auto start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) a.arr_insert_tail(keys[i]);
        auto finish = chrono::steady_clock::now();
        insert_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
        checksum += a.arr_getsz();

        start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) checksum += a.arr_search(i).size();
        finish = chrono::steady_clock::now();
        search_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
    }
    show_result("array", "push_back", n, insert_time);
    show_result("array", "get_by_index", n, search_time);
}

void bench_singly_linked_list(const vector<string>& keys) {
    int n = (int)keys.size();
    long long insert_time = 0;
    long long search_time = 0;
    for (int repeat = 0; repeat < REPEATS; repeat++) {
        UList a;
        auto start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) a.ul_insert_head(keys[i]);
        auto finish = chrono::steady_clock::now();
        insert_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
        checksum += a.ul_getsz();

        start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) checksum += a.ul_find(keys[i]) != nullptr;
        finish = chrono::steady_clock::now();
        search_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
    }
    show_result("singly_linked_list", "push_front", n, insert_time);
    show_result("singly_linked_list", "find_by_value", n, search_time);
}

void bench_doubly_linked_list(const vector<string>& keys) {
    int n = (int)keys.size();
    long long insert_time = 0;
    long long search_time = 0;
    for (int repeat = 0; repeat < REPEATS; repeat++) {
        DList a;
        auto start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) a.dl_insert_head(keys[i]);
        auto finish = chrono::steady_clock::now();
        insert_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
        checksum += a.dl_items().size();

        start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) checksum += a.dl_find(keys[i]) != nullptr;
        finish = chrono::steady_clock::now();
        search_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
    }
    show_result("doubly_linked_list", "push_front", n, insert_time);
    show_result("doubly_linked_list", "find_by_value", n, search_time);
}

void bench_stack(const vector<string>& keys) {
    int n = (int)keys.size();
    long long insert_time = 0;
    for (int repeat = 0; repeat < REPEATS; repeat++) {
        Stack a;
        auto start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) a.st_push(keys[i]);
        auto finish = chrono::steady_clock::now();
        insert_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
        checksum += a.st_items().size();
    }
    show_result("stack", "push", n, insert_time);
}

void bench_queue(const vector<string>& keys) {
    int n = (int)keys.size();
    long long insert_time = 0;
    for (int repeat = 0; repeat < REPEATS; repeat++) {
        Queue a;
        auto start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) a.q_push(keys[i]);
        auto finish = chrono::steady_clock::now();
        insert_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
        checksum += a.q_items().size();
    }
    show_result("queue", "push", n, insert_time);
}

void bench_chain_table(const vector<string>& keys) {
    int n = (int)keys.size();
    long long insert_time = 0;
    long long search_time = 0;
    for (int repeat = 0; repeat < REPEATS; repeat++) {
        ChainTable a;
        auto start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) a.ct_put(keys[i], "v");
        auto finish = chrono::steady_clock::now();
        insert_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
        checksum += a.ct_getsz();

        start = chrono::steady_clock::now();
        string value;
        for (int i = 0; i < n; i++) checksum += a.ct_get(keys[i], value);
        finish = chrono::steady_clock::now();
        search_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
    }
    show_result("chain_table", "put", n, insert_time);
    show_result("chain_table", "get", n, search_time);
}

void bench_cuckoo_table(const vector<string>& keys) {
    int n = (int)keys.size();
    long long insert_time = 0;
    long long search_time = 0;
    for (int repeat = 0; repeat < REPEATS; repeat++) {
        CuckooTable a;
        auto start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) a.cu_put(keys[i], "v");
        auto finish = chrono::steady_clock::now();
        insert_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
        checksum += a.cu_getsz();

        start = chrono::steady_clock::now();
        string value;
        for (int i = 0; i < n; i++) checksum += a.cu_get(keys[i], value);
        finish = chrono::steady_clock::now();
        search_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
    }
    show_result("cuckoo_table", "put", n, insert_time);
    show_result("cuckoo_table", "get", n, search_time);
}

void bench_avl_tree(const vector<string>& keys) {
    int n = (int)keys.size();
    long long insert_time = 0;
    long long search_time = 0;
    for (int repeat = 0; repeat < REPEATS; repeat++) {
        AVLTree a;
        auto start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) a.t_insert(i);
        auto finish = chrono::steady_clock::now();
        insert_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
        checksum += a.t_items().size();

        start = chrono::steady_clock::now();
        for (int i = 0; i < n; i++) checksum += a.t_search(i);
        finish = chrono::steady_clock::now();
        search_time += chrono::duration_cast<chrono::nanoseconds>(finish - start).count();
    }
    show_result("avl_tree", "insert", n, insert_time);
    show_result("avl_tree", "search", n, search_time);
}

int main(int argc, char* argv[]) {
    int n = 2000;
    if (argc > 1) {
        try {
            n = stoi(argv[1]);
        } catch (...) {
            n = 0;
        }
    }
    if (n < 1 || n > 100000) {
        cerr << "Usage: ./benchmark_cpp [N between 1 and 100000]\n";
        return 1;
    }

    vector<string> keys;
    for (int i = 0; i < n; i++) keys.push_back("key_" + to_string(i));

    cout << "structure,operation,elements,mean_ns_per_operation\n";
    bench_array(keys);
    bench_singly_linked_list(keys);
    bench_doubly_linked_list(keys);
    bench_stack(keys);
    bench_queue(keys);
    bench_chain_table(keys);
    bench_cuckoo_table(keys);
    bench_avl_tree(keys);

    cerr << "Checksum: " << checksum << "\n";
    return 0;
}
