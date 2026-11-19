#!/usr/bin/env sh
set -eu
cd "$(dirname "$0")/.."
if ! command -v gcovr >/dev/null 2>&1; then
  echo 'Для HTML-покрытия установите gcovr (например, sudo apt install gcovr).' >&2
  exit 1
fi

cmake -S . -B build-coverage -DENABLE_COVERAGE=ON -DBUILD_UNIT_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-coverage --parallel
find build-coverage -type f -name '*.gcda' -delete
ctest --test-dir build-coverage --output-on-failure
mkdir -p build-coverage/coverage

gcovr --root . --object-directory build-coverage --filter '(.*/)?(array|avltree|dlist|hashtable|queue|stack|ulist)\.cpp$' --html-details build-coverage/coverage/index.html --print-summary --fail-under-line 85
printf 'HTML-отчёт: lab3/build-coverage/coverage/index.html\n'