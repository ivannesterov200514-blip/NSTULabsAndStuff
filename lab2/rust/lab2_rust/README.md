# Lab 2 — Rust

Перенос содержимого `lab2/cpp` из репозитория `NSTULabsAndStuff` на Rust.

## Сборка

Требуется Rust stable (edition 2021). Внешние crates не используются.

```bash
cargo build --release
```

Проверка компиляции:

```bash
cargo check
```

Запуск имеет тот же формат аргументов, что и исходная программа:

```bash
cargo run --release -- --file file.data --query "SETCREATE myset"
cargo run --release -- --file file.data --query "SETADD myset alpha"
cargo run --release -- --file file.data --query "PRINT myset"
```

Каждый запуск выполняет одну команду. Изменяющие команды сохраняют контейнеры обратно в указанный файл. Формат строк базы данных оставлен совместимым с исходной C++-версией (`H`, `T`, `C`, `K`, `U`). Как и в исходном проекте, имена, ключи и значения должны быть отдельными токенами без пробелов.

## Поддерживаемые команды

| Команда | Назначение |
|---|---|
| `PRINT name` | Вывести контейнер |
| `SETCREATE name` | Создать множество |
| `SETADD name value` | Добавить элемент во множество |
| `SETDEL name value` | Удалить элемент из множества |
| `SET_AT name value` | Проверить наличие элемента |
| `TCREATE name` | Создать AVL-дерево |
| `TINSERT name integer` | Вставить ключ в AVL-дерево |
| `TSNAKE name` | Обход дерева «змейкой» |
| `HCREATE name CHAIN\|CUCKOO` | Создать хеш-таблицу выбранного типа |
| `HSET name key value` | Добавить или заменить пару ключ-значение |
| `HGET name key` | Получить значение по ключу |
| `HDEL name key` | Удалить ключ |
| `GENEALOGY N child parent ...` | Построить генеалогическое дерево и вывести число потомков |
| `LFUCREATE name capacity` | Создать LFU-кэш |
| `LFUSET name key value` | Добавить значение в кэш |
| `LFUGET name key` | Получить значение с увеличением частоты использования |
| `ASTEROIDS massR massL ...` | Рассчитать столкновения астероидов |
| `TURTLES N a1 b1 ... aN bN` | Решить задачу о черепахах |
| `GISTOGRAM text...` | Вывести ASCII-гистограмму первых байтов символов |

## Перенос файлов

| Исходный файл | Rust-файл / замена |
|---|---|
| `array.cpp`, `array.h` | `src/array.rs` |
| `asteroids.cpp`, `asteroids.h` | `src/asteroids.rs` |
| `hashtable.cpp`, `hashtable.h` | `src/hashtable.rs` |
| `lfu.cpp`, `lfu.h` | `src/lfu.rs` |
| `set.cpp`, `set.h` | `src/set.rs` |
| `ulist.cpp`, `ulist.h` | `src/ulist.rs` |
| `zmeika.cpp`, `zmeika.h` | `src/zmeika.rs` |
| `main.cpp` | `src/main.rs` |
| `Makefile` | `Cargo.toml` и корневой `Makefile` |

Заголовочные `.h`-файлы объединены с реализациями в соответствующие Rust-модули: в Rust для этого не нужны отдельные заголовки.

## Примечание об исходном снимке

На указанном коммите исходный `Makefile` упоминает отсутствующий `avltree.cpp`, а `main.cpp` включает отсутствующие `t1.h` и `t3.h`. AVL-дерево фактически реализовано в `zmeika.cpp`. В Rust-проекте зависимости между реализованными структурами подключены напрямую через модули; для сборки не требуются отсутствующие файлы.
