#!/usr/bin/env sh
set -eu
cd "$(dirname "$0")/.."
if ! command -v clang-tidy >/dev/null 2>&1; then
    echo 'Для линтера нужен clang-tidy. Установите LLVM/clang-tidy и повторите make lint.' >&2
    exit 1
fi
for source in array.cpp avltree.cpp dlist.cpp hashtable.cpp queue.cpp stack.cpp ulist.cpp binary_storage.cpp main.cpp bench/benchmark.cpp; do
    echo "Проверяем $source"
    clang-tidy "$source" -- -std=c++17 -I.
done
