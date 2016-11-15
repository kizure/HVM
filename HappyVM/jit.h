#include <Windows.h>
#include "x64emitter.h"

typedef long long (*JitFunc)(void*);
int test(int);

void testJit() {
	int size = 64;

	// x64 basics https://software.intel.com/en-us/articles/introduction-to-x64-assembly
	
	// page 7 - https://www.cs.cmu.edu/~fp/courses/15213-s07/misc/asm64-handout.pdf
	// https://defuse.ca/online-x86-assembler.htm#disassembly

	// Useful for figuring out registers http://vikaskumar.org/amd64/

	// Make sure to compile under x64 else it won't work correctly.

	/*unsigned char code[] = {
		0x48, 0xc7, 0xc0, 0x2a, 0x0, 0x0, 0x0, // mov 42, rax
		0xc3 // ret
	};*/

	/*unsigned char code[] = {
		0x48, 0x89, 0xc8, // mov rax, rcx
		0x48, 0x83, 0xc0, 0x02, // add rax, 2
		0xc3 // ret
	};*/
	
	// Currently doesn't do much.
	X64Emitter emitter = *new X64Emitter();
	emitter.movr64rm64(REG_RAX, new MEM_REF(REG_INT_ARG1, 1, 1));
	emitter.ret();

	void* memory = VirtualAlloc(NULL, size, MEM_COMMIT, PAGE_READWRITE);
	
	// Copy into prog data into memory
	memcpy(memory, emitter.buffer, size);
	
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