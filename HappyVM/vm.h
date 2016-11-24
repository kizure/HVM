#include "stack.h"
#include "HObject.h"

#ifndef VM_H
#define VM_H

	/* ------ MicroVM Instruction Set ----  */
	/* Instruction Type: 1 byte             */
	/* InstrArg Count: 1 byte               */
	/* ArgType Count: ArgCount*1 byte       */
	/* Arg Data: ArgCount*sizeof(arg) bytes */
	/* -------------------------------      */

	/*
		Push - Pushes a value to the stack
		Mov
		Add
		Sub
		Mul
		Div
		Mod
		Or
		Xor
		Not
		And
		Jmp
		Shl
		Shr
		Call (Jump sub routine)
		Retv
		Test (skip next instruction if false)
		End (Stops the VM)
	*/

#define HOP_INT_TYPE		0
#define HOP_FLOAT_TYPE		1
#define HOP_STRING_TYPE		2
#define HOP_ARRAY_TYPE		3
#define HOP_POINTER_TYPE	4

#define HOP_CONDITION_EQUAL					1
#define HOP_CONDITION_LESS					2
#define HOP_CONDITION_GREATER				3
#define HOP_CONDITION_NOT_EQUAL				4
#define HOP_CONDITION_LESS_THAN_EQUAL_TO	5
#define HOP_CONDITION_GREATER_THAN_EQUAL_TO 6

typedef struct {
	int address; // pointer to place in code.
	int funcAddress; // Function place in code being called.
} CallFrame;

class HappyVM {
public:
	HappyVM(void);
	HappyVM(char*);
	~HappyVM(void);
	void run(void);
	void stop();
	void setProgram(char*);
	Stack<HObject> *dataStack; // Data Stack
	
private:
	void execute();
	void pushObject();
	HObject* popObjFromStack();
	HObject* recursivePointers(HObject* o);
	int getInt();
	void skipInstruction();

	bool running;
	int ip; // Instruction pointer.
	Stack<CallFrame> *callStack; // Call Stack used for the
	char* prog; // Program data
	long double ltime; // For benchmarking.
	long ops;
};

#endif