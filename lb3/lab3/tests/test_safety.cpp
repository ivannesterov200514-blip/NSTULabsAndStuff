#include "array.h"
#include "dlist.h"
#include "stack.h"
#include "queue.h"
#include "avltree.h"
#include "hashtable.h"
#include <stdexcept>
#include <type_traits>

// Самодостаточные тесты для проверки критических ошибок памяти.
// Не заменяют Google Test / Boost.Test / Catch2.
int main() {
    static_assert(!std::is_copy_constructible<Arr>::value, "Arr must not be shallow-copied");
    static_assert(!std::is_copy_constructible<DList>::value, "DList must not be shallow-copied");
    static_assert(!std::is_copy_constructible<Stack>::value, "Stack must not be shallow-copied");
    static_assert(!std::is_copy_constructible<Queue>::value, "Queue must not be shallow-copied");
    static_assert(!std::is_copy_constructible<AVLTree>::value, "AVLTree must not be shallow-copied");
    static_assert(!std::is_copy_constructible<ChainTable>::value, "ChainTable must not be shallow-copied");
    static_assert(!std::is_copy_constructible<CuckooTable>::value, "CuckooTable must not be shallow-copied");

    Arr arr;
    arr.arr_insert_tail("alpha");
    arr.arr_insert_tail("beta");
    for (int bad : {0, 1, -1}) {
        bool threw = false;
        try { arr.arr_resize(bad); }
        catch (const std::invalid_argument&) { threw = true; }
        if (!threw) return 1;
    }
    if (arr.arr_getsz() != 2) return 2;
    if (arr.arr_search(0) != "alpha") return 3;
    if (arr.arr_search(1) != "beta") return 4;
    arr.arr_resize(5);
    arr.arr_insert_tail("gamma");
    if (arr.arr_getsz() != 3) return 5;
    return 0;
}
