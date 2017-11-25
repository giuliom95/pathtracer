# options
CXX=g++
CXXFLAGS=-W -Wall -std=c++11 -O3
TARGET=bin/raytracer

USD_ROOT=$(HOME)/dev/packages/USD/install
PTEX_ROOT=$(HOME)/dev/packages/ptex/install
INCLUDES=-I/usr/include/OpenEXR -I$(USD_ROOT)/include -I$(PTEX_ROOT)/include

LDFLAGS=-lIlmImf -lHalf -L$(PTEX_ROOT)/lib64 -lPtex

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
