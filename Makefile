CXX      := clang++
CXXFLAGS := -std=c++11 -fsanitize=address -Wall -Wextra -g

SRCS     := $(wildcard *.cpp)
OBJS     := $(SRCS:.cpp=.o)

all: build
	./main

build: $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o main

clean:
	rm -f main $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@