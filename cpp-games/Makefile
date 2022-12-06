CXX=g++
CPPFLAGS=-g3 -O3  -std=c++17
INCLUDES=-Itbb/latest/include
LDLIBS=-Ltbb/latest/lib/intel64/gcc4.8/ -ltbb
TARGET=run_games

SRCS=main.cpp Player.cpp Game.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CXX) $(CPPFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDLIBS) $(LDFLAGS) -lstdc++fs

%.o: %.c %.h
	$(CXX) $(CPPFLAGS) $(INCLUDES) -c $(input) -o $(output) -lstdc++fs
	
clean:
	rm $(OBJS) $(TARGET)


