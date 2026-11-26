CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
SRC = main.cpp asteroids.cpp set.cpp array.cpp zmeika.cpp hashtable.cpp lfu.cpp ulist.cpp

all:
	$(CXX) $(CXXFLAGS) $(SRC) -o lab2

check:
	$(CXX) -std=c++17 -g -Wall -Wextra -fsanitize=address,undefined $(SRC) -o lab2_asan

clean:
	rm -f lab2 lab2_asan
