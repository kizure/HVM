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
#include <cmath>

using namespace std;

HappyVM::HappyVM(void) {
	this->callStack = new Stack<CallFrame>(128);
	this->dataStack = new Stack<HObject>(2048);
	this->initEMethods();
	this->ip = 0;
	this->ops = 0;
}

HappyVM::HappyVM(char* data) {
	this->callStack = new Stack<CallFrame>();
	this->dataStack = new Stack<HObject>();
	this->initEMethods();
	this->ip = 0;
	this->prog = data; // Set the program data for the vm to execute.
	this->ops = 0;
}

#define EXTERN_PRINT 0
#define EXTERN_SIN 1
#define EXTERN_COS 2
#define EXTERN_TAN 3
#define EXTERN_SQRT 4

void HappyVM::initEMethods() {
	this->methods = (emethod*)malloc(sizeof(emethod)*1024); // Up to 1024 external methods for now.

	methods[EXTERN_PRINT] = &HappyVM::vmPrint;
	methods[EXTERN_SIN] = &HappyVM::vmSin;
	methods[EXTERN_COS] = &HappyVM::vmCos;
	methods[EXTERN_TAN] = &HappyVM::vmTan;
	methods[EXTERN_SQRT] = &HappyVM::vmSqrt;
}

void HappyVM::run(void) {
	running = true;
	while(running) { // Only allow 2 instructions to run.
		execute();
	}
}

void HappyVM::execute() {
#if 1
	ops++;
	if (ops % 1000 == 0) { // Only check every thousand ticks or so.
		const long double now = time(0)*1000;

		if (now - this->ltime > 1000) {
			cout << ops << " operations/s" << endl;
			ops = 0;
			this->ltime = now;
		}
	}

#endif

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
			bool wasPointerB = false;
			bool wasPointerA = false;
			int aPointer = 0;
			HObject* b = this->popObjFromStack(&wasPointerB, nullptr);
			HObject* a = this->popObjFromStack(&wasPointerA, &aPointer); // Leave it on stack.
			a->operation(b, (HInstruction)data);
			if (!wasPointerA)
				this->dataStack->push(a);
			else // So instead of pushing a C++ reference to the stack of the result, push a HVM pointer to the stack.
				this->dataStack->push(new HPointer(aPointer));
			
			if (!wasPointerB)
				delete b;
		}
		break;
	case HOP_NOT:
		{
			bool wasPointer=false;
			int pointer=0;
			HObject* a = this->popObjFromStack(&wasPointer, &pointer);
			a->operation(nullptr, HOP_NOT);
			if (wasPointer)
				this->dataStack->push(new HPointer(pointer));
			else
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
			int aPointer=0;
			int bPointer=0;
			bool aIsPointer=false;
			bool bIsPointer=false;

			char cmp = this->prog[this->ip++];
			HObject* b = this->popObjFromStack(&bIsPointer, &bPointer);
			HObject* a = this->popObjFromStack(&aIsPointer, &aPointer);
			int result = a->cmp(b);

			if (aIsPointer)
				this->dataStack->push(new HPointer(aPointer));
			else
				this->dataStack->push(a);

			if (bIsPointer)
				this->dataStack->push(new HPointer(bPointer));
			else
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

			HObject* ary = this->popObjFromStack(nullptr, nullptr);
			HObject* index = this->popObjFromStack(&wasPointer, nullptr);

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

			HObject* ary = this->popObjFromStack(nullptr, nullptr);
			HObject* item = this->popObjFromStack(nullptr, nullptr);
			HObject* index = this->popObjFromStack(&wasPointer, nullptr);

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
	case HOP_ECALL:
		{
			int callId = this->getInt();
			// Lookup id and call method.

			emethod p = this->methods[callId];
			(this->*p)();

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

void HappyVM::vmPrint() {
	bool ptr = false;
	HObject* obj = this->popObjFromStack(&ptr, nullptr);

	if (ObjectUtils::instanceof<HInt>(obj)){
		HInt* var = dynamic_cast<HInt*>(obj);
		printf("%i", *static_cast<int*>(var->getValue()));
	} else if (ObjectUtils::instanceof<HString>(obj)) {
		HString* var = dynamic_cast<HString*>(obj);
		printf("%.*s", var->getLength(), static_cast<char*>(var->getValue()));
	} else if (ObjectUtils::instanceof<HFloat>(obj)) {
		HFloat* var = dynamic_cast<HFloat*>(obj);
		printf("%f", *static_cast<float*>(var->getValue())); // Print the float
	} else if (ObjectUtils::instanceof<HArray>(obj)) {
		HArray* var = dynamic_cast<HArray*>(obj);
		printf("HArray [%i]", var->getValue()); // Printf address of the array.
	}

	if (!ptr)
		delete obj;
}

// EXTERNAL FUNCTIONS //
void HappyVM::vmCos() {
	bool ptr = false;
	HObject* obj = this->popObjFromStack(&ptr, nullptr);

	float val = 0;

	if (ObjectUtils::instanceof<HInt>(obj)){
		HInt* var = dynamic_cast<HInt*>(obj);
		val = *static_cast<int*>(var->getValue());
	} else if (ObjectUtils::instanceof<HFloat>(obj)) {
		HFloat* var = dynamic_cast<HFloat*>(obj);
		val = *static_cast<float*>(var->getValue());
	} else{
		printf("HappyVM Warning: Cos function cannot accept this data type, int and float only.");
	}

	// Push result onto stack for cos function
	this->dataStack->push(new HFloat(cosf(val)));

	if (!ptr)
		delete obj;
}

void HappyVM::vmSin() {
	bool ptr = false;
	HObject* obj = this->popObjFromStack(&ptr, nullptr);

	float val = 0;

	if (ObjectUtils::instanceof<HInt>(obj)){
		HInt* var = dynamic_cast<HInt*>(obj);
		val = *static_cast<int*>(var->getValue());
	} else if (ObjectUtils::instanceof<HFloat>(obj)) {
		HFloat* var = dynamic_cast<HFloat*>(obj);
		val = *static_cast<float*>(var->getValue());
	} else{
		printf("HappyVM Warning: Sin function cannot accept this data type, int and float only.");
	}

	// Push result onto stack for sin function
	this->dataStack->push(new HFloat(sinf(val)));

	if (!ptr)
		delete obj;
}

void HappyVM::vmTan() {
	bool ptr = false;
	HObject* obj = this->popObjFromStack(&ptr, nullptr);

	float val = 0;

	if (ObjectUtils::instanceof<HInt>(obj)){
		HInt* var = dynamic_cast<HInt*>(obj);
		val = *static_cast<int*>(var->getValue());
	} else if (ObjectUtils::instanceof<HFloat>(obj)) {
		HFloat* var = dynamic_cast<HFloat*>(obj);
		val = *static_cast<float*>(var->getValue());
	} else{
		printf("HappyVM Warning: Tan function cannot accept this data type, int and float only.");
	}

	// Push result onto stack for tan function
	this->dataStack->push(new HFloat(tanf(val)));

	if (!ptr)
		delete obj;
}

void HappyVM::vmSqrt() {
	bool ptr = false;
	HObject* obj = this->popObjFromStack(&ptr, nullptr);

	float val = 0;

	if (ObjectUtils::instanceof<HInt>(obj)){
		HInt* var = dynamic_cast<HInt*>(obj);
		val = *static_cast<int*>(var->getValue());
	} else if (ObjectUtils::instanceof<HFloat>(obj)) {
		HFloat* var = dynamic_cast<HFloat*>(obj);
		val = *static_cast<float*>(var->getValue());
	} else{
		printf("HappyVM Warning: Sqrt function cannot accept this data type, int and float only.");
	}

	// Push result onto stack for tan function
	this->dataStack->push(new HFloat(sqrtf(val)));

	if (!ptr)
		delete obj;
}

// END OF EXTERNAL FUNCTIONS //

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
		case HOP_ARRAY_TYPE:
			{
				int len = this->getUShort();
				for (int i = 0;i < len;i++) {
					char dataType = this->prog[this->ip++];
					this->skipInstruction(dataType);
				}
				break;
			}
		case HOP_STRING_TYPE: 
			{
				this->ip+=this->getUShort();
				break;
			}
		}
	} 
	else if (data==HOP_TEST)
		this->ip+=1;
	else if (data == HOP_JMP || data == HOP_CALL || data == HOP_ECALL)
		this->ip+=4;
}

