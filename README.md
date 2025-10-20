**BasilOS 6.0**
# Educational Microkernel Operating System
BasilOS is a custom, command-line operating system developed as part of my operating systems structure course. 
It demonstrates practical knowledge of process control, memory management, real-time clock integration, and serial I/O — written in C and organized into modular subsystems.
#**Project Overview**
BasilOS was built from the ground up to simulate core operating system behavior within a controlled, educational environment.
**It supports:**
- Creation and management of user and system processes
- Dynamic heap allocation using Memory Control Blocks (MCBs)
- Time and date management through a Real-Time Clock (RTC)
- Serial communication and interrupt handling
- A shell environment for direct user interaction
The goal was to understand and implement how real operating systems schedule processes, allocate memory, and interact with hardware at a low level.

#**Key Features**
**Command-Line Shell**
An interactive terminal where users can issue commands such as:
- help, clear, shutdown, and version
- date get/set and time get/set
- pcb commands to create, delete, suspend, resume, and inspect processes
- mcb commands to allocate or free memory
- alarm commands to set timed process messages
Each command is parsed, validated, and executed through the command handler (comhand) process.

**Process Management**
Implements a full Process Control Block (PCB) system supporting ready, running, and blocked states.
Each process stores:
- Name, class type, suspension state, and priority (0–9)
- Execution stack and stack pointer
- Links to the next and previous PCBs for scheduling
Processes can be suspended, resumed, deleted, or prioritized dynamically at runtime.
Queue operations maintain correct scheduling order and CPU hand-off behavior.

**Memory Management**
Our custom heap allocator uses Memory Control Blocks (MCBs) to handle memory allocation and release.
Features include:
- First-fit allocation algorithm
- Automatic block merging to reduce fragmentation
- Console commands to display all, free, or allocated memory blocks
This system closely models how a real OS manages dynamic memory.

**Real-Time Clock and Alarms**
Integrated support for hardware-level RTC interaction:
- date get/set and time get/set commands read/write RTC registers
- alarm command schedules time-based process creation with custom messages
Each alarm runs as a separate process, showing how timed events can be linked to the scheduler.

**Serial I/O and Interrupts**
Supports UART-based serial communication and interrupt-driven input/output.
 Includes:
- Initialization and configuration of COM ports
- Interrupt Service Routines (ISRs) for serial input/output
- Error handling for invalid devices and busy states
This module demonstrates how user input and device data are managed asynchronously.

#**Development Details**
Language: C
Environment: Visual Studio / GCC toolchain
Target Architecture: x86
Operating Systems Tested: Linux Mint and Windows (via serial emulation)


#**My Role and Contributions**
- Designed and implemented command handler logic (comhand)
- Integrated the real-time clock and time/date command modules
- Wrote and formatted the User’s Manual and Programmer’s Manual
- Helped debug PCB and MCB subsystems to ensure stable process scheduling
- Worked collaboratively with a small team to merge features into a single bootable OS build


#**Learning Outcomes**
Developing BasilOS provided hands-on experience with:
- Low-level C programming and memory management
- Process scheduling and context switching
- Device communication through interrupts
- System-level debugging and modular code organization
The project strengthened my understanding of how modern operating systems work at the kernel level.

#**Documentation**
User’s Manual (v6.0): Command reference and examples
Programmer’s Manual (v6.0): Full list of functions, data structures, and algorithms


How to Run
# Clone the repository
git clone https://github.com/<your-username>/BasilOS.git
cd BasilOS

# Compile
make

# Launch
./BasilOS

Use help in the terminal to view available commands.
Summary
BasilOS 6.0 is a compact, functional operating system built entirely in C to demonstrate foundational OS concepts.
It combines process scheduling, memory management, and hardware-level I/O into a cohesive learning project that reflects real-world system design principles.
