#include <stdio.h>
#include <string.h>
#include <windows.h>

int main(int argc, char **argv)
{
	unsigned char code[] = {
		0x55,
		0x48, 0x89, 0xe5,
		0x89, 0x7d, 0xfc,
		0x89, 0x75, 0xf8,
		0x8b, 0x75, 0xfc,
		0x0f, 0xaf, 0x75, 0xf8,
		0x89, 0xf0,
		0x5d,
		0xc3,
	};
	
	void *mem = VirtualAlloc(NULL, sizeof(code), MEM_COMMIT, PAGE_READWRITE); // Make read/write so we can put stuff into the buffer.
	
	memcpy(mem, code, sizeof(code)); // Copy the code into the memory.
	
	DWORD old;
	VirtualProtect(mem, sizeof(code), PAGE_EXECUTE_READ, &old); // Change the protection of the memory to executable from read / write.
	
	int (*func)() = mem;
	
	printf("%d * %d = %d\n", 5, 11, func(5, 11));
	
	VirtualFree(mem, 0, MEM_RELEASE); // Free the memory.
	system("pause");
	return 0;
}