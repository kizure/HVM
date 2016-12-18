%include "io.inc"

; For calling conventions.
; http://unixwiz.net/techtips/win32-callconv-asm.html
; Values return through EAX from functions.
; callee saved registers are EAX, ECX, EDX

; TODO List:
;   - Implement every datatype obviously.
;   - Implement every variable type
;   - Implement bytecode reader
;   - Implement file reader. (not very important, if at all. might not even do it this way)
;
;   - Breakpoint Debugging feature:
;       For the breakpoint opcode, could do some cool cmd line stuff for debugging at that point?
;       Call stack inspection
;       Data stack inspection
;       Disassemble instructions around current instruction being executed.
;       Then exit and it would continue to next breakpoint

extern _malloc
extern _free
extern _printf
extern _time

section .data
    vm_alloc_failed db "VM: Allocation failed!", 0
    sfmt db "%s", 10 , 0 ; 10 means new line '\n'
    sfint db "%d", 10, 0
    stack_pop_underflow db "Stack pop failed, underflow!", 0
    stack_peek_underflow db "Stack peek failed, underflow!", 0
    stack_lookback_underflow db "Stack lookback failed, underflow!", 0
    vm_variable_type_mismatch db "Variable type mismatch!", 0
    vm_stopped db "VM finished running!", 0
    
    ; Technically all of this should be in the .bss segment.
    current_time dd 0
    last_time dd 0
    ticks dd 0
    ; http://stackoverflow.com/questions/1465927/how-can-i-access-system-time-using-nasm
    
    ; Program instructions.
    test_prog db 9, 0x0, 0x0, 0x0, 0x0
    ; Have an idea for the reserved space. I can make a magic signature which the bytecode compiler
    ; can find and then replace the 1MB of space with the bytecode, then run the application.
     resb 1024*1024 ; 1MB of space.
    ; jmp 0 (infinite loop)
    
    ; VM static pointer
    vm: dd 0

section .text
global CMAIN

%include "vm.defines.asm"

CMAIN:
    mov ebp, esp; for correct debugging
    ; Clear all registers.
    
    ; For debugging, allows for getting the address of them
    push eax
    
        mov eax, last_time
        mov eax, current_time
    
    pop eax
    
    ; read files of bytes or something from the arguments at the start.
    
    push VM_VARIABLE_SIZE * 1024
    push 1024 ; Call stack size
    push test_prog ; Program data
        call create_vm
    add esp, 12
    
    mov [vm], eax
    
    push dword [vm]
        call vm_run
    add esp, 4
    
    ret
    
    ;push NUM_OF_OPERATIONS ; columns (num of operations)
    ;push DATA_TYPE_COUNT ; rows (num of data types)
    ;    call create_dispatch
    ;add esp, 8
    
    ;mov [dispatch], eax ; Address for dispatch register.
    
    ;push datatype.int.add
    ;push datatype.int.sub
    ;push 2
    ;push 3
    ;push 4
    ;push 5
    ;push 6
    ;push 7
    ;push 8
    ;push 9
    ;push 10
;    push 11
;    push 12
;    push 13
;    push 14
;    push 15
;    push 16
;    push 17
;    push 18
;    push 19
;    push 20
;    push 21
;    push DATA_TYPE_INT ; Y Index
;    push dword [dispatch] ; Dispatch table
;        call dispatch_populate_1d
;    add esp,96
;    
;    ; test if it works.
;    push DATA_TYPE_INT
;    push VM_OPERATION_SUB
;    push dword [dispatch] ; Push *dispatch
;       call dispatch_get
;    add esp, 12
;    
;    ; For testing only.
;    ; Pop and free
;    
;    ; Everywhere else is fine.
;    push dword [stack] ; Push *stack (should be vm but works for testing)
;        call eax ; Call the address which we dispatched to.
;    add esp, 4
    
    ret

update_time:
    push ebp
    mov ebp, esp
   
    push eax
    push ecx
    push edx
        push 0 ; NULL (returns the value instead.)
        call _time
        add esp, 4
        
        mov [current_time], eax
    pop edx
    pop ecx
    pop eax
   
    mov esp, ebp
    pop ebp
    ret

