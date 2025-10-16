#include <mpx/gdt.h>
#include <mpx/interrupts.h>
#include <mpx/serial.h>
#include <mpx/vm.h>
#include <sys_req.h>
#include <string.h>

#include <memory.h>
#include <processes.h> 
#include <mpx/kernel_pcb.h>
#include <mpx/isr.h>
#include <mcb.h>

#include <mpx/comhand.h>

static void klogv(device dev, const char *msg)
{
	char prefix[] = "klogv: ";
	serial_out(dev, prefix, strlen(prefix));
	serial_out(dev, msg, strlen(msg));
	serial_out(dev, "\r\n", 2);
}

void kmain(void)
{
	// 0) Serial I/O -- <mpx/serial.h>
	// If we don't initialize the serial port, we have no way of
	// performing I/O. So we need to do that before anything else so we
	// can at least get some output on the screen.
	// Note that here, you should call the function *before* the output
	// via klogv(), or the message won't print. In all other cases, the
	// output should come first as it describes what is about to happen.
	//int IO_check = serial_init(COM1);
	int IO_check = serial_init(COM1);
	if (IO_check != 0) {
		klogv(COM1, "Serial I/O initization failed");
		//exit(1);
	}
	klogv(COM1, "\nInitialized serial I/O on COM1 device...");

	// 1) Global Descriptor Table (GDT) -- <mpx/gdt.h>
	// Keeps track of the various memory segments (Code, Data, Stack, etc.)
	// required by the x86 architecture. This needs to be initialized before
	// interrupts can be configured.
	gdt_init();
	klogv(COM1, "Initializing Global Descriptor Table...");

	// 2) Interrupt Descriptor Table (IDT) -- <mpx/interrupts.h>
	// Keeps track of where the various Interrupt Vectors are stored. It
	// needs to be initialized before Interrupt Service Routines (ISRs) can
	// be installed.
	idt_init();
	klogv(COM1, "Initializing Interrupt Descriptor Table...");

	// 3) Disable Interrupts -- <mpx/interrupts.h>
	// You'll be modifying how interrupts work, so disable them to avoid
	// crashing.
	cli();
	klogv(COM1, "Disabling interrupts...");

	// 4) Interrupt Request (IRQ) -- <mpx/interrupts.h>
	// The x86 architecture requires ISRs for at least the first 32
	// Interrupt Request (IRQ) lines.
	irq_init();
	klogv(COM1, "Initializing Interrupt Request routines...");

	// 5) Programmable Interrupt Controller (PIC) -- <mpx/interrupts.h>
	// The x86 architecture uses a Programmable Interrupt Controller (PIC)
	// to map hardware interrupts to software interrupts that the CPU can
	// then handle via the IDT and its list of ISRs.
	pic_init();
	klogv(COM1, "Initializing Programmable Interrupt Controller...");

	// 6) Reenable interrupts -- <mpx/interrupts.h>
	// Now that interrupt routines are set up, allow interrupts to happen
	// again.
	sti();
	klogv(COM1, "Enabling Interrupts...");

	// 7) Virtual Memory (VM) -- <mpx/vm.h>
	// Virtual Memory (VM) allows the CPU to map logical addresses used by
	// programs to physical address in RAM. This allows each process to
	// behave as though it has exclusive access to memory. It also allows,
	// in more advanced systems, the kernel to swap programs between RAM and
	// storage (such as a hard drive or SSD), and to set permissions such as
	// Read, Write, or Execute for pages of memory. VM is managed through
	// Page Tables, data structures that describe the logical-to-physical
	// mapping as well as manage permissions and other metadata.
	vm_init();
	klogv(COM1, "Initializing Virtual Memory...");

	// 8) MPX Modules -- *headers vary*
	// Module specific initialization -- not all modules require this.
	klogv(COM1, "Initializing MPX modules...");
	initialize_heap(50000);
	// R5: sys_set_heap_functions(...);
	sys_set_heap_functions(allocate_memory, free_memory);
	// R4: create commhand and idle processes

	// 9) YOUR command handler -- *create and #include an appropriate .h file*
	// Pass execution to your command handler so the user can interact with
	// the system.
	
	// R4: Removing call to our command handler 
	/**
	 * klogv(COM1, "Transferring control to commhand...");
	 * comhand();
	 */

	// Create two SYSTEM processes:
	// Command Handler Process, running at highest priority
	// allocate and initialize new PCB
	pcb_t* comhand_pcb_ptr = pcb_allocate(); 

	// fill fields of comhand PCB
	strcpy(comhand_pcb_ptr->name, "comhand"); 
	comhand_pcb_ptr->priority = 0;
	comhand_pcb_ptr->state = PCB_READY;
	comhand_pcb_ptr->suspended = PCB_NOT_SUSPENDED;
	comhand_pcb_ptr->class_type = PCB_SYSTEM; 

	// Clear stack
	memset(comhand_pcb_ptr->stack, 0, STACK_SIZE); 

	// Record top-of-stack pointer in the pcb 
	comhand_pcb_ptr->stack_ptr = comhand_pcb_ptr->stack_ptr - sizeof(context_t); 
	
	// context saved at the top of the new stack
	context_t* comhand_ctx = (context_t*)comhand_pcb_ptr->stack_ptr; 

	// initialize registers
	comhand_ctx->ds = 0x10;
	comhand_ctx->es = 0x10;
	comhand_ctx->fs = 0x10;
	comhand_ctx->gs = 0x10;
	comhand_ctx->ss = 0x10;

	comhand_ctx->eax = 0;
	comhand_ctx->ebx = 0;
	comhand_ctx->ecx = 0;
	comhand_ctx->edx = 0;
	comhand_ctx->esi = 0;
	comhand_ctx->edi = 0;
	
	// ebp = bottom of allocated stack
	comhand_ctx->ebp = (unsigned int)(comhand_ctx + sizeof(context_t)); 

	// EIP points to the start function
	comhand_ctx->eip = (unsigned int)comhand;
	comhand_ctx->cs = 0x08; 
	comhand_ctx->eflags = 0x0202; 

	// insert comhand PCB into ready queue
	pcb_insert(comhand_pcb_ptr); 

	// System idle process, running at lowest priority 
	// allocate and initialize new PCB
	pcb_t* system_idle_pcb_ptr = pcb_allocate(); 

	// fill PCB fields, give system idle process a name and lowest priority 
	strcpy(system_idle_pcb_ptr->name, "sysIDLE0"); 
	system_idle_pcb_ptr->priority = 9; 
	system_idle_pcb_ptr->state = PCB_READY; 
	system_idle_pcb_ptr->suspended = PCB_NOT_SUSPENDED;
	system_idle_pcb_ptr->class_type = PCB_SYSTEM; 

	// Clear stack
	memset(system_idle_pcb_ptr->stack, 0, 1024); 

	// Record top-of-stack pointer in the pcb 
	system_idle_pcb_ptr->stack_ptr = system_idle_pcb_ptr->stack_ptr - sizeof(context_t); 

	// save context at top of new stack
	context_t* sysIDLE_ctx = (context_t*) system_idle_pcb_ptr->stack_ptr; 

	//initialize registers 
	sysIDLE_ctx->ds = 0x10;
	sysIDLE_ctx->es = 0x10;
	sysIDLE_ctx->fs = 0x10;
	sysIDLE_ctx->gs = 0x10;
	sysIDLE_ctx->ss = 0x10;

	sysIDLE_ctx->eax = 0;
	sysIDLE_ctx->ebx = 0;
	sysIDLE_ctx->ecx = 0;
	sysIDLE_ctx->edx = 0;
	sysIDLE_ctx->esi = 0;
	sysIDLE_ctx->edi = 0;

	//ebp = bottom of allocated stack
	sysIDLE_ctx->ebp = (unsigned int)(sysIDLE_ctx + sizeof(context_t)); 

	//EIP points to the start function
	sysIDLE_ctx->eip = (unsigned int)sys_idle_process; 
	sysIDLE_ctx->cs = 0x08;
	sysIDLE_ctx->eflags = 0x0202; 

	// insert sysIDLE pcb into ready queue
	pcb_insert(system_idle_pcb_ptr); 
	idle_pcb = system_idle_pcb_ptr; 

	klogv(COM1, "Relinquishing control to comhand...");
	serial_open(COM1, 9600);
	__asm__ volatile ("int $0x60" :: "a"(IDLE));

	// 10) System Shutdown -- *headers to be determined by your design*
	// After your command handler returns, take care of any clean up that
	// is necessary.
	klogv(COM1, "Starting system shutdown procedure...");

	// 11) Halt CPU -- *no headers necessary, no changes necessary*
	// Execution of kmain() will complete and return to where it was called
	// in boot.s, which will then attempt to power off Qemu or halt the CPU.
	klogv(COM1, "Closing COM1 device then halting CPU...");
	serial_close(COM1);
}
