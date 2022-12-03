CXX=g++
CPPFLAGS=-g -O3 
INCLUDES=-Itbb/latest/include
LDLIBS=-Ltbb/latest/lib/intel64/gcc4.8/ -ltbb
TARGET=run_games

SRCS=main.cpp Player.cpp Game.cpp
OBJS=$(subst .cpp,.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) 
	$(CXX) $(CPPFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDLIBS) $(LDFLAGS)

%.o: %.c
	$(CXX) $(CPPFLAGS) $(INCLUDES) -c $(input) -o $(output)
	
clean:
	rm $(OBJS) $(TARGET)


