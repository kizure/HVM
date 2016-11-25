# HappyVM - Stack-based virtual machine
HappyVM or HVM for short is a stack-based VM meaning that is has no registers, it uses a stack to store all data on.

## Data Types:
- Float (32 bit)
- Integer (32 bit)
- Pointer (32 bit)
- Array
- String

## Instruction Set:
| Instruction | Description | Extra Information |
| ----------- | ----------- | ----------------- |
| ADD | Pops and adds top values off the stack | |
| SUB | Pops and subtracts top values off the stack | |
| MUL | Pops and multiplies top values off the stack | |
| DIV | Pops and divides top values off the stack | |
| MOD | Pops and takes the modulus top values off the stack | |
| OR | Pops and ORs top values off the stack | |
| XOR | Pops and XORs top values off the stack | |
| NOT | Pops and NOTs top value off the stack | |
| AND | Pops and ANDs top values off the stack | |
| JMP | Jumps to a specific address in the program code | |
| CALL | Jumps to sub routine and pushes current address to call stack to return | Combine with RET to return |
| RET  | Returns from a sub routine, pops address off call stack and jmps to it | Combine with CALL to go to sub routine |
| TEST | Compares the top two values in the stack | Values are preserved (not pushed off stack) |
| PUSH | Pushes a value to the stack (float, int, string, pointer, array)| |
| POP | Pops a value off the stack | |
| END | Stops the virtual machine from running | |
| SHL | Pops and shifts a value to the left by x amount | |
| SHR | Pops and shifts a value to the right by x amount | |
| BRKPT | Breakpoint, if a debugger is attached to the VM then it will fire a breakpoint | No effect without debugger attached | 

## Usage:
In the VMs current state, the VM only runs through programming it and compiling the project.
In the main HappyVM.cpp file the following code can be used as an example to run a basic program

```
	VmEmitter* emitter = new VmEmitter(64);
	int start = emitter->label();
	
	// (2 + 4) * (5 * 2)
	emitter->push(2);
	emitter->push(4);
	emitter->add();
	emitter->push(5);
	emitter->push(2);
	emitter->mul();
	emitter->mul();
	emitter->end();

	HappyVM* vm = new HappyVM(emitter->complete());
	
	vm->run();
```

The VM Emitter supports jump patching so labels can be used without knowing their address immediately. (jumping forward)

## Todo List:
- Test POINTER data type. (Now supports pointer -> pointer -> ... -> value but needs testing.)
- Test skipInstruction to make sure it works correctly.
	
### Long term:
- Runtime optimization (aimed at JIT)
- Make compatible with any endianness.
- make into a JIT-interpreter hybrid compiler. (X86 based.)
- Make some sort of programming language so it can be used easily.
- Implement the language slowly and build it up. for example start at 1+2

### VM Limitations:
- Fixed sized stack for both call stack and data stack. (cannot dynamically shrink and grow)
- Limited to a max of 32 unqiue instructions (shouldn't be an issue I hope.)
- Max compiled string length is 65535 (runtime is 'unlimited')
