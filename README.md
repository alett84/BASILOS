**MPX Operating System Kernel (x86)**
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

**Features**
**Kernel Core** 
-Protected-mode initialization (core-c.c, core-asm.s) sets up GDT, IDT, PIC and virtual memory paging. 
-Panic and diagnostic logging via kpanic() and serial output. 

**Memory Managment**
-Custom heap allocator implemented via Memory Control Blocks (MCBs). 
  -Functions: initialize_heap(), allocate_memory(), free_memory(), and mcb_split(). 
-System memory wrappers (sys_alloc_mem, sys_free_mem) and user-replaceable heap functions. 

**Process Control**
-Full Process Control Block (PCB) subsystem: allocation, insertion, priority, and queue managment. 
-Scheduler supports ready, blocked, and running queues 
-Context switching implemented through sys_call() interrupt handler. 

**Device and I/O**
-Serial Communication: UART driver for COM1-COM4 with interrupt-driven I/O. 
-Device Control Blocks (DCB's): Manage async reads/writes and locking for devices. 
-RTC Driver: Direct CMOS access for date/time control with leap year support. 

**Command Line Interface**
-Command handler (comhand) provides modular commands: 
  -cmd


