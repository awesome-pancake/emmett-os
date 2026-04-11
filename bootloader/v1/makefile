all: main.efi

main.o: main.c
	gcc -I/gnu-efi/inc -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c main.c -o main.o

main.so: main.o
	ld -shared -Bsymbolic -L/gnu-efi/x86_64/lib -L/gnu-efi/x86_64/gnuefi -T/gnu-efi/gnuefi/elf_x86_64_efi.lds /gnu-efi/x86_64/gnuefi/crt0-efi-x86_64.o main.o -o main.so -lgnuefi -lefi

main.efi: main.so
	objcopy -j .text -j .sdata -j .data -j .rodata -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --output-target efi-app-x86_64 --subsystem=10 main.so main.efi