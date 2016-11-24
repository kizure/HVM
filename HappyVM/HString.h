#include "HObject.h"
#include <assert.h>
#include <Windows.h>

#ifndef HSTRING_H
#define HSTRING_H

class HString : public HObject {
public:
	HString(char* string, int size) : HObject(), Length(size), Chars(string) { }
	HString(int size) : HObject(), Length(size) { this->Chars = (char*)malloc(this->Length * sizeof(char)); }
	~HString() { free(this->Chars); }
	virtual void operation(HObject* o, HInstruction op) {
		assert(ObjectUtils::instanceof<HString>(o) && "HString: non-string type attempting operation with string type.");
		HString* b = dynamic_cast<HString*>(o);

		switch (op) {
		case HOP_ADD:
			// Concatanate the two strings together. (This will require the string size increasing.)
			// Could be optimized through storing the pointer instead of copying the whole array, allocating, freeing, copying, etc
			{
				int newSize = this->Length + b->Length;
				char* newString = (char*)malloc(newSize * sizeof(char));
				memcpy(newString, this->Chars, (this->Length * sizeof(char) )); // Copy first string in
				memcpy(newString + ( this->Length * sizeof(char) ), b->Chars, ( b->Length * sizeof(char) )); // Copy last string in.
				free(this->Chars);
				this->Chars = newString;
				this->Length = newSize;
			}
			break;
		case HOP_SUB:
			{
				int index = -1;
				while (( index = ObjectUtils::contains(this->Chars, this->Length, b->Chars, b->Length) ) != -1) {
					// Need to handle if the index is 0 as index-1 will be -1.
					// TODO: the code needs to be able to handle if sizeof(char) is different to 1.

					// Tests to do:
					// If the string is in the middle
					// If the string is at the start.
					// If their are multiple of the same string to remove.

					int newSize = this->Length - b->Length;
					char* newString = (char*)malloc(newSize * sizeof(char));
					
					if (index == 0) { // Start
						memcpy(newString, this->Chars+b->Length, newSize * sizeof(char));
					} else if (index + b->Length == this->Length) { // End
						memcpy(newString, this->Chars, newSize * sizeof(char));
					} else { // Somewhere between
						memcpy(newString, this->Chars, index-1); // Copy in upto string
						memcpy(newString+(index-1), this->Chars+index, b->Length * sizeof(char));
					}
					
					free(this->Chars);
					this->Chars = newString;
					this->Length = newSize;
				}
			}
			// Check this contains the other string and then remove all instances of it from this string.
			break;
		}
	}

	virtual int cmp(HObject* o) {			
		HString* b = nullptr;
		if (o != nullptr) { // If it is null then operation on itself.
			if (ObjectUtils::instanceof<HString>(o)==false)
				return HOP_CONDITION_NOT_EQUAL;
			b = dynamic_cast<HString*>(o);
		}

		if (b->Length!=this->Length)
			return HOP_CONDITION_NOT_EQUAL;
		
		for (int i = 0;i < b->Length;i++) {
			if (b->Chars[i] != this->Chars[i])
				return HOP_CONDITION_NOT_EQUAL;
		}

		return HOP_CONDITION_EQUAL;
	}

	int getLength() {
		return this->Length;
	}

	virtual void* getValue() { return this->Chars;}
private:
	char* Chars;
	int Length;
};

#endif