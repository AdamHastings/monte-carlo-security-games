
CXX=g++
CPPFLAGS=-g -O3 -I../tbb/latest/include
LDFLAGS=-g
LDLIBS=-L../tbb/latest/lib/intel64/gcc4.8/ -ltbb

SRCS=test.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: test

test: $(OBJS)
	$(CXX) $(LDFLAGS) -o test $(OBJS) $(LDLIBS)

test.o: test.cpp

clean:
	rm $(OBJS)

#  g++ -o test test.cpp -I../tbb/latest/include -L../tbb/latest/lib/intel64/gcc4.8/ -ltbb