; +0 Data stack size (bytes)
; +1 Call stack size (bytes)
; +2 *Progam data
; return *vm
create_vm:
    push ebp
    mov ebp, esp
    
    push ebx ; temp
    push ecx ; *vm
        push VM_SIZE
            call mmalloc
        add esp, 4
        
        mov ecx, eax
        
        mov ebx, [ebp+8] ; Prog data
        mov [ecx+VM_PROGDATA], ebx
        
        mov ebx, [ebp+12] ; Call stack size
        push ebx ; Stack size (bytes)
            call create_stack
        add esp, 4
        mov [ecx+VM_CALLSTACK], eax
        
        mov ebx, [ebp+16] ; Data stack size
        push ebx ; Stack size (bytes)
            call create_stack
        add esp, 4
        mov [ecx+VM_DATASTACK], eax
        
        mov [ecx+VM_IP], dword 0 ; Start at 0
        
        mov [ecx+VM_RUNNING], dword 0 ; This could be changed to byte to save 3 bytes of memory, don't really see the point atm.
        
        mov eax, ecx ; Put *vm into return register.
    pop ecx
    pop ebx
    
    mov esp, ebp
    pop ebp
    ret

; +0 *vm
vm_run:
    push ebp
    mov ebp, esp
    
    %define PROG_REG ebx
    %define VM_REG eax
    %define IP_REG ecx
    %define INSTRUCTION_REG dl ; single byte
    
    push eax ; *vm
    push ebx ; *prog_data
    push ecx ; IP
    push edx ; current_instruction
    push esi ; temp?
    
    xor edx, edx
    
    mov VM_REG, [ebp+8]
    
    mov PROG_REG, dword [eax+VM_PROGDATA]
    mov IP_REG, dword [eax+VM_IP]
    
    %ifdef EN_PREFETCH
        prefetcht0 [PROG_REG] ; prefetch instruction data.
    %endif
    
    mov [eax+VM_RUNNING], dword 1 ; Signals the vm is running
    
    vm_loop:
        inc dword [ticks]
        ; ========================= NON VM RELATED        
        push eax
        push edx
        push ecx
            
            ; Only check time every 5000 ticks, the actual time function slows the interpreter down, rip
            mov eax, [ticks]
            mov ecx, 5000 ; Every 5,000 ticks
            div ecx
            mov eax, edx ; Put modulo into eax
            cmp eax, 0
            jne vm_loop.pass_time
            
            call update_time
            
            mov eax, [current_time]
            sub eax, [last_time]
            cmp eax, 1
            jge vm_loop.printtime
            jmp vm_loop.pass_time
        
            vm_loop.printtime:
                push dword [ticks]
                    call println_int
                add esp, 4
                mov dword [ticks], 0 ; Reset
                mov eax, [current_time]
                mov [last_time], eax ; Last_time = current_time
        
            vm_loop.pass_time:
        pop ecx
        pop edx
        pop eax
        
        ; ==========================================
        
        ;jmp vm_loop
        
        ; While vm->running
        CMP [eax+VM_RUNNING], dword 0
        je vm_continue
        
        ; Currently only reading a single byte from the stream. Could read the whole 32 bits and then empty the register?
        ; Not sure how much performance improvement would be gotten vs complexity of the extra code.
        
        ; Get the next instruction and read it.
        mov INSTRUCTION_REG, [PROG_REG+IP_REG]
        ; Parse the instruction
        inc IP_REG
        
        ; End
        CMP INSTRUCTION_REG, VM_OPERATION_END
        je vm.end
        
        ; Ret
        cmp INSTRUCTION_REG, VM_OPERATION_RET
        je vm.ret
        
        ; Call
        cmp INSTRUCTION_REG, VM_OPERATION_CALL
        je vm.call
        
        ; Jmp
        cmp INSTRUCTION_REG, VM_OPERATION_JMP
        je vm.jmp
        
        cmp INSTRUCTION_REG, VM_OPERATION_POP
        je vm.pop
        
        cmp INSTRUCTION_REG, VM_OPERATION_BRKPT
        je vm.brkpt
        
        vm.pop:
            
            push eax
                ; Pop off the stack
                push dword [VM_REG+VM_DATASTACK]
                    call stack_pop
                add esp, 4
                
                ; Deallocate the memory which it was using.
                push eax
                    call mfree
                add esp, 4
                
            pop eax
            
            jmp vm_loop
        
        vm.jmp: ; JMP instruction
            push eax
                ; Update memory with the new IP
                mov [VM_REG+VM_IP], IP_REG
                push VM_REG
                    call vm_readint ; EAX contains jmp address.
                add esp, 4
                mov IP_REG, eax
            pop eax
        
            jmp vm_loop
        
        vm.ret: ; RET instruction
            ; Pop off call stack
            ; Set IP to the value popped off the call stack
            push eax
                push dword [eax+VM_CALLSTACK]
                    call stack_pop
                add esp, 4
                mov IP_REG, eax ; Go back to that address.
            pop eax
            jmp vm_loop
            
        vm.call: ; CALL instruction
            ; preserve eax
            push eax
                ; Update memory with the new IP
                mov [VM_REG+VM_IP], IP_REG
                push VM_REG
                    call vm_readint
                add esp, 4
                mov IP_REG, [VM_REG+VM_IP] ; Put new IP after reading int.
                mov esi, eax ; Use ESI as temp variable. Contains jmp address.
            pop eax
            
            ; Push ip to callstack for saving
            push dword [VM_REG+VM_CALLSTACK]
            push IP_REG
                call stack_push
            add esp, 8
            
            ; set reg_ip to new jmp address.
            mov IP_REG, esi
            
            jmp vm_loop
            
        vm.end: ; END instruction
            mov [eax+VM_RUNNING], dword 0 ; Stop vm from running
            jmp vm_loop ; Next instruction.
        
        vm.brkpt: ; Breakpoint instruction BRKPT
            int3
            jmp vm_loop
        
        
        ; Just in case as we are mimicking a switch statement, kinda.
        jmp vm_loop
        
    vm_continue:
    
    pop esi
    pop edx
    pop ecx
    pop ebx
    pop eax
    
    push vm_stopped
        call println_string
    add esp, 4
    
    ; VM stopped.
    
    mov esp, ebp
    pop ebp
    ret

; +0 *vm
vm_readint:
    push ebp
    mov ebp, esp
    
    push ebx
        mov eax, dword [ebp+8] ; Put *vm into eax
        mov ebx, dword [eax+VM_IP] ; Put IP into temp register
        add [eax+VM_IP], dword 4 ; Increment by 4 bytes
        mov eax, dword [eax+VM_PROGDATA] ; Put *vm->prog_data into eax
        mov eax, dword [eax+ebx] ; get the 4 bytes of the prog memory
    pop ebx
    
    mov esp, ebp
    pop ebp
    ret

; +0 *vm
; +1 data_type
; +2 data
vm_pushobj:
    push ebp
    mov ebp, esp
    
    push eax
    push ebx
    push ecx
    push edx
        
        mov ebx, dword [ebp+8] ; data
        mov ecx, dword [ebp+12] ; data_type
        mov edx, dword [ebp+16] ; *vm
        
        push dword VM_VARIABLE_SIZE
            call mmalloc
        add esp, 4 ; Memory reference in EAX
            
        ; Fill vm_variable struct with information.
        mov dword [eax+VM_VARIABLE_TYPE], ecx ; data type
        mov dword [eax+VM_VARIABLE_DATA], ebx ; data (either pointer or int / float)
        
        push dword [edx+VM_DATASTACK] ; Stack
        push eax ; Pointer to variable.
            call stack_push
        add esp, 8
    
    pop edx
    pop ecx
    pop ebx
    pop eax
        
    mov esp, ebp
    pop ebp
    ret
 
; push zero terminated string address.
println_string:
    push ebp
    mov ebp, esp
    
    push eax
    push ecx
    push edx
        push dword [ebp+8] ; string address
        push dword sfmt ; String format
            call _printf
        add esp, 8
    pop edx
    pop ecx
    pop eax
    
    mov esp, ebp
    pop ebp
    ret
    
println_int:
    push ebp
    mov ebp, esp
    
    push eax
    push ecx
    push edx
        push dword [ebp+8] ; string address
        push dword sfint ; String format
            call _printf
        add esp, 8
    pop edx
    pop ecx
    pop eax
    
    mov esp, ebp
    pop ebp
    ret

    
; Include into the text section
%include "vm.type.int.asm"
%include "vm.memory.asm"
%include "vm.dispatch.asm"
%include "stack.asm"
