# Compiler
CXX = g++

LDLIBS=-lgtest -lgtest_main -pthread

# Compiler flags
CXXFLAGS = -Wall -std=c++17

# Name of your executable
TARGET = test

# Source files
SOURCES = test.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Default target
all: $(TARGET)

# Rule to create the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDLIBS)

# Rule to compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

# Clean up
clean:
	rm -f $(OBJECTS) $(TARGET)

# Phony targets
.PHONY: all clean