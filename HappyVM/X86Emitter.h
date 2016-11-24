#ifndef X86EMITTER_H
#define X86EMITTER_H

#include <stdint.h>
#include <Windows.h>

#define X86_REGISTER_32_EAX 0
#define X86_REGISTER_32_ECX 1
#define X86_REGISTER_32_EDX 2
#define X86_REGISTER_32_EBX 3
#define X86_REGISTER_32_ESP 4
#define X86_REGISTER_32_EBP 5
#define X86_REGISTER_32_ESI 6
#define X86_REGISTER_32_EDI 7

#define MODRM_SIB_DISPLACEMENT_8_SIGNED 0x40
#define MODRM_SIB_DISPLACEMENT_32_SIGNED 0x80
#define MODRM_REGISTER_REGISTER 0xC0
#define MODRM_MEM_REFERENCE 0x0 // No displacement if R/M == 100, displacement only if R/M == 101

#define MODRM_SET_SIB 0x4

#define SIB_SCALE_1 0x0
#define SIB_SCALE_2 0x40
#define SIB_SCALE_4 0x80
#define SIB_SCALE_8 0xC0
#define SIB_NO_BASE_REGISTER 0x5

// Some instructions have a direction bit. If it is zero from register to memory add [ebx], eax, 1 is opposite.
// NOT ALL INSTRUCTIONS HAVE THIS.
// If D == 0 then MOD R/M <= REG (operand ordering for MODRM byte) (REG is source)
// If D == 1 then REG <= MOD R/M (REG is destination)
#define GET_DIRECTION_BIT(A) ((A >> 1) & 1)

// Single operand instruction, operand goes into R/M field of MODRM bit.
// In two operand instruction, second operand goes into R/M field.

// For some single operand or immediate operand, the REG field may be used as an operand extension bit.
// In this case R/M will be used instead for the operand.

// To get 16 bit addressing, a prefix byte is required.
// Some instructions have a size bit for the operands being used.
// if s==0 8 bit
// if s==1 32 bit. where s is opcode&1

// No [EBP] addressing due to SIB byte
// However, [EBP+0] is used instead (requries an extra byte for disp8)

// Very good document: http://www.c-jump.com/CIS77/CPU/x86/lecture.html#X77_0020_encoding_overview

// SIB Byte:

// Top 2 bytes are for the scale (1,2,4 or 8)
// Then the base bits are for adding another register [eax+ebx*n+disp]
// If base == 101 then no other register to add. (disp+eax*n)
// ESP register cannot be used (it is illegal.)

struct Instruction {
	unsigned char OpCode; // This is very good until it floating point instructions which require 2 bytes.
	unsigned char ModRM;
	unsigned char SIB;
	unsigned char* Displacement;
	unsigned char* Immediate;
	int DisplacementSize;
	int ImmediateSize;

	bool RequiresSib;
	bool RequiresDisplacement;
	bool RequiresImmediate;
};

struct Operand {
	// Immediate
	// Just a register
	// Memory reference w/ register*scale+register+offset
	bool mem;
	bool hasbaseReg;
	unsigned char reg;
	unsigned char extrareg;
	unsigned char scale;
	int offset;
	int immediate;
	bool isImmediate;

	Operand(int32_t imm32) : immediate(imm32), mem(false), hasbaseReg(false), scale(1), offset(0), isImmediate(true), reg(0) {}
	Operand(unsigned char reg) : mem(false), hasbaseReg(false), scale(1), offset(0), reg(reg), isImmediate(false) {}
	Operand(unsigned char reg, unsigned char scale, int offset) : mem(true), isImmediate(false), hasbaseReg(false), scale(scale), offset(offset), reg(reg) {}
	Operand(unsigned char reg, unsigned char reg2, unsigned char scale, int offset) : mem(true), isImmediate(false), hasbaseReg(true), scale(scale), offset(offset), reg(reg), extrareg(reg2) {}
};

class X86Emitter {
public:
	X86Emitter();
	~X86Emitter();
	Instruction* genInstruction(unsigned char opcode, int direction, int size, Operand* a);
	Instruction* genInstruction(unsigned char opcode, int direction, int size, Operand* a, Operand* b);
	
	unsigned char* buffer;
private:
	
	unsigned char* genByte(int8_t value) {
		unsigned char* memory = (unsigned char*)malloc(sizeof(int8_t));
		memory[0] = value&0xff;
		return memory;
	}

	unsigned char* genInt32(int32_t value) {
		unsigned char* memory = (unsigned char*)malloc(sizeof(int32_t));
		memory[0] = (value)&0xff;
		memory[1] = (value>>8)&0xff;
		memory[2] = (value>>16)&0xff;
		memory[3] = (value>>24)&0xff;
		return memory;
	}

	void push(char a) {
		this->buffer[this->ptr++] = a;
	}
	int ptr;
};

#endif