void HappyVM::skipInstruction(char dataType) {
	switch (dataType) {
		case HOP_INT_TYPE:
		case HOP_FLOAT_TYPE:
		case HOP_POINTER_TYPE:
			this->ip += 4;
			break;
		case HOP_ARRAY_TYPE: // Needs to be implemented.
			{
				int len = this->getUShort();
				for (int i = 0;i < len;i++) {
					char dataType = this->prog[this->ip++];
					this->skipInstruction(dataType);
				}
			}
			assert(false && "skip instruction array data type not implemented yet");
			break;
		case HOP_STRING_TYPE: 
			{	
				this->ip+=this->getUShort();
			}
			break;
	}
}

inline int HappyVM::getUShort() {
	char uLen = this->prog[this->ip++];
	char lLen = this->prog[this->ip++];
	return uLen << 8 | lLen;
}

HObject* HappyVM::popObjFromStack(bool* isPtr, int* pointerAddress) {
	HObject* obj = this->dataStack->pop();

	// Check if it is a pointer, if so 
	if (ObjectUtils::instanceof<HPointer>(obj)) {
		HPointer* pointer = dynamic_cast<HPointer*>(obj);
		int address = *static_cast<int*>(pointer->getValue());
		if (pointerAddress!=nullptr)
			*pointerAddress = address;
		if (isPtr != nullptr) // Was a pointer.
			*isPtr = true;
		// printf("Pointer Address: %i\n", offset);
		obj = this->recursivePointers(this->dataStack->lookat(address));
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
				int len = this->getUShort();

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
			
			int len = this->getUShort();

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