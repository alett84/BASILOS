bits 32
global sys_call_isr

;;; System call interrupt handler. To be implemented in Module R3.
extern sys_call			; The C function that sys_call_isr will call
sys_call_isr:
	; do NOT push EFLAGS, CS, and EIP. Push ESP last
	; pushing general purpose registers
	push ebp
	push edi
	push esi 
	push edx
	push ecx
	push ebx
	push eax
	; pushing segment registers
	push ss
	push gs
	push fs
	push es
	push ds

	; switch that context
	push esp
	call sys_call
	mov esp, eax

	; popping segment registers
	pop ds
	pop es
	pop fs
	pop gs
	pop ss
	; popping general purpose registers
	pop eax
	pop ebx 
	pop ecx
	pop edx
	pop esi
	pop edi
	pop ebp

	iret
