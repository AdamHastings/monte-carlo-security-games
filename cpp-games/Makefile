CXX=g++
CPPFLAGS=-Wall -Werror -std=c++17 -fno-builtin-memset
INCLUDES=-Itbb/latest/include -Ijson
LDLIBS=-Ltbb/latest/lib/intel64/gcc4.8/ 
LDFLAGS=-ltbb
TARGET=run_games

SRCS=main.cpp Player.cpp Insurer.cpp Attacker.cpp Defender.cpp Game.cpp Distributions.cpp 
OBJS=$(subst .cpp,.o,$(SRCS)) jsoncpp.o

$(all): $(TARGET)

release: CPPFLAGS+=-O3 -march=native -Ofast
release: $(TARGET)

debug: CPPFLAGS+=-DDEBUG -g3
debug: $(TARGET)

$(TARGET): $(OBJS) Makefile
	$(CXX) $(CPPFLAGS) $(INCLUDES) -o $(TARGET) $(OBJS) $(LDLIBS) $(LDFLAGS)

%.o: %.c %.h Makefile
	$(CXX) $(CPPFLAGS) $(INCLUDES) -c $(input) -o $(output) $(LDLIBS) $(LDFLAGS)

clean:
	rm $(OBJS) $(TARGET)

