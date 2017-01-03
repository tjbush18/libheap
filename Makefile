objects = heap.o memoryblock.o

CXX = i686-elf-g++
CC = i686-elf-g++
AR = i686-elf-ar
CPPFLAGS += -Iinclude/ -O0 -ffreestanding -Wall -Wextra -fno-exceptions -fno-rtti -fpermissive -c

static: $(addprefix src/, $(objects))
	mkdir -p bin
	$(AR) rcs bin/libheap.a $(addprefix src/, $(objects))
