CC=g++
CXXFLAGS=-std=c++11 -Wall
SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

all: $(OBJS)

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c $< -o $@
	@echo "CC <= $<"

clean:
	$(RM) $(OBJS)
