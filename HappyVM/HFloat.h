#include "HObject.h"
#include <assert.h>

#ifndef HFLOAT_H
#define HFLOAT_H

class HFloat : public HObject {
public:
	HFloat(float value) : Value(value), HObject() {}
	HFloat() : Value(0), HObject() {}
	~HFloat() {}
	virtual void operation(HObject* o, HInstruction op) {
		assert(ObjectUtils::instanceof<HFloat>(o) && "HFloat: non-float type attempting operation with float type.");
		HFloat* b = dynamic_cast<HFloat*>(o);

		switch (op) {
			case HOP_ADD: this->Value += b->Value; break;
			case HOP_SUB: this->Value -= b->Value; break;
			case HOP_MUL: this->Value *= b->Value; break;
			case HOP_DIV: this->Value /= b->Value; break;
			default: assert(false && "HFloat: invalid operation for a float"); break;
		}
	}

	virtual int cmp(HObject* o) {
		HFloat* b = nullptr;
		if (o != nullptr) { // If it is null then operation on itself.
			assert(ObjectUtils::instanceof<HFloat>(o) && "HFloat: non-float type attempting operation with integer type.");
			b = dynamic_cast<HFloat*>(o);
		}

		if (b->Value == this->Value) {
			return HOP_CONDITION_EQUAL;
		} else if (this->Value < b->Value) {
			return HOP_CONDITION_LESS;
		} else if (this->Value > b->Value) {
			return HOP_CONDITION_GREATER;
		} else {
			assert(false && "HFloat: cmp doesn't have that condition defined");
		}

		return HOP_CONDITION_EQUAL; // Default.
	}

	virtual void* getValue() { return &this->Value;}
private:
	float Value;
};


#endif