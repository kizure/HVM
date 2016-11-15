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
		//this->pushFloat(value);
		assert(false && "VmEmitter: Failed to push float to binary, not implemented yet");
	}

	int label() {
		return this->ptr;
	}

	void push(char* string, int size) {
		this->pushChar(encodeInstruction(HOP_STRING_TYPE, HOP_PUSH));
		memcpy(this->data+this->ptr, string, size);
		this->ptr += size;
		// This might work, possible off by one error with this->ptr?
	}
	// ===========================================================

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

	void pushInt(int val) {
		pushChar((val>>24)&0xFF);
		pushChar((val>>16)&0xFF);
		pushChar((val>>8)&0xFF);
		pushChar(val&0xFF);
	}

	char encodeInstruction(char datatype, char instruction) {
		return ( (datatype & 7) << 5) + (instruction & 0x1F);
	}

	void pushChar(char c) {
		data[this->ptr++] = c;
	}

	char* complete() {
		return data;
	}

private:
	char* data;
	int ptr;
};

#endif