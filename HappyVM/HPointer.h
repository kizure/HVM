#include "HObject.h"
#include <assert.h>

#ifndef HPOINTER_H
#define HPOINTER_H

class HPointer : public HObject {
public:
	HPointer(int value) : Value(value), HObject() {}
	HPointer() : Value(0), HObject() {}
	~HPointer() {}
	virtual void operation(HObject* o, HInstruction op) {
		HPointer* b = nullptr;
		if (o != nullptr) { // If it is null then operation on itself.
			assert(ObjectUtils::instanceof<HPointer>(o) && "HInt: non-integer type attempting operation with integer type.");
			b = dynamic_cast<HPointer*>(o);
		}

		// Not sure if any of this will be required as it will be derefenced before here?
		switch (op) {
			case HOP_ADD: this->Value += b->Value; break;
			case HOP_SUB: this->Value -= b->Value; break;
			case HOP_MUL: this->Value *= b->Value; break;
			case HOP_DIV: this->Value /= b->Value; break;
			case HOP_AND: this->Value &= b->Value; break;
			case HOP_OR: this->Value |= b->Value; break;
			case HOP_XOR: this->Value ^= b->Value; break;
			case HOP_MOD: this->Value %= b->Value; break;
			case HOP_NOT: this->Value = !this->Value; break;
			case HOP_SHR: this->Value >>= b->Value; break;
			case HOP_SHL: this->Value <<= b->Value; break;
			default: assert(false && "HInt: invalid operation for an integer"); break;
		}
	}

	virtual int cmp(HObject* o) {
		assert(false && "HPointer: Trying to compare pointers, why?");
		return HOP_CONDITION_EQUAL;
	}

	virtual void* getValue() { return &this->Value;}
private:
	int Value;
};

#endif