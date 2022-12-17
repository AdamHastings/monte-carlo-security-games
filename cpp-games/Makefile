CXX=g++
CPPFLAGS= -Wall -Werror -std=c++11 -g3 -fno-builtin-memset -fno-lifetime-dse -O3 -march=native -Ofast 
INCLUDES=-Itbb/latest/include -Ijson
LDLIBS=-Ltbb/latest/lib/intel64/gcc4.8/ 
LDFLAGS= -ltbb -lstdc++fs 
TARGET=run_games

SRCS=main.cpp Player.cpp Game.cpp
OBJS=$(subst .cpp,.o,$(SRCS)) jsoncpp.o

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CXX) $(CPPFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDLIBS) $(LDFLAGS)

%.o: %.c %.h
	$(CXX) $(CPPFLAGS) $(INCLUDES) -c $(input) -o $(output) $(LDLIBS) $(LDFLAGS)
	
clean:
	rm $(OBJS) $(TARGET)


