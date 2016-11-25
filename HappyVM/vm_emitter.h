#ifndef VM_EMITTER_H
#define VM_EMITTER_H

class VmEmitter {
public:
	~VmEmitter() {
		if (data!=nullptr)
			free(data);
	}

	VmEmitter(int size) {
		this->data = (char*)malloc(size * sizeof(char));
		assert(this->data != nullptr && "VmEmitter: Failed to allocate program space, ran out of memory!");
		this->ptr = 0;
	}

	// ===========================================================
	void push(int value) {
		this->pushChar(encodeInstruction(HOP_INT_TYPE, HOP_PUSH));
		this->pushInt(value);
	}

	void pushPtr(int ptr) {
		this->pushChar(encodeInstruction(HOP_POINTER_TYPE, HOP_PUSH));
		this->pushInt(ptr);
	}

	void push(float value) {
		this->pushChar(encodeInstruction(HOP_FLOAT_TYPE, HOP_PUSH));
		this->pushFloat(value);
		//assert(false && "VmEmitter: Failed to push float to binary, not implemented yet");
	}

	void push(HArray* harray) {
		this->pushChar(encodeInstruction(HOP_ARRAY_TYPE, HOP_PUSH));
		// Sizeof array 32 bit.
		int size = harray->getSize();
		this->pushInt(size);
		
		// Then push each item onto the array.
		for (int i= 0;i < harray->getSize();i++) {
			HObject* o = harray->getValue(i);
			if (ObjectUtils::instanceof<HFloat>(o))	{ // Float
				this->pushChar(HOP_FLOAT_TYPE);
				this->pushFloat(*static_cast<float*>(dynamic_cast<HFloat*>(o)->getValue()));
			} else if (ObjectUtils::instanceof<HString>(o)) { // String
				HString* s = dynamic_cast<HString*>(o);
				this->pushChar((s->getLength()>>8)&0xff);
				this->pushChar(s->getLength()&0xff);
				memcpy(this->data + this->ptr, s->getValue(), size*sizeof(char));
				this->ptr += size;
			} else if (ObjectUtils::instanceof<HInt>(o)) { // Ints
				this->pushChar(HOP_INT_TYPE);
				this->pushInt(*static_cast<int*>(dynamic_cast<HInt*>(o)->getValue()));
			} else if (ObjectUtils::instanceof<HPointer>(o)) { // Pointers
				this->pushChar(HOP_POINTER_TYPE);
				this->pushInt(*static_cast<int*>(dynamic_cast<HPointer*>(o)->getValue()));
			} else if (ObjectUtils::instanceof<HArray>(o)) { // Arrays
				this->pushChar(HOP_ARRAY_TYPE);
				this->push(dynamic_cast<HArray*>(o));
			} else {
				assert(false && "Emitting an unknown type in an array");
			}
		}
	}

	void push(char* string, int size) {
		assert(size<=0xffff && "Cannot push string of that size onto the stack, length > 65535");
		this->pushChar(encodeInstruction(HOP_STRING_TYPE, HOP_PUSH));
		this->pushChar((size>>8)&0xff);
		this->pushChar(size&0xff);
		memcpy(this->data + this->ptr, string, size);
		this->ptr += size;
		// This might work, possible off by one error with this->ptr?
	}
	// ===========================================================

	int label() {
		return this->ptr;
	}

	void pop() {
		this->pushChar(HOP_POP);
	}
	
	void end() {
		this->pushChar(HOP_END); // end instruction.
	}
	
	void add() {
		this->pushChar(HOP_ADD);
	}

	void sub() {
		this->pushChar(HOP_SUB);
	}

	void breakpoint() {
		this->pushChar(HOP_BRKPT);
	}

	void mul() {
		this->pushChar(HOP_MUL);
	}

	void div() {
		this->pushChar(HOP_DIV);
	}

	void mod() {
		this->pushChar(HOP_MOD);
	}

	void or() {
		this->pushChar(HOP_OR);
	}

	void xor() {
		this->pushChar(HOP_XOR);
	}

	void not() {
		this->pushChar(HOP_NOT);
	}

	void and() {
		this->pushChar(HOP_AND);
	}

	// ===============================================
	int jmp(int address) {
		int asmPos = this->ptr;
		this->pushChar(HOP_JMP);
		this->pushInt(address);
		return asmPos; // Return the address where the jmp is placed into program memory.
	}

	void patchJmp(int jmpAddress, int newAddress) {
		int temp = this->ptr;
		this->ptr = jmpAddress;
		this->jmp(newAddress);
		this->ptr = temp;
	}

	// ===============================================

	// ===============================================
	void call(int address) {
		this->pushChar(HOP_CALL);
		this->pushInt(address);
	}

	void ecall(int address) {
		this->pushChar(HOP_ECALL);
		this->pushInt(address);
	}

	// ===============================================

	void ret() {
		this->pushChar(HOP_RET);
	}

	// ===============================================
	void test(int condition) {
		this->pushChar(HOP_TEST);
		this->pushChar((char)condition);
	}
	// ===============================================

	void shl() {
		this->pushChar(HOP_SHL);
	}

	void shr() {
		this->pushChar(HOP_SHR);
	}

	void lditm() {
		this->pushChar(HOP_LDITM);
	}

	void stitm() {
		this->pushChar(HOP_STITM);
	}

	char* complete() {
		return data;
	}

private:
	
	void pushInt(int val) {
		pushChar((val>>24)&0xFF);
		pushChar((val>>16)&0xFF);
		pushChar((val>>8)&0xFF);
		pushChar(val&0xFF);
	}

	void pushFloat(float f) {
		FloatUnion uni;
		uni.value = f;
		pushChar(uni.buffer[0]);
		pushChar(uni.buffer[1]);
		pushChar(uni.buffer[2]);
		pushChar(uni.buffer[3]);
	}

	char encodeInstruction(char datatype, char instruction) {
		return ( (datatype & 7) << 5) + (instruction & 0x1F);
	}

	void pushChar(char c) {
		data[this->ptr++] = c;
	}

	char* data;
	int ptr;
};

#endif