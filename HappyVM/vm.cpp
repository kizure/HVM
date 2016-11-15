#include "stdafx.h" // Always on top

#include "vm.h"
#include <assert.h>
#include <Windows.h>
#include <string>
#include <iostream>
#include "HInt.h"
#include "HPointer.h"
#include "HFloat.h"
#include "HString.h"
#include "HArray.h"
#include <ctime>

using namespace std;

HappyVM::HappyVM(void) {
	this->callStack = new Stack<CallFrame>(128);
	this->dataStack = new Stack<HObject>(2048);
	this->ip = 0;
	this->ops = 0;
}

HappyVM::HappyVM(char* data) {
	this->callStack = new Stack<CallFrame>();
	this->dataStack = new Stack<HObject>();
	this->ip = 0;
	this->prog = data; // Set the program data for the vm to execute.
	this->ops = 0;
}

void HappyVM::run(void) {
	running = true;
	while(running) { // Only allow 2 instructions to run.
		execute();
	}
}

void HappyVM::execute() {
	ops++;
	const long double now = time(0)*1000;

	if (now - this->ltime > 1000) {
		cout << ops << " operations/s" << endl;
		ops = 0;
		this->ltime = now;
	}

	char data = this->prog[this->ip++] & 0x1F;

	switch (data) {
	case HOP_ADD:
	case HOP_SUB:
	case HOP_MUL:
	case HOP_DIV:
	case HOP_AND:
	case HOP_OR:
	case HOP_XOR:
	case HOP_MOD:
	case HOP_SHR:
	case HOP_SHL:
		{
			HObject* b = this->popObjFromStack();
			HObject* a = this->popObjFromStack();
			a->operation(b, (HInstruction)data);
			this->dataStack->push(a);
			delete b; // Cleanup as b no longer exists.
		}
		break;
	case HOP_NOT:
		{
			HObject* a = this->popObjFromStack();
			a->operation(nullptr, HOP_NOT);
			this->dataStack->push(a);
		}
		break;
	case HOP_PUSH:
		{
			this->pushObject();
		}
		break;
	case HOP_POP: // Remove from the stack, does nothing really, just for cleanup.
		{
			delete this->dataStack->pop();
		}
		break;
	case HOP_JMP:
		{
			int addr = this->getInt();
			this->ip = addr; // Goto the specified address for the jump.
		}
		break;
	case HOP_CALL:
		{
			int addr = this->getInt();
			CallFrame* frame = new CallFrame();
			frame->address = this->ip;
			frame->funcAddress = addr;
			this->callStack->push(frame);
			this->ip = addr;
		}
		break;
	case HOP_RET:
		{
			CallFrame* frame = this->callStack->pop();
			this->ip = frame->address;
			delete frame; // Unallocate memory.
		}
	case HOP_TEST:
		{
			char cmp = this->prog[this->ip++];
			HObject* b = this->popObjFromStack();
			HObject* a = this->popObjFromStack();
			int result = a->cmp(b);

			this->dataStack->push(a);
			this->dataStack->push(b);

			// a != b then break, this is a special one for integers / floats. 10 > 2 and isn't equal to it either.
			if (cmp == HOP_CONDITION_NOT_EQUAL && 
				(result == HOP_CONDITION_LESS || result == HOP_CONDITION_GREATER || result == HOP_CONDITION_NOT_EQUAL))
				break;

			if (cmp == result)
				break;

			// If the condition fails then skip that instruction.
			this->skipInstruction();
		}
		break;
	case HOP_END:
		this->running = false;
		break;
	case HOP_BRKPT: // Triggers a breakpoint
		cout << "VM: Breakpoint triggered at program address: " << this->ip << endl;
		if (IsDebuggerPresent())
			DebugBreak();
		break;
	default:
		break;
	}
}

