#include "HObject.h"
#include <assert.h>

#ifndef HINT_H
#define HINT_H

class HInt : public HObject {
public:
	HInt(int value) : Value(value), HObject() {}
	HInt() : Value(0), HObject() {}
	~HInt() {}
	virtual void operation(HObject* o, HInstruction op) {
		HInt* b = nullptr;
		if (o != nullptr) { // If it is null then operation on itself.
			assert(ObjectUtils::instanceof<HInt>(o) && "HInt: non-integer type attempting operation with integer type.");
			b = dynamic_cast<HInt*>(o);
		}

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
		HInt* b = nullptr;
		if (o != nullptr) { // If it is null then operation on itself.
			if (ObjectUtils::instanceof<HInt>(o)==false)
				return HOP_CONDITION_NOT_EQUAL;
			b = dynamic_cast<HInt*>(o);
		}

		if (b->Value == this->Value) {
			return HOP_CONDITION_EQUAL;
		} else if (this->Value < b->Value) {
			return HOP_CONDITION_LESS;
		} else if (this->Value > b->Value) {
			return HOP_CONDITION_GREATER;
		} else {
			assert(false && "HInt: cmp doesn't have that condition defined");
		}

		return HOP_CONDITION_EQUAL; // Default.
	}

	virtual void* getValue() { return &this->Value;}
private:
	int Value;
};

#endif