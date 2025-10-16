.POSIX:

kernel/serial.o: include/mpx/serial.h kernel/serial.c include/mpx/io.h \
  include/mpx/device.h include/sys_req.h include/mpx/dcb.h

kernel/kmain.o: kernel/kmain.c include/mpx/gdt.h include/mpx/interrupts.h \
  include/mpx/serial.h include/mpx/device.h include/mpx/vm.h \
  include/sys_req.h include/string.h include/memory.h \
  include/mpx/comhand.h include/mpx/kernel_pcb.h

kernel/rtc.o: kernel/rtc.c include/mpx/io.h

kernel/core-c.o: kernel/core-c.c include/mpx/gdt.h include/mpx/panic.h \
  include/mpx/interrupts.h include/mpx/io.h include/mpx/serial.h \
  include/mpx/device.h include/sys_req.h include/string.h \
  include/mpx/vm.h

kernel/kernel_pcb.o: kernel/kernel_pcb.c include/mpx/kernel_pcb.h \
include/stdlib.h include/memory.h include/string.h

kernel/isr.o: kernel/isr.c include/mpx/isr.h include/mpx/dcb.h \
include/stdlib.h include/string.h include/mpx/kernel_pcb.h include/sys_req.h

kernel/dcb.o: kernel/dcb.c include/mpx/dcb.h

KERNEL_OBJECTS=\
	kernel/core-asm.o\
	kernel/sys_call_isr.o\
	kernel/serial.o\
  kernel/serial_isr.o\
	kernel/kmain.o\
	kernel/core-c.o\
  kernel/rtc.o\
  kernel/kernel_pcb.o\
  kernel/isr.o\
  kernel/dcb.o