void HappyVM::skipInstruction() {
	char data = this->prog[this->ip++] & 0x1F;
	char dataType = this->prog[this->ip-1] & 0xE0; // Only get the top 3 bits.
	if (data == HOP_PUSH)
	{
		switch (dataType) {
		case HOP_INT_TYPE:
		case HOP_FLOAT_TYPE:
		case HOP_POINTER_TYPE:
			this->ip += 4;
			break;
		case HOP_ARRAY_TYPE: // Needs to be implemented.
			assert(false && "skip instruction array data type not implemented yet");
			break;
		case HOP_STRING_TYPE: 
			{	
				char uLen = this->prog[this->ip++];
				char lLen = this->prog[this->ip++];
				int len = uLen << 8 | lLen;
				this->ip+=len;
			}
				break;
		}
	} 
	else if (data==HOP_TEST)
		this->ip+=1;
	else if (data == HOP_JMP || data == HOP_CALL)
		this->ip+=4;
}

HObject* HappyVM::popObjFromStack() {
	HObject* obj = this->dataStack->pop();

	// Check if it is a pointer, if so 
	if (ObjectUtils::instanceof<HPointer>(obj)) {
		HPointer* pointer = dynamic_cast<HPointer*>(obj);
		int offset = *static_cast<int*>(pointer->getValue());
		obj = this->recursivePointers(this->dataStack->lookback(offset));
		delete pointer;
	}

	return obj;
}

HObject* HappyVM::recursivePointers(HObject* o) {
	if (ObjectUtils::instanceof<HPointer>(o)) {
		HPointer* pointer = dynamic_cast<HPointer*>(o);
		int offset = *static_cast<int*>(pointer->getValue());
		return this->recursivePointers(this->dataStack->lookback(offset));
	} else {
		return o;
	}
}

int HappyVM::getInt() {
	char b1 = this->prog[this->ip++]; // MSB
	char b2 = this->prog[this->ip++];
	char b3 = this->prog[this->ip++];
	char b4 = this->prog[this->ip++]; // LSB

	return b1<<24|b2<<16|b3<<8|b4;
}

void HappyVM::pushObject() {
	unsigned char dataType = this->prog[this->ip-1] & 0xE0; // Only get the top 3 bits.
	dataType >>= 5; // Move down by 5 so they are at the LSB

	HObject* o;

	switch (dataType) {
	case HOP_INT_TYPE: // HInt
		{
			o = new HInt(this->getInt());
		}
		break;
	case HOP_FLOAT_TYPE: // HFloat
		break;
	case HOP_STRING_TYPE: // HString (zero terminated)
		{			
			// A way to reduce the wasted memory problem
			// A singly linked list which allocates small blocks of memory at a time 256 characters per chunk?
			// Once a chunk gets full a new one is created to and pointed to.
			// This can then be traversed to get the data back out of into a single char*.
			// The only problem with this is that is is a HUGE performance destroyed due to the added complexity
			// All of these issues make me want to go for a 2 byte length before the string.
			// That would be simpler, faster and waste less memory, only loose 1 byte of space in program data.
			// Also, as 2 byte length the max size would be 0xffff characters (65535)

			// Max of 65535 characters. (64KB)
			char uLen = this->prog[this->ip++];
			char lLen = this->prog[this->ip++];

			int len = uLen << 8 | lLen;

			char* data = (char*)malloc(sizeof(char) * len); // 64KB buffer.

			for(int i = 0;i < len;i++) {
				data[i] = this->prog[this->ip++];
			}

			o = new HString(data, len);
		}
		break;
	case HOP_ARRAY_TYPE: // HArray
		break;
	case HOP_POINTER_TYPE:
		{
			o = new HPointer(this->getInt()); // Get pointer offset address.
		}
		break;
	}

	this->dataStack->push(o);
}

void HappyVM::stop() {
	//printf("HappyVM: vm was stopped");
	ip = 0; // Reset to the start of the program data
	running = false;
}

void HappyVM::setProgram(char * code) {
	if (running) {
		stop();
	}
	prog = code;
}

HappyVM::~HappyVM(void) {
	delete this->callStack;
	delete this->dataStack;
}