CXX			 = clang++

CXXFLAGS ?= -fsanitize=address -Wall -Wextra -g
CXXFLAGS += -std=c++20
CXXFLAGS += `pkg-config --cflags ncurses`

LDFLAGS  += `pkg-config --libs ncurses`

TARGET = iris
SRC		 = main.cpp

all: $(TARGET)
clean:
	rm -f $(TARGET)

$(TARGET): $(SRC)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LDFLAGS)

.PHONY: all clean
