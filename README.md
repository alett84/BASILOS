**MPX Operating System Kernel (x86) **
A modular. educational 32-bit x86 microkernel built from scratch as a project for my _Operating Systems Structure_ course. 
This project demonstrates protected-mode kernel design, interupt handling, memory managment, process scheduling, and serial I/O, all implemented in C and Assembly. 

**Overview**
The MPX kernel runs in protected mode and provides a foundation for multitasking and user interaction through a command-line interface(CLI). Each major subsystem including interrupts, process control, memory managment, and I/O is modular and interconnected via system calls. 

**Key Subsystems**
- Boot and Core Setup: Initializes the GDT, IDT, PIC and paging.
- Heap Managment: Implements a first-fit allocator using Memory Control Blocks(MCB's).
- Process Control: Supports PCB creation, ready/blocked queues, and context switching.
- System Calls: Managed via software interrupt(int 0x60) handlers for READ, WRITE, LOCK, UNLOCK, and process control.
- Serial I/O Driver: Full UART communication using interrupt-driven ring buffers.
- CLI(comhand): Interactive shell with commands for PCBs, memory, RTC and alarms. 
