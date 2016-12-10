; Fixed size stack implementation for fixed sized structs.
; Could be made dynamic in the future
; The stack stores the memory addresses to the data.
; Not the actual data.

; Stack Struct
; int32 StackPointer (Index)
; char* StackData

%define STACK_POINTER 0
%define STACK_ARRAY 4

%define POINTER_BYTE_SIZE 4

; +0 Stack size
; ret: stack pointer
create_stack:
    push ebp
    mov ebp, esp
    
    mov eax, [ebp+8]  ; Stack size
    add eax, POINTER_BYTE_SIZE ; For Stack Pointer space
    
    ; Allocate memory for the stack
    push eax
        call mmalloc
    add esp, 4
    
    mov dword [eax+STACK_POINTER], 0 ; Set stack pointer value to 0
    
    mov esp, ebp
    pop ebp
    ret
    
; +0 Stack*
delete_stack:
    push ebp
    mov ebp, esp
    
    ; Free stack memory.
    push dword [ebp+8] ; Stack Pointer
        call mfree
    add esp, 4
    
    mov esp, ebp
    pop ebp
    ret
   
; +0 Stack*
; return *Item
stack_pop:
    push ebp
    mov ebp, esp
    
    ; if (stack->Pointer-1 < 0)
    ;   printf("error...");
    ;   return &0;
    ; else
    ; return stack->Array[--stack->Pointer];
    
    mov eax, [ebp+8] ; Stack address
    
    push ebx
        sub dword [eax+STACK_POINTER], 1 ; Decrement pointer by 1
        mov ebx, [eax+STACK_POINTER] ; Put pointer into EBX
        
        test ebx, ebx ; AND ebx with itself
        jl stack_pop.failed ; If pointer < 0 then fail. (underflow)
    
        mov eax, [ebx*POINTER_BYTE_SIZE+eax+STACK_ARRAY] ; *4 for sizeof(int32)
    jmp stack_pop.end ; TODO: possibly inline to reduce jmps?
    
stack_pop.failed:
    push stack_pop_underflow
        call println_string
    add esp, 4
    
    xor eax, eax ; Set to zero address meaning failed.
    jmp stack_pop.end
    
stack_pop.end:
    pop ebx
    
    mov esp, ebp
    pop ebp
    ret

; +0 *Stack
; +1 *Item
stack_push:
    push ebp
    mov ebp, esp
    
    push eax
    push ebx
    push ecx
    
        ; stack->Array[stack->Pointer++] = item as pointer
    
        mov eax, [ebp+8] ; *Item
        mov ebx, [ebp+12] ; *Stack
        mov ecx, [ebx+STACK_POINTER] ; Stack index
        
        ; TODO: add stack overflow checking. (Kinda not possible without the size.)
        
        mov [ecx*POINTER_BYTE_SIZE+ebx+STACK_ARRAY], eax ; Put *Item into array
        
        add dword [ebx+STACK_POINTER], 1 ; Increment stack pointer
        
    pop ecx
    pop ebx
    pop eax
    
    mov esp, ebp
    pop ebp
    ret
    
; +0 *Stack
; Ret address
stack_peek:
    push ebp
    mov ebp, esp
    
    push ebx
        ; return stack->Array[stack->Pointer-1]
        
        mov eax, [ebp+8] ; *Stack
        mov ebx, [eax+STACK_POINTER] ; Pointer value
        sub ebx, 1 ; Temporary subtract one.
        test ebx, ebx
        jl stack_peek.failed ; Underflow
        
        mov eax, [eax+ebx*POINTER_BYTE_SIZE+STACK_ARRAY] ; Put value at top of stack into eax to return
    
    pop ebx
    
    mov esp, ebp
    pop ebp
    ret

stack_peek.failed:
    push stack_peek_underflow
        call println_string
    add esp, 4
    xor eax, eax ; Set pointer to zero to show it failed.
    pop ebx
    
    mov esp, ebp
    pop ebp
    ret
    
; +0 *Stack
; +1 Lookback value
; ret address
stack_lookback:
    push ebp
    mov ebp, esp
    
    push ebx
        ; return stack->Array[stack->Pointer-lookback]
        mov eax, [ebp+12] ; stack*
        mov ebx, [eax+STACK_POINTER] ; Pointer value
        sub ebx, 1 ; As required by peek too as the pointer value is in the next cell.
        sub ebx, dword [ebp+8] ; Pointer value - look back value
        test ebx, ebx
        jl stack_lookback.failed ; Underflow
        
        mov eax, [eax+ebx*POINTER_BYTE_SIZE+STACK_ARRAY]
        
    pop ebx
    
    mov esp, ebp
    pop ebp
    ret
    
stack_lookback.failed:
    push stack_lookback_underflow
        call println_string
    add esp, 4
    xor eax, eax ; Set to zero to show it failed.
    pop ebx
    
    mov esp, ebp
    pop ebp
    ret