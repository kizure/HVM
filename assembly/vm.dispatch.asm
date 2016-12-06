; === VM datatype dependant dispatch table implementation ===
; Need constraints as it will be a 2D array for the first index (data type)
; Stop out of bounds stuff, kinda upsets performance but not much I can do I guess?

; As I want to have dynamic typing for arrays and values I need to be able to apply the 
; operation to each data type.
; I could do this by populating a dispatch table for each type to their associated implementation
; of that operation. This would make it easier to implement in assembly me thinks.

; +0 X Size
; +1 Y Size
; Return *2d_dispatch
create_dispatch:
    push ebp
    mov ebp, esp
    
    ; Allocate some memory for the dispatch table.

    push ebx
        mov ebx, [ebp+12] ; X Size
        mov eax, [ebp+8] ; Y Size
        mul ebx ; X*Y for total size
        imul eax, eax, 4 ; eax=eax*4 (for pointer size)
        
        add eax, DISPATCH_METADATA_SIZE ; Meta data at the start.
    
        ; Allocate the memory.
        push eax
            call mmalloc
        add esp, 4
        ; EAX has memory address for dispatch table
        
        mov dword [eax+DISPATCH_X_SIZE], ebx ; Put X size into meta data.
    pop ebx
    
    mov esp, ebp
    pop ebp
    ret
    
; +0 Dispatch table
delete_dispatch:
    push ebp
    mov ebp, esp
    
    push eax
    
        mov eax, [ebp+8] ; *Dispatch table
        push eax
            call mfree
        add esp, 4
    
    pop eax
    
    mov esp, ebp
    pop ebp
    ret


; Variable arguments (DISPATCH_X_SIZE quantity of arguments)
; +n Y index
; +n+1 *Dispatch_table
dispatch_populate_1d:
    push ebp
    mov ebp, esp
    
    ;
    ;   for (int i = 0; i < DISPATCH_X_SIZE;i++)
    ;       dispatch->table[DISPATCH_X_SIZE*y_index*4+i*4] = "stack"[12+(DISPATCH_X_SIZE-i)*4]
    
    push eax ; Dispatch table
    push ebx ; Y Index
    push ecx ; Start memory address.
    push edx ; X Size, reused in loop for stack addressing
    push edi ; Destination address counter
    push esi ; Stack address counter
        
        ; [4 * (Y * row_size + x_offset) + dispatch_table+DISPATCH_METADATA_SIZE] = [ebp+12+(x_offset*4)]
        mov eax, [ebp+8]
        mov ebx, [ebp+12]
        mov edx, [eax+DISPATCH_X_SIZE]
        mov ecx, ebx ; Put Y index into memory address
        imul ecx, edx ; Multiply by row size
        lea edi, [ecx*4+eax+DISPATCH_METADATA_SIZE] ;  4 * 1d_address + dispatch_table
        lea edx, [edx*4] ; X_Size in bytes (4 bytes per pointer)
        lea esi, [ebp+12+edx] ; offset for last item in the stack array.
        add edx, edi ; Add the base array pointer to the upper limit.
    
    ; Enter loop
    dispatch_populate_1d.loop:
        ; Copy from stack into dispatch table.
        cmp edi, edx ; Compare current index and upper bound
        je dispatch_populate_1d.break ; if current_index==upper_bound then break
        
        push eax
            mov eax, [esi]
            mov [edi], eax
        pop eax
        sub esi, 4 ; source, go backwards through source stack array.
        add edi, 4 ; destination
        jmp dispatch_populate_1d.loop
    
    ; Exit loop
    dispatch_populate_1d.break:
        
    pop esi
    pop edi
    pop edx
    pop ecx
    pop ebx
    pop eax
    
    mov esp, ebp
    pop ebp
    ret


; +0 Data type
; +1 Operation
; +2 *Dispatch table
; return address
dispatch_get:
    push ebp
    mov ebp, esp
    
    push ebx
    push ecx
    
        mov eax, [ebp+8] ; *Dispatch table.
        mov ebx, [ebp+12] ; Operation
        mov ecx, [ebp+16] ; Data type
        imul ecx, [eax+DISPATCH_X_SIZE] ; y*x_size
        add ecx, ebx ; 4*(ecx+ebx)+DISPATCH_METATABLE_SIZE
        lea ecx, [ecx*4+eax+DISPATCH_METADATA_SIZE]
        mov eax, [ecx]
        
    pop ecx
    pop ebx
    
    mov esp, ebp
    pop ebp
    ret