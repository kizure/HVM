#include "HObject.h"
#include <assert.h>
#include <Windows.h>

#ifndef HARRAY_H
#define HARRAY_H

class HArray : public HObject {
public:
	HArray(int size) : HObject() { this->Array = (HObject*)malloc(sizeof(HObject) * size); this->Size = size; }
	~HArray() { free(this->Array); }
	virtual void operation(HObject* o, HInstruction op) {
		assert(ObjectUtils::instanceof<HArray>(o) && "HArray: non-array type attempting operation with array type.");
		HArray* b = dynamic_cast<HArray*>(o);
		switch (op) {
			// Not sure what to add in here, more of their functions really. Should + (concatinate the arrays?)
			default: assert(false && "HArray: invalid operation for an array type"); break;
		}
	}

	virtual int cmp(HObject* o) {
		assert(false && "HArray: Array comparisons have not yet been implemented yet.");
		// This could get interesting
		return HOP_CONDITION_EQUAL;
	}

	virtual void* getValue() { return this->Array; }
private:
	HObject* Array;
	int Size;
};

#endif