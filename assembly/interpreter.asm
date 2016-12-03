%include "io.inc"

; For calling conventions.
; http://unixwiz.net/techtips/win32-callconv-asm.html
; Values return through EAX from functions.

extern _malloc
extern _free
extern _printf

section .data
    vm_alloc_failed db "VM: Allocation failed!", 0
    sfmt db "%s", 10 , 0 ; 10 means new line '\n'
    stack_pop_underflow db "Stack pop failed, underflow!", 0
    stack_peek_underflow db "Stack peek failed, underflow!", 0
    stack_lookback_underflow db "Stack lookback failed, underflow!", 0

section .text
global CMAIN

%include "vm.defines.asm"

CMAIN:
    mov ebp, esp; for correct debugging
    ; Clear all registers.
    
    ; read files of bytes or something from the arguments at the start.
    
    push 12
    push 1024
    call create_stack
    add esp, 8
    
    push eax
    push 10 ; fake address
    call stack_push
    add esp, 8
        
    push eax
        push eax
        call stack_pop
        add esp, 4
    pop eax
    
    ; Do some stuff with the stack.
    
    push eax ; Currently stores the pointer to stack
    call delete_stack
    add esp, 4
    
    push 22 ; columns (num of operations)
    push 5 ; rows (num of data types)
    call create_dispatch
    add esp, 8
    
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
    push eax ; Dispatch table
    call dispatch_populate_1d
    add esp,96
    
    push eax
        push 0
        push 0
        push eax
        call dispatch_get
        add esp, 12
        
        call eax ; Call the address which we dispatched to.
        
    pop eax
    
    
    ret
 
; push zero terminated string address.
println_string:
    push ebp
    mov ebp, esp
    
    push dword [ebp+8] ; string address
    push dword sfmt ; String format
    call _printf
    add esp, 8
    
    pop ebp
    ret
    
; Include into the text section
%include "vm.type.int.asm"
%include "vm.memory.asm"
%include "vm.dispatch.asm"
%include "stack.asm"
