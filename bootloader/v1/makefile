all: bootx64.efi

bootx64.o: bootx64.c
	gcc -I/gnu-efi/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c bootx64.c -o bootx64.o

bootx64.so: bootx64.o
	ld -shared -Bsymbolic -L/gnu-efi/x86_64/lib -L/gnu-efi/x86_64/gnuefi -T/gnu-efi/gnuefi/elf_x86_64_efi.lds /gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o bootx64.o -o bootx64.so -lgnuefi -lefi

bootx64.efi: bootx64.so
	objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --output-target efi-app-x86_64 --subsystem=10 bootx64.so bootx64.efi