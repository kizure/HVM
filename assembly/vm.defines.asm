; VM data types
%define DATA_TYPE_INT       0
%define DATA_TYPE_STRING    1
%define DATA_TYPE_FLOAT     2
%define DATA_TYPE_ARRAY     3
%define DATA_TYPE_POINTER   4
%define DATA_TYPE_COUNT     5

; Prefetch hinting to the processor. Used for dispatch table caching.
; Yet to be properly implemented.
%define EN_PREFETCH

; VM Variable struct

; Struct VM_VARIABLE
;   int32 Type
;   int32 *ptr (can be both pointer and immediate data value, like int or float)
%define VM_VARIABLE_SIZE        8
%define VM_VARIABLE_TYPE        0
%define VM_VARIABLE_DATA        4 ; Either pointer or actual value depending on data type.

; Struct Vm
;   Stack* DataStack
;   Stack* CallStack
;   char* ProgData
;   int32 IP (Instruction pointer)
; ... think more stuff to be added.
%define VM_DATASTACK    0       ; Pointer to the data stack.
%define VM_CALLSTACK    4       ; Pointer to the call stack.
%define VM_PROGDATA     8       ; Pointer to the start of the prog_data
%define VM_IP           12      ; VM_PROGDATA+VM_IP for current address
%define VM_RUNNING      16      ; Bool if the vm is running or not.
%define VM_SIZE         20

; This may be collapsed into a list of int32s as the stack can be used exactly for that.
; Less derefencing and memory allocating / free, therefore better execution time.
; Seems like a good idea instead of a single item struct and using malloc and free.
; Struct CallInfo
;   int32 calleeAddress

%define NUM_OF_OPERATIONS   22
%define NUM_OPS_PER_TYPE    12
%define VM_OPERATION_ADD    0       ; Integers work, require VM integration.
%define VM_OPERATION_SUB    1       ; Integers work, require VM integration.
%define VM_OPERATION_MUL    2
%define VM_OPERATION_DIV    3
%define VM_OPERATION_MOD    4
%define VM_OPERATION_OR     5
%define VM_OPERATION_XOR    6
%define VM_OPERATION_NOT    7
%define VM_OPERATION_AND    8
%define VM_OPERATION_JMP    9       ; Completed, not tested, Not per data type.
%define VM_OPERATION_CALL   10      ; Completed, not tested, Not per data type.
%define VM_OPERATION_RET    11      ; Completed, not tested, Not per data type.
%define VM_OPERATION_TEST   12      
%define VM_OPERATION_PUSH   13      ; Not per data type.
%define VM_OPERATION_END    14      ; Completed, works, Not per data type.
%define VM_OPERATION_SHL    15      
%define VM_OPERATION_SHR    16
%define VM_OPERATION_POP    17      ; Not per data type.
%define VM_OPERATION_BRKPT  18      ; Not per data type.
%define VM_OPERATION_LDITM  19      ; Not per data type.
%define VM_OPERATION_STITM  20      ; Not per data type.
%define VM_OPERATION_ECALL  21      ; Not per data type.

; 2D Dispatch table
; Struct
;   in32 x_size
;   2d_array dispatch_table
%define DISPATCH_X_SIZE 0
%define DISPATCH_METADATA_SIZE 4







