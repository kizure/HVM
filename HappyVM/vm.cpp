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
			bool wasPointer = false;
			HObject* b = this->popObjFromStack(&wasPointer);
			HObject* a = this->popObjFromStack(nullptr); // Leave it on stack.
			a->operation(b, (HInstruction)data);
			this->dataStack->push(a);
			printf("Pointer? : %i\n", wasPointer);
			if (!wasPointer)
				delete b; // Cleanup as b no longer exists UNLESS it was a pointer.
		}
		break;
	case HOP_NOT:
		{
			HObject* a = this->popObjFromStack(nullptr);
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
			HObject* o = this->dataStack->pop();
			delete o;
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
			HObject* b = this->popObjFromStack(nullptr);
			HObject* a = this->popObjFromStack(nullptr);
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
	case HOP_LDITM: // Load Item from array onto stack
		{
			// Push Index (HInt)
			// Push Array (or pointer)

			bool wasPointer = false;

			HObject* ary = this->popObjFromStack(nullptr);
			HObject* index = this->popObjFromStack(&wasPointer);

			if (ObjectUtils::instanceof<HInt>(index) && ObjectUtils::instanceof<HArray>(ary)) {
				HInt* idx = dynamic_cast<HInt*>(index);
				HArray* nary = dynamic_cast<HArray*>(ary);

				this->dataStack->push(nary->getValue(*static_cast<int*>(idx->getValue()))); // Push array item back onto stack
			} else {
				assert(false && "Load Item instruction failed, requires integer for index and array type for array");
			}

			if (!wasPointer)
				delete index;
			break;
		}
	case HOP_STITM:
		{
			// Push index
			// Push item (to put onto array)
			// Push Array (or pointer)
			
			bool wasPointer = false;

			HObject* ary = this->popObjFromStack(nullptr);
			HObject* item = this->popObjFromStack(nullptr);
			HObject* index = this->popObjFromStack(&wasPointer);

			if (ObjectUtils::instanceof<HInt>(index) && ObjectUtils::instanceof<HArray>(ary)) {
				HInt* idx = dynamic_cast<HInt*>(index);
				HArray* nary = dynamic_cast<HArray*>(ary);
				nary->setValue(*static_cast<int*>(idx->getValue()), item);
			} else {
				assert(false && "Store Item instruction failed, requires integer for index and array type for array");
			}

			if (!wasPointer)
				delete index;

			break;
		}
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

HObject* HappyVM::popObjFromStack(bool* pointer) {
	HObject* obj = this->dataStack->pop();

	// Check if it is a pointer, if so 
	if (ObjectUtils::instanceof<HPointer>(obj)) {
		if (pointer !=nullptr) // Was a pointer.
			*pointer = true;
		HPointer* pointer = dynamic_cast<HPointer*>(obj);
		int offset = *static_cast<int*>(pointer->getValue());
		printf("Pointer Offset: %i\n", offset);
		obj = this->recursivePointers(this->dataStack->lookat(offset));
		delete pointer;
	}

	return obj;
}

HObject* HappyVM::recursivePointers(HObject* o) {
	if (ObjectUtils::instanceof<HPointer>(o)) {
		HPointer* pointer = dynamic_cast<HPointer*>(o);
		int offset = *static_cast<int*>(pointer->getValue());
		return this->recursivePointers(this->dataStack->lookat(offset));
	} else {
		return o;
	}
}

HObject* HappyVM::getArray() {
	// Binary layout
	// int for size of array
	// For each item in array
	//		uint8 for type
	//		the data for that type.
	// next item

	int length = this->getInt();
	HArray* result = new HArray(length);
	for (int i = 0;i < length;i++) {
		unsigned char type = this->prog[this->ip++];
		switch (type) {
		case HOP_INT_TYPE: // HInt
			result->setValue(i, new HInt(this->getInt()));
			break;
		case HOP_FLOAT_TYPE: // HFloat
			result->setValue(i, new HFloat(this->getFloat()));
			break;
		case HOP_STRING_TYPE: // HString (zero terminated)
			{			
				char uLen = this->prog[this->ip++];
				char lLen = this->prog[this->ip++];

				int len = uLen << 8 | lLen;

				char* data = (char*)malloc(sizeof(char) * len); // 64KB buffer.

				for(int i = 0;i < len;i++) {
					data[i] = this->prog[this->ip++];
				}

				result->setValue(i, new HString(data, len));
			}
			break;
		case HOP_ARRAY_TYPE: 
			result->setValue(i, this->getArray());
			break;
		case HOP_POINTER_TYPE:
			result->setValue(i, new HPointer(this->getInt())); // Get pointer offset address.
			break;
		}
	}

	return result;
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
		{
			o = new HFloat(this->getFloat());
		}
		break;
	case HOP_STRING_TYPE: // HString (zero terminated)
		{			
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
		o = this->getArray();
		break;
	case HOP_POINTER_TYPE:
		{
			o = new HPointer(this->getInt()); // Get pointer offset address.
		}
		break;
	}

	this->dataStack->push(o);
}

int HappyVM::getInt() {
	char b1 = this->prog[this->ip++]; // MSB
	char b2 = this->prog[this->ip++];
	char b3 = this->prog[this->ip++];
	char b4 = this->prog[this->ip++]; // LSB

	return b1<<24|b2<<16|b3<<8|b4;
}

float HappyVM::getFloat() {
	char b1 = this->prog[this->ip++]; // MSB
	char b2 = this->prog[this->ip++];
	char b3 = this->prog[this->ip++];
	char b4 = this->prog[this->ip++]; // LSB
	FloatUnion f;
	f.buffer[0] = b1;
	f.buffer[1] = b2;
	f.buffer[2] = b3;
	f.buffer[3] = b4;
	return f.value;
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