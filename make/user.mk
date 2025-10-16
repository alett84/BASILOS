.POSIX:

user/core.o: user/core.c include/string.h include/mpx/serial.h \
  include/mpx/device.h include/processes.h include/sys_req.h

user/user_pcb.o: user/user_pcb.c include/mpx/user_pcb.h \
include/stdlib.h include/string.h include/mpx/kernel_pcb.h

user/mpxio.o: user/mpxio.c include/mpx/mpxio.h \
   include/sys_req.h include/string.h include/memory.h \
   include/stdlib.h  include/mpx/kernel_pcb.h

user/comhand.o: user/comhand.c include/mpx/comhand.h \
   include/mpx/device.h include/sys_req.h include/mpx/mpxio.h \
   include/mpx/cmd_alarm.h include/mpx/cmd_mcb.h include/mpx/kernel_pcb.h
   
user/cmd_datetime.o: user/cmd_datetime.c include/mpx/cmd_datetime.h \
   include/stdlib.h include/string.h include/mpx/mpxio.h \
   include/rtc.h

user/cmd_pcb.o: user/cmd_pcb.c include/mpx/cmd_pcb.h \
   include/stdlib.h include/string.h include/mpx/mpxio.h \
   include/mpx/user_pcb.h include/processes.h include/mpx/kernel_pcb.h

user/cmd_alarm.o: user/cmd_alarm.c include/mpx/cmd_alarm.h \
  include/mpx/mpxio.h include/mpx/io.h include/mpx/isr.h \
  include/mpx/user_pcb.h include/mpx/kernel_pcb.h include/processes.h \
  include/rtc.h include/string.h include/stdlib.h include/mpx/kernel_pcb.h

user/cmd_mcb.o: user/cmd_mcb.c include/mpx/cmd_mcb.h \
include/mpx/vm.h include/mpx/mpxio.h include/string.h\
include/mcb.h include/mpx/kernel_pcb.h\

USER_OBJECTS=\
	user/core.o \
   user/comhand.o \
   user/mpxio.o \
   user/cmd_datetime.o \
   user/cmd_pcb.o \
   user/user_pcb.o\
   user/cmd_alarm.o\
   user/cmd_mcb.o