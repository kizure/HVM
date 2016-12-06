%include "io.inc"

; For calling conventions.
; http://unixwiz.net/techtips/win32-callconv-asm.html
; Values return through EAX from functions.
; callee saved registers are EAX, ECX, EDX

extern _malloc
extern _free
extern _printf

section .data
    vm_alloc_failed db "VM: Allocation failed!", 0
    sfmt db "%s", 10 , 0 ; 10 means new line '\n'
    stack_pop_underflow db "Stack pop failed, underflow!", 0
    stack_peek_underflow db "Stack peek failed, underflow!", 0
    stack_lookback_underflow db "Stack lookback failed, underflow!", 0
    vm_variable_type_mismatch db "Variable type mismatch!", 0

    ; Pointers for the stack and dispatch structs.
    
    stack: dd 0
    dispatch: dd 0

section .text
global CMAIN

%include "vm.defines.asm"

CMAIN:
    mov ebp, esp; for correct debugging
    ; Clear all registers.
    
    ; read files of bytes or something from the arguments at the start.
    
    push 16
    call create_stack
    add esp, 4
    
    mov [stack], eax ; Stack into edx
    
    push NUM_OF_OPERATIONS ; columns (num of operations)
    push DATA_TYPE_COUNT ; rows (num of data types)
        call create_dispatch
    add esp, 8
    
    mov [dispatch], eax ; Address for dispatch register.
    
    push datatype.int.add
    push datatype.int.sub
    push 2
    push 3
    push 4
    push 5
    push 6
    push 7
    push 8
    push 9
    push 10
    push 11
    push 12
    push 13
    push 14
    push 15
    push 16
    push 17
    push 18
    push 19
    push 20
    push 21
    push DATA_TYPE_INT ; Y Index
    push dword [dispatch] ; Dispatch table
        call dispatch_populate_1d
    add esp,96
    
    test_loop:
    ; Create variable and push to stack
        push dword VM_VARIABLE_SIZE
            call mmalloc
        add esp, 4
        
        ; Fill vm_variable struct with information.
        mov dword [eax+VM_VARIABLE_TYPE], DATA_TYPE_INT
        mov dword [eax+VM_VARIABLE_DATA], 12
    push dword [stack] ; Stack
    push eax ; Stack value
        call stack_push
    add esp, 8
    
    ; Create variable and push to stack
        push dword VM_VARIABLE_SIZE
            call mmalloc
        add esp, 4
        
        ; Fill vm_variable struct with information.
        mov dword [eax+VM_VARIABLE_TYPE], DATA_TYPE_INT
        mov dword [eax+VM_VARIABLE_DATA], 16
    push dword [stack] ; Stack
    push eax
        call stack_push
    add esp, 8
    
    %ifdef EN_PREFETCH
        ; prefetcht0 [dispatch_address]
    %endif
    
    ; test if it works.
    push DATA_TYPE_INT
    push VM_OPERATION_ADD
    push dword [dispatch] ; Push *dispatch
       call dispatch_get
    add esp, 12
    
    ; For testing only.
    ; Pop and free
    
    ; Everywhere else is fine.
    push dword [stack] ; Push *stack (should be vm but works for testing)
        call eax ; Call the address which we dispatched to.
    add esp, 4
    
    jmp test_loop
    
    ret
 
; push zero terminated string address.
println_string:
    push ebp
    mov ebp, esp
    
    push dword [ebp+8] ; string address
    push dword sfmt ; String format
        call _printf
    add esp, 8
    
    mov esp, ebp
    pop ebp
    ret
    
; Include into the text section
%include "vm.type.int.asm"
%include "vm.memory.asm"
%include "vm.dispatch.asm"
%include "stack.asm"
