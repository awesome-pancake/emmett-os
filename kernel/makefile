kernel.bin: kernel.c
	gcc kernel.c inc/list.c inc/console.c inc/memory.c -I inc -nostartfiles --entry kernel_main -o kernel.elf