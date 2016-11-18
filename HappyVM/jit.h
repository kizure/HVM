#include <Windows.h>
#include "x86emitter.h"

typedef int (*JitFunc)(void*);

void testJit() {
	int size = 64;

	// test();

	// x64 basics https://software.intel.com/en-us/articles/introduction-to-x64-assembly
	
	// page 7 - https://www.cs.cmu.edu/~fp/courses/15213-s07/misc/asm64-handout.pdf
	// https://defuse.ca/online-x86-assembler.htm#disassembly

	// Useful for figuring out registers http://vikaskumar.org/amd64/

	void* memory = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
	
	X86Emitter* emitter = new X86Emitter();
	emitter->genInstruction(0x80, 0, 1, new Operand((unsigned char)X86_REGISTER_32_EBX, X86_REGISTER_32_EAX, 2, 0xffff), new Operand(0xff));

	// STOP RUNNING AT THE MOMENT
	return;

	// Copy into prog data into memory
	memcpy(memory, emitter->buffer, size);
	
	// Make executable
	DWORD oldProtect;
	VirtualProtect(memory, size, PAGE_EXECUTE_READ, &oldProtect);

	JitFunc func = (JitFunc)memory;

	char* PointerTest = (char*)malloc(sizeof(char) * 16);
	memset(PointerTest, 0, sizeof(char) * 16);
	PointerTest[1] = 12; // Some data for testing.

	long long result = func(PointerTest);
	cout << "Result: " << result << endl;

	// Free the memory
	VirtualFree(memory,0, MEM_RELEASE);

	system("pause");
}