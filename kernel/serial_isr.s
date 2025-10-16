bits 32
global serial_isr

extern serial_interrupt
extern get_private_interrupt_stack

serial_isr:
    cli 

    ; switching to special private interrupt stack (important)
    call get_private_interrupt_stack
    mov ebx, esp ; save original ss in ebx so it can make it around stack switch
    mov esp, eax ; switch to private interrupt stack
    push ebx     ; save original ss to first thing in private interrupt stack 

    call serial_interrupt

    pop esp      ; go back to original place
    
    sti
    iret
