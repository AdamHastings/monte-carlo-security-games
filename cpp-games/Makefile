CXX=g++
CPPFLAGS=-g -O3
LDFLAGS=-g
LDLIBS=

SRCS=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: game

game: $(OBJS)
	$(CXX) $(LDFLAGS) -o game $(OBJS) $(LDLIBS)

game.o: main.cpp

clean:
	rm $(OBJS)


