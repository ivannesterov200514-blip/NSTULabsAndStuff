#!/usr/bin/env sh
set -eu
cd "$(dirname "$0")/.."
if ! command -v gcovr >/dev/null 2>&1; then
  echo 'Для HTML-покрытия установите gcovr (python -m pip install gcovr).' >&2
  exit 1
fi
# Используем новую папку, чтобы данные предыдущего прогона не влияли на счётчики.
cmake -S . -B build-coverage -DENABLE_COVERAGE=ON -DBUILD_UNIT_TESTS=ON -DCMAKE_BUILD_TYPE=Debug
cmake --build build-coverage --parallel
ctest --test-dir build-coverage --output-on-failure
mkdir -p build-coverage/coverage
# ЛР: проверяем модуль структур данных; отчёт не включает CLI и файлы тестов.
gcovr --root . --object-directory build-coverage --filter '.*/(array|avltree|dlist|hashtable|queue|stack|ulist)\.cpp$' --html-details build-coverage/coverage/index.html --print-summary --fail-under-line 85
printf 'HTML-отчёт: lab3/build-coverage/coverage/index.html\n'
