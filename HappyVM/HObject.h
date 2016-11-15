#ifndef HOBJECT_H
#define HOBJECT_H

#include <iostream>

using namespace std;

// Also test might need to be made less general (JPZ, JNZ, etc)
typedef enum {
	HOP_ADD=0, // DONE
	HOP_SUB=1, // DONE
	HOP_MUL=2, // DONE
	HOP_DIV=3, // DONE
	HOP_MOD=4, // DONE
	HOP_OR=5, // DONE
	HOP_XOR=6, // DONE
	HOP_NOT=7, // DONE
	HOP_AND=8, // DONE
	HOP_JMP=9, // DONE
	HOP_CALL=10, // DONE (not tested)
	HOP_RET=11, // DONE (not tested)
	HOP_TEST=12, // DONE
	HOP_PUSH=13, // DONE
	HOP_POP=17, // DONE
	HOP_END=14, // DONE
	HOP_SHL=15, // DONE
	HOP_SHR=16, // DONE
	HOP_BRKPT=18, // DONE
} HInstruction;

// Used to change floats from bytes to actual floating point values. (endianness could be an issue)
union FloatUnion {
	float value;
	char buffer[sizeof(float)];
};

class ObjectUtils {
public:
	static bool IsLittleEndian() {
		short int word = 0x0001;
		char* byte = (char*)&word;
		return (byte[0] ? true : false);
	}

	template<typename Base, typename T>
	static inline bool instanceof(const T *ptr) {
		return dynamic_cast<const Base*>(ptr) != nullptr;
	}

	static int contains(char* in, int iLen, char* toFind, int fLen) {
		int j = 0;
		int start = 0;
		bool started = false;
		for (int i = 0;i < iLen;i++) {
			if (in[i] == toFind[j]) {
				if (started==false){
					start = i;
					started = true;
				}
				j++;
			} else {
				j = 0; // Reset
				started = false;
			}
			if (j == fLen) 
				return start; // Index of the contains.
		}
		return -1; // Not found
	}
};

class HObject {
//private:
	//bool markFlag;
public:
	HObject();
	~HObject(){}
	//bool isMarked();
	//void mark();
	//void unmark();
	virtual void operation(HObject* o, HInstruction op) = 0;
	virtual int cmp(HObject* o) = 0;
	virtual void* getValue() = 0;
};

#endif