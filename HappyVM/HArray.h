#include "HObject.h"
#include <assert.h>
#include <Windows.h>

#ifndef HARRAY_H
#define HARRAY_H

class HArray : public HObject {
public:
	HArray(int size) : HObject() { this->Array = (HObject**)malloc(sizeof(HObject*) * size); this->Size = size; }
	~HArray() { delete[] this->Array; }

	virtual void operation(HObject* o, HInstruction op) {
		assert(ObjectUtils::instanceof<HArray>(o) && "HArray: non-array type attempting operation with array type.");
		HArray* b = dynamic_cast<HArray*>(o);
		switch (op) {
			case HOP_ADD:
				{
					int newSize = this->Size + b->getSize();
					HObject** concatArray = (HObject**)malloc(sizeof(HObject*) * newSize);
					memcpy(concatArray, this->Array, this->Size*sizeof(HObject*));
					memcpy(concatArray+this->Size, b->getValue(), b->getSize()*sizeof(HObject*));

					delete[] this->Array;
					//delete[] b->getValue(); // Not sure if this is required due to it being deleted after the operation anyway.
					this->Array = concatArray;
					this->Size = newSize;
					break;
				}
			case HOP_SUB:
				{
					// This requires the order to be the same.
					int remCounter = 0;
					for (int j = 0;j < this->getSize();j++) {
						for (int i = 0;i < b->getSize();i++) {
							if (this->getValue(j)->cmp(b->getValue(i)) == HOP_CONDITION_EQUAL) {
								this->setValue(j, nullptr);
								remCounter++;
								break;
							}
						}
					}
					int remainingItems = this->Size - remCounter;

					printf("Item Count: %i, after deleting count: %i\n", this->Size, remainingItems);

					HObject** newArray = (HObject**)malloc(sizeof(HObject*)*remainingItems);
					
					int k = 0;
					for (int i = 0;i < this->Size;i++) {
							if (this->getValue(i) != nullptr)
								newArray[k++] = this->getValue(i);
					}
					
					this->Size = remainingItems;
					delete [] this->Array;
					
					this->Array = newArray;

					break;
				}
			// Not sure what to add in here, more of their functions really. Should + (concatinate the arrays?)
			default: assert(false && "HArray: invalid operation for an array type"); break;
		}
	}

	virtual int cmp(HObject* o) {
		HArray* b = nullptr;
		if (o != nullptr) { // If it is null then operation on itself.
			if (ObjectUtils::instanceof<HArray>(o)==false)
				return HOP_CONDITION_NOT_EQUAL;
			b = dynamic_cast<HArray*>(o);
		}

		// If they don't have the same amount of items then obviously the array's aren't the same.
		if (b->getSize() != this->Size)
			return HOP_CONDITION_NOT_EQUAL;

		// Check that the values are the same.
		for (int i =0;i < this->Size;i++) {
			int cmpResult = this->getValue(i)->cmp(b->getValue(i));
			if (cmpResult != HOP_CONDITION_EQUAL) { // If the values are different then they are not equal.
				return HOP_CONDITION_NOT_EQUAL;
			}
		}

		return HOP_CONDITION_EQUAL;
	}

	void setValue(int index, HObject* obj) {
		this->Array[index] = obj;
	}

	HObject* getValue(int index) {
		return this->Array[index];
	}

	int getSize() {
		return this->Size;
	}

	virtual void* getValue() { return this->Array; }
private:
	HObject** Array;
	int Size;
};

#endif