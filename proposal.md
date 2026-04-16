# Capstone Project Descriptions

Through this project I hoped to learn how to make programs that were capable of booting and running on a computer without an operating system. To accomplish this, I set a goal to write a "microkernel" that manages several aspects that an operating system would. This will likely not be a fully-featured operating system, but instead a kernel.

## Needs to Have
- Bootable on a computer without an operating system without breaking
- Management of various low-level aspects of a system, such as interrupts
- User I/O (keyboard, screen)
- Methods for string output
- Memory management functions
- Effective abstraction of some low-level details

## Nice to Have
- Can parse filesystems, read files into memory
- UNIX-like device I/O
- A full shell system with commands
- Able to execute files (single task or multitask)
- System calls
- Implementation of some libc headers