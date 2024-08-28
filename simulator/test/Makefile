# Compiler
CXX = g++

LDLIBS=-lgtest -lgtest_main -pthread -lgsl -lgslcblas -lm

# Compiler flags
CXXFLAGS = -Wall -std=c++17 -g3 -ggdb3 

# Name of your executable
TARGET = test

# Source files
SOURCES += test.cpp


# Object files
OBJECTS = $(SOURCES:.cpp=.o)
EXT_OBJECTS = ../obj/debug/Player.o ../obj/debug/Defender.o ../obj/debug/utils.o ../obj/debug/Attacker.o ../obj/debug/Game.o ../obj/debug/Insurer.o ../obj/debug/params.o ../obj/debug/Distributions.o ../obj/debug/jsoncpp.o

# Default target
all: $(TARGET)

# Rule to create the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) $(EXT_OBJECTS) -o $@ $(LDLIBS)

# Rule to compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean