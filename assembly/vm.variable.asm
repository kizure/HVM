
; For some reason it doesn't work, throws a seg fault as a macro
; But works fine if inlined out the macro? wat?
%macro create_variable 2 ; type, value/pointer
    push dword VM_VARIABLE_SIZE
        call mmalloc
    add esp, 4
    
    ; Fill vm_variable struct with information.
    mov dword [eax+VM_VARIABLE_TYPE], %1
    mov dword [eax+VM_VARIABLE_DATA], %2
    
    ; eax contains pointer to variable.
    
%endmacro