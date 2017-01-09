#include "HObject.h"
#include <Windows.h>
#include <assert.h>

#ifndef STACK_H
#define STACK_H

template<class T>
class Stack {
private:
	T** stack;
	int pos;
	int size;

public:
	void push(T* obj) {
		assert(this->pos < (this->size - 1) && "HappyVM: Stack overflow exception, push failed");
		this->stack[pos++] = obj;
	}

	T* pop(void) {
		assert(this->pos > 0 && "HappyVM: Stack underflow, pop failed");
		return this->stack[--pos];
	}

	T* peek(void) {
		assert(this->pos > 0 && "HappyVM: Stack underflow, peek failed");
		return this->stack[pos-1];
	}

	T* lookat(int address) {
		return this->stack[address];
	}

	int getSize() {
		return this->pos;
	}

	int getPos() {
		return this->pos;
	}

	Stack(int size) {
		this->pos = 0;
		this->size = size;
		this->stack = (T**)malloc(this->size * sizeof(T*));
	}
	
	Stack() {
		this->pos = 0;
		this->size = 1024;
		this->stack = (T**)malloc(this->size * sizeof(T*));
	}

	~Stack(void) {
		free(stack); // Free the memory for the stack.
	}
};

#endif