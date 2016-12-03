; push size
; return eax (address)
mmalloc:
    push ebp
    mov ebp, esp
    
    ; size
    push dword [ebp+8]
        call _malloc
    add esp, 4
    test eax, eax ; Test if allocation failed.
    jz mmalloc.failed

    pop ebp
    ret

mmalloc.failed:
    push vm_alloc_failed
        call println_string
    add esp, 4
    
    pop ebp ; End of function cleanup
    ret
            
; push address
mfree:
    push ebp
    mov ebp, esp
    
    push dword [ebp+8]
        call _free
    add esp, 4
    
    pop ebp
    ret