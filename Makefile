# options
CXX=g++
CXXFLAGS=-std=c++11 -O3 -Wall
TARGET=bin/pathtracer

INCLUDES=-I/usr/include/OpenEXR

LDFLAGS=-lIlmImf -lHalf -pthread

# globs
SRCS := $(wildcard src/*.cpp)
HDRS := $(wildcard src/*.hpp)
OBJS := $(patsubst src/%.cpp,bin/%.o,$(SRCS))

# link it all together
$(TARGET): $(OBJS) $(HDRS)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(OBJS) -o $(TARGET)

# compile an object based on source and headers
bin/%.o: src/%.cpp $(HDRS)
	@mkdir -p bin
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# tidy up
clean:
	rm -f $(TARGET) $(OBJS)

all: bin/pathtracer
	@echo "All done!"