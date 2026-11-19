CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -Wpedantic
SRC = main.cpp array.cpp ulist.cpp dlist.cpp stack.cpp queue.cpp avltree.cpp hashtable.cpp binary_storage.cpp
LIBSRC = array.cpp ulist.cpp dlist.cpp stack.cpp queue.cpp avltree.cpp hashtable.cpp

.PHONY: all clean benchmark lint test-cli test coverage
all: dbms

dbms: $(SRC) $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -o $@ $(SRC)

benchmark_cpp: bench/benchmark.cpp $(LIBSRC) $(wildcard *.h)
	$(CXX) $(CXXFLAGS) -O2 -I. -o $@ bench/benchmark.cpp $(LIBSRC)

benchmark: benchmark_cpp
	./benchmark_cpp 2000

test-cli: dbms
	python3 tests/test_cli.py ./dbms

test:
	cmake -S . -B build -DBUILD_UNIT_TESTS=ON
	cmake --build build --parallel
	ctest --test-dir build --output-on-failure

lint:
	sh scripts/lint_cpp.sh

coverage:
	sh scripts/coverage.sh

clean:
	rm -f dbms benchmark_cpp
