; Integer data type implementation for the VM.
; Contains the operations which the vm does on integer values.

; +0 *vm_instance
datatype.int.add:
    push ebp
    mov ebp, esp
    
    push eax
    push ebx
    
        mov eax, [ebp+8] ; Vm_instance*
    
    pop ebx
    pop eax
    
    
    pop ebp
    ret
    
    
datatype.int.sub:
    push ebp
    mov ebp, esp
    
    pop ebp
    ret