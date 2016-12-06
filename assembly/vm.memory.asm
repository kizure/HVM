; push size
; return eax (address)
mmalloc:
    push ebp
    mov ebp, esp
    
    ; X86 calling convention.
    push ecx
    push edx 
        ; size
        push dword [ebp+8]
            call _malloc
        add esp, 4
    pop edx
    pop ecx
        
    test eax, eax ; Test if allocation failed.
    jz mmalloc.failed
    
    mov esp, ebp
    pop ebp
    ret

mmalloc.failed:
    push vm_alloc_failed
        call println_string
    add esp, 4
    
    mov esp, ebp
    pop ebp ; End of function cleanup
    ret
            
; push address
mfree:
    push ebp
    mov ebp, esp
    
    ; X86 calling convention, these have to be saved else they will be overwritten.
    push eax
    push ecx
    push edx
        push dword [ebp+8]
            call _free
        add esp, 4
    pop edx
    pop ecx
    pop eax
    
    mov esp, ebp
    pop ebp
    ret