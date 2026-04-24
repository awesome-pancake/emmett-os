# Capstone Project Description

Through this project I hoped to learn how to make programs that were capable of booting and running on a computer without an operating system. To accomplish this, I set a goal to write a "microkernel" that manages several aspects that an operating system would. This will likely not be a fully-featured operating system due to time constaints, but instead a small-scale bootable program that does similar things to an operating system. Here the name "Emmett OS" is more of a tongue-in-cheek joke than an accurate description of the project.

## Needs to Have
- Bootable on a computer without an operating system without breaking
- Management of various low-level aspects of a system:
    - System interrupts
    - Global descriptor table
    - Paging and memory virtualisation
- User I/O (keyboard, screen)
- Methods for string output
- Memory management functions
    - Macro memory management (virtual memory and paging)
    - Heap management

## Nice to Have
- Can parse filesystems, read files into memory
- UNIX-like device I/O
- A full shell system with commands
- Able to execute files (single task or multitask)
- System calls
- Implementation of some libc headers