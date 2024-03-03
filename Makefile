SRCS := $(wildcard src/*.cpp)
OBJS := $(patsubst src/%.cpp, build/%.o, $(SRCS))

CC = clang++ -stdlib=libc++ -std=c++17 -fsanitize=address
CFLAGS = -g -O0 -I /usr/include/llvm-14/llvm -I src/include -rdynamic
LLVMFLAGS = `llvm-config --cxxflags --ldflags --system-libs --libs core orcjit native`
# -I /usr/lib/llvm-14/include -I /usr/lib/llvm-14/build/include 
# -L /usr/lib/llvm-14/build/lib -lLLVM-14

all: kaleidoscope

kaleidoscope: $(OBJS)
	$(CC) $(CFLAGS) $(LLVMFLAGS) -o $@ $^

build/%.o: src/%.cpp
	$(CC) $(CFLAGS) $(LLVMFLAGS)  -c $< -o $@

clean:
	rm -rf build/* kaleidoscope