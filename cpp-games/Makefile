CXX=g++
CPPFLAGS=-g -O3 -Itbb/latest/include
LDFLAGS=-g
LDLIBS=-Ltbb/latest/lib/intel64/gcc4.8/ -ltbb

SRCS=main.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: game

game: $(OBJS)
	$(CXX) $(LDFLAGS) -o game $(OBJS) $(LDLIBS)

game.o: main.cpp

clean:
	rm $(OBJS)


