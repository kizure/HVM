; Integer data type implementation for the VM.
; Contains the operations which the vm does on integer values.

; This is what I want operations to look like for each data type in the future
; Need to distinguish between two operand instructions and single operand instructions.
; Not sure how I am going to do that in the VM.
; Encode operand count into the instruction somehow?
; I want to do this to reduce code redundancy as much as possible.
; +0 *var a
; +1 *var b
;datatype.int.add:
;    push ebp
;    mov ebp, esp
;    
;    push eax
;    push ebx
;        mov ebx, [ebp+8] ; *b
;        mov eax, [ebp+12] ; *a
;        mov ebx, [ebx+VM_VARIABLE_DATA] ; get data in B
;        add [eax+VM_VARIABLE_DATA], ebx
;    pop ebx
;    pop eax
;    
;    pop ebp
;    ret
    
; +0 *vm_instance
datatype.int.add:
    push ebp
    mov ebp, esp
    
    push eax ; *vm_instance / variable A
    push edx ; B variable
    push edi ; temp
        
        mov eax, [ebp+8] ; Vm_instance*
        push eax
            ;push dword [eax+VM_DATASTACK] ; pop off the data stack
            push eax ; tmp I think for the moment as I am actually pushing the stack for testing.
                call stack_pop
            add esp, 4
            mov edx, eax ; B variable
        pop eax
        
        ;push eax
            ;push dword [eax+VM_DATASTACK] ; pop off the data stack
            push eax ; tmp
                call stack_pop
            add esp, 4
            ;mov ecx, eax ; A variable
        ;pop eax
        
        ; Reuse eax as it is no longer needed.
        ; Type check both vm_variables
        mov edi, [eax+VM_VARIABLE_TYPE] ; Get variable type A
        cmp edi, [edx+VM_VARIABLE_TYPE] ; compare a type with b type.
    
        jne datatype.int.mismatch       ; if types are not equal, then throw error.
        
        ; A = A+B
        mov edi, [edx+VM_VARIABLE_DATA]
        add [eax+VM_VARIABLE_DATA], edi
        
        ; Destroy/Release B
        
        jmp datatype.int.add.continue
        
    datatype.int.mismatch:
        push vm_variable_type_mismatch
            call println_string
        add esp, 4
    
    datatype.int.add.continue:
    
    ; TEMPORARY CODE FOR TESTING PURPOSES ONLY.
    ; Free both integers, this won't happen in actual vm.
    
    push eax
        call mfree
    add esp, 4
    
    push edx
       call mfree
    add esp, 4
    
    pop edi
    pop edx
    pop eax
    
    mov esp, ebp
    pop ebp
    ret
    
datatype.int.sub:
    push ebp
    mov ebp, esp
    
    pop ebp
    ret