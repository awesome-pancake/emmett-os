kernel.bin: kernel.c
	gcc -Wl,--oformat=binary -c kernel.c
	objcopy -O binary -j .text kernel.o kernel.bin