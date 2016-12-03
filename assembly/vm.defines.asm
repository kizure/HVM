; VM data types
%define DATA_TYPE_INT       0
%define DATA_TYPE_STRING    1
%define DATA_TYPE_FLOAT     2
%define DATA_TYPE_ARRAY     3
%define DATA_TYPE_POINTER   4
%define DATA_TYPE_COUNT     5

; VM Variable struct

; Struct VM_VARIABLE
;   int32 Type
;   int32 *ptr (for array or string only)
;   int32 value (for int, float and pointer only)
%define VM_VARIABLE_SIZE            12
%define VM_VARIABLE_TYPE            0
%define VM_VARIABLE_DATA_PTR        4
%define VM_VARIABLE_DATA_VAL        8

; Struct Vm
;   Stack* DataStack
;   Stack* CallStack
;   char* ProgData
;   int32 IP (Instruction pointer)
; ... think more stuff to be added.
%define VM_DATASTACK    0
%define VM_CALLSTACK    4
%define VM_PROGDATA     8
%define VM_IP           12
%define VM_SIZE         16

%define NUM_OF_OPERATIONS   22
%define VM_OPERATION_ADD    0
%define VM_OPERATION_SUB    1
%define VM_OPERATION_MUL    2
%define VM_OPERATION_DIV    3
%define VM_OPERATION_MOD    4
%define VM_OPERATION_OR     5
%define VM_OPERATION_XOR    6
%define VM_OPERATION_NOT    7
%define VM_OPERATION_AND    8
%define VM_OPERATION_JMP    9
%define VM_OPERATION_CALL   10
%define VM_OPERATION_RET    11
%define VM_OPERATION_TEST   12
%define VM_OPERATION_PUSH   13
%define VM_OPERATION_END    14
%define VM_OPERATION_SHL    15
%define VM_OPERATION_SHR    16
%define VM_OPERATION_POP    17
%define VM_OPERATION_BRKPT  18
%define VM_OPERATION_LDITM  19
%define VM_OPERATION_STITM  20
%define VM_OPERATION_ECALL  21

; 2D Dispatch table
; Struct
;   in32 x_size
;   2d_array dispatch_table
%define DISPATCH_X_SIZE 0
%define DISPATCH_METADATA_SIZE 4







