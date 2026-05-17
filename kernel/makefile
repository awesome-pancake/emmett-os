kernel.elf:

interrupt.o: src/interrupt.s
	nasm -f elf64 src/interrupt.s -o interrupt.o

memory.o: src/memory.s
	nasm -f elf64 src/memory.s -o memory.o

kernel.elf: src/*.c memory.o interrupt.o
	gcc -Wall src/*.c *.o -I inc -nostartfiles --entry kernel_main -o kernel.elf