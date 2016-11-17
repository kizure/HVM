#include <stdint.h>

#ifndef X64EMITTER_H
#define X64EMITTER_H

/****
	X64 Instruction Encoding
	http://wiki.osdev.org/X86-64_Instruction_Encoding#General_Overview
	https://www-user.tu-chemnitz.de/~heha/viewchm.php/hs/x86.chm/x64.htm#Opcode
	http://www.intel.co.uk/content/dam/www/public/us/en/documents/manuals/64-ia-32-architectures-software-developer-vol-2a-manual.pdf
	https://support.amd.com/TechDocs/24594.pdf (page 92 is useful (rq iq) )
****/

/*
	General Instruction Format:
		Instruction Prefixed (up to 4 bytes, optional)
		Opcode (1,2 or 3 bytes)
		ModR/M (1 byte, if needed)
		SIB	(1 byte, if needed)
		Displacement(1, 2 or 4 bytes or none)
		Immediate(1, 2 or 4 bytes or none)

	ModR/M Format:
		R/M - bit 0 to 2
		Reg/Op - bit 3 to 5
		Mod - bit 6 to 7
	SIB Format:
		Base - bit 0 to 2
		Index - bit 3 to 5
		Scale - bit 6 to 7

	SIB has to be used when doing memory based operands. (REX X and B can extend stuff.)

	When MOD == 0 R/M == 100 it has to use the SIB byte for the register.
	When MOD == 01 R/M == 100 SIB+disp8
	When MOD == 10 R/M == 100 SIB+disp32

	For each instruction, one prefix may be used from each group and cab be placed in any order.
*/

// Programming helpers.
// As immediate values are varying length (1, 2 or 4 bytes) they have to have a seperate function.
#define X64_REGISTER	1<<8
#define X64_MEM_REF		2<<8

#define X64_CHECK_ARG_TYPE(REG, TEST) ( (REG >> 8) == (TEST >> 8) ? true : false )

// General purpose registers 64 bit.
#define REG_RAX 0
#define REG_RCX 1
#define REG_RDX 2
#define REG_RBX 3
#define REG_RSP 4
#define REG_RBP 5
#define REG_RSI 6
#define REG_RDI 7
#define REG_R8	8
#define REG_R9	9
#define REG_R10	10
#define REG_R11	11
#define REG_R12	12
#define REG_R13	13
#define REG_R14	14
#define REG_R15	15

// 64 bit floating point registers.
#define REG_MMX0 0
#define REG_MMX1 1
#define REG_MMX2 2
#define REG_MMX3 3
#define REG_MMX4 4
#define REG_MMX5 5
#define REG_MMX6 6
#define REG_MMX7 7

// 128 Bit media registers (Streaming SIMD registers)
#define REG_XMM0 0
#define REG_XMM1 1
#define REG_XMM2 2
#define REG_XMM3 3
#define REG_XMM4 4
#define REG_XMM5 5
#define REG_XMM6 6
#define REG_XMM7 7
#define REG_XMM8 8
#define REG_XMM9 9
#define REG_XMM10 10
#define REG_XMM11 11
#define REG_XMM12 12
#define REG_XMM13 13
#define REG_XMM14 14
#define REG_XMM15 15

#define REG_INT_ARG1 REG_RCX
#define REG_INT_ARG2 REG_RDX
#define REG_INT_ARG3 REG_R8
#define REG_INT_ARG4 REG_R9

#define REG_FLOAT_ARG1 REG_XMM0
#define REG_FLOAT_ARG2 REG_XMM1
#define REG_FLOAT_ARG3 REG_XMM2
#define REG_FLOAT_ARG4 REG_XMM3

#define REG_INT_RET REG_RAX
#define REG_FLOAT_RET REG_XMM0

// Group 1, Lock and repeat group 1
#define PREFIX_LOCK 0xF0
#define PREFIX_REPNE_REPNZ 0xF2		// (only used with string instructions)
#define PREFIX_REPE_REPZ 0xF3		// (only used with string instructions)
#define PREFIX_SIMD 0xf3			// Streaming SIMD Extensions prefix.

// Segment override group 2
#define PREFIX_CS 0x2E
#define PREFIX_SS 0x36
#define PREFIX_DS 0x3E
#define PREFIX_ES 0x26
#define PREFIX_FS 0x64
#define PREFIX_GS 0x65
#define PREFIX_BRANCH_NOT_TAKEN 0x2e
#define PREFIX_BRANCH_TAKEN 0x3e

// Operand size override group 3
#define PREFIX_OPERAND_SIZE_OVERRIDE 0x66

// Address size override group 4
#define PREFIX_ADDRESS_SIZE_OVERRIDE 0x67

// Used for referencing memory.
// If this is used, then MOD = 01 if offset<=0xff. disp8
// MOD = 10 if offset>0xff disp16
// Possible ways to implement
// for functions containing rm64 add another function with MEM_REF as the arg
struct MEM_REF {
	int32_t offset; // offset
	uint8_t reg; // register
	uint8_t scale; // scale.
	bool is_mem; // As this could be used for non-memory register access.

	// Used for default [register] memory access.
	MEM_REF(uint8_t reg) : reg(reg), offset(reg), scale(reg), is_mem(false) {}
	
	// Used for [register*scale+offset] memory access
	MEM_REF(uint8_t reg, uint8_t scale, int32_t offset) : reg(reg), scale(scale), offset(offset), is_mem(true) {}
	
	// Default constructor.
	MEM_REF() : offset(0), reg(0), scale(0) {}
};

// Used to make passing data between functions easier, less arguments to make it easier to maintain.
struct INSTRUCTION {
	char* PREFIX;	// Some opcodes have prefixes.
	char REX;	// Some opcodes don't have a rex.
	char OPCODE; // Some opcodes have more than one value.
	char MODRM;
	char SIB;	// Some opcodes have SIBs
	char* EXTRAS;	// Some opcodes have extra data like immediate values.
	int ExtrasCount;

	// ------------
	// Not used yet.
	int PrefixCount;
	bool RexUsed;
	
	INSTRUCTION() : PREFIX(nullptr), REX(0), OPCODE(0), MODRM(0), SIB(0), EXTRAS(nullptr), ExtrasCount(0) {}
	INSTRUCTION(char rex, char opcode) : PREFIX(nullptr), REX(rex), OPCODE(opcode), MODRM(0), SIB(0), EXTRAS(nullptr), ExtrasCount(0) {}
};

class X64Emitter {
public:
	// So the weird issue with r12 and r13 requiring extra bytes is beginning to make sense
	// In the MODRM byte there is a opcode which defines the SIB byte
	// This means certain registers collide with that code.
	// So they have to use the SIB byte with 0 base and scale of 1
	// This is why r12 and r13 are weird registers but r14 and r15 work fine.

	void ret();
	
	// Mem_ref testing stuff.
	void sib(INSTRUCTION* instr, MEM_REF* regmem);
	void movr64rm64(int reg, MEM_REF* regmem);
	void movrm64r64(MEM_REF* regmem, int reg);
	bool r64rm64(int reg, MEM_REF* regmem, INSTRUCTION* instr);			// Seems to work as expected for both rm64r64 and r64rm64
	bool r64imm64(int reg, int64_t imm, INSTRUCTION* instr);			// Not tested properly.
	bool rm64imm32(MEM_REF* regmem, int32_t imm, INSTRUCTION* instr);	// Not finished.
	bool rm64(MEM_REF* regmem, INSTRUCTION* instr);						// Not finished.
	void pushInstruction(INSTRUCTION* instr, bool reqSib);


	/*** Function utils.  ***/
	bool regNeedsSIB(int reg);
	void sib(bool ismem, int base, int reg, char* sib);
	// Returns the size of the sib part.
	// LSB to MSB order when pushing them

	int getLength();
	void clear();
	char* genBytes32(int32_t disp);
	char* genBytes64(int64_t imm64);
	void pushimm64(int64_t imm64);
	void pushimm32(int32_t imm32);
	void pushimm16(int16_t imm16);
	void pushimm8(int8_t imm8);
	X64Emitter();
	~X64Emitter();
	unsigned char* buffer;
private:
	void push(char);
	int ptr;
};

#endif