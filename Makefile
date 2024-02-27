SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp, build/%.o, $(SRCS))

CC = clang++ -stdlib=libc++ -std=c++14
CFLAGS = -g -O3 -I /usr/lib/llvm-14/include -I /usr/lib/llvm-14/build/include -I src/include
# LLVMFLAGS = `llvm-config --cxxflags --ldflags --system-libs --libs all`
LLVMFLAGS = -L /usr/lib/llvm-14/build/lib -lLLVM-14

all: kaleidoscope

kaleidoscope: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

build/%.o: src/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build/* kaleidoscope