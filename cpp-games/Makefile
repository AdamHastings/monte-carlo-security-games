CXX=g++
CPPFLAGS=-Wall -Werror -std=c++17 -fno-builtin-memset
INCLUDES=
LDLIBS=
LDFLAGS= 
TARGET=run_games

# Debug compiler flags
DEBUG_FLAGS = -O0 -DDEBUG -g3 -ggdb3 $(CPPFLAGS)

# Release compiler flags
RELEASE_FLAGS = -O3 -march=native -Ofast -DRELEASE $(CPPFLAGS)

# Source files
SOURCES = $(wildcard src/*.cpp)

# Object files directories
OBJDIR_DEBUG = obj/debug
OBJDIR_RELEASE = obj/release

# Object files
OBJECTS_DEBUG = $(SOURCES:src/%.cpp=$(OBJDIR_DEBUG)/%.o)
OBJECTS_RELEASE = $(SOURCES:src/%.cpp=$(OBJDIR_RELEASE)/%.o)

# Executable names
BINDIR_DEBUG = run/debug
BINDIR_RELEASE = run/release
EXECUTABLE_DEBUG = $(BINDIR_DEBUG)/$(TARGET)_debug
EXECUTABLE_RELEASE = $(BINDIR_RELEASE)/$(TARGET)_release

# Build rules for debug and release versions
debug: $(EXECUTABLE_DEBUG)
release: $(EXECUTABLE_RELEASE)

all: debug release

$(EXECUTABLE_DEBUG): $(OBJECTS_DEBUG) | $(BINDIR_DEBUG)
	$(CXX) $(DEBUG_FLAGS) $^ -o $@

$(EXECUTABLE_RELEASE): $(OBJECTS_RELEASE) | $(BINDIR_RELEASE)
	$(CXX) $(RELEASE_FLAGS) $^ -o $@

$(OBJDIR_DEBUG)/%.o: src/%.cpp | $(OBJDIR_DEBUG)
	$(CXX) $(DEBUG_FLAGS) -c $< -o $@

$(OBJDIR_RELEASE)/%.o: src/%.cpp | $(OBJDIR_RELEASE)
	$(CXX) $(RELEASE_FLAGS) -c $< -o $@

$(OBJDIR_DEBUG) $(BINDIR_DEBUG) $(OBJDIR_RELEASE) $(BINDIR_RELEASE):
	mkdir -p $@

clean:
	rm -rf obj run

.PHONY: debug release clean
