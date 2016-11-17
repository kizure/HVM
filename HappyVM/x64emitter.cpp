#include "stdafx.h"
#include "x64emitter.h"
#include <Windows.h>
#include <assert.h>

/*
	0100 1000  0000 0001  1100 1000
	Right MSB
	Left LSB

	REX field
	11000000
	Refer to intel guide page 43.

	0 to 2 is R/W
	3 to 5 is register value
	6 to 7 is MOD (addressing? mode)

	add [rax], rcx
		48 01 08
		0100 1000 0000 0001 0000 1000
	
		Byte 3 (REX)
		64 bit register
	
		Byte 2 (Opcode)
		0000 0001
	
		Byte 1 (ModR/W)
		0000 1000
		MOD = 00
		REG = 001
		R/W(REG2) = 000

	add rax, rcx
		rax = 0
		rcx = 1
		add opcode = 0

		add opcode is 0, 1, 2 or 3 depending on registers

		OpCode byte
		0000 0001
		0000 00-- is the opcode
		---- --0- is the direction flag (no memory so it is zero)
		---- ---1 is the size flag (0 for 8 bit values, 1 for more than that, as the registers are 64 bits it is 1)

		ModR/W byte
		1100 1000
		11-- ---- These indicate that the R/M field is a register
		--00 1--- Is a register (1 is rcx)
		---- -000 is a register in the r/w field (0 is rax)

		REX field (x64 requires this)
		0100 1000
		0100 ---- This is a requirement (fixed bit pattern)
		---- 1000 This is because the registers are 64 bit (64 bit operand)

	Another example:
		add rax, 1
		4         3         2         1        
		0100 1000 1000 0011 1100 0000 0000 0001
	
		Byte 4
		0100 1000 is the REX byte (rax is 64 bit register thus 64 bit field is flagged)
	
		Byte 3
		1000 0011 is the add Opcode for imm8 to reg/mem16/32/64
	
		Byte 2
		1100 0000 ModR/W (11 means r/w is register, 0 is the register)
	
		Byte 1
		0000 0001 Immediate 8 bit value
	
	Another example:
		mov rax, rcx (puts rcx into rax)
		48h 89h c8h
	
		Byte 3
		0100 1000 REX byte (64 bit register in use)
	
		Byte 2
		1000 1001 Opcode for mov reg/mem16/32/64, reg16/32/64
	
		Byte 1
		1100 1000 ModR/W byte (11 shows R/W is a register, 001 is RCX, 000 is REX)
	
	Another example:
		mov r8, rcx (puts rcx into r8) r8 is a specific x64 register which breaks the x86 register scheme
		49 89 c8
		0100 1001 1000 1001 1100 1000
	
		Byte 3
		0100 1001 REX byte (1001 is for 64 bit register with 1 bit for extra bit for r/m register)
	
		Byte 2
		1000 1001 Opcode for move
	
		Byte 1
		1100 1000 ModR/W Registers ( 001 = rcx (operand 2), (1)000 which is r8 (operand 1) )
	
	Another example:
		mov r8, [rcx] (puts the data at the pointer rcx into r8)
		4c 8b 01
		0100 1100 1000 1011 0000 0001
	
		Byte 3 (REX)
		0100 1100 (64 bit register, extension to ModR/M field)
	
		Byte 2
		1000 1011 Opcode for mov reg64, reg/mem64
	
		Byte 1
		0000 0001 ModR/W (mod = 00 so indirect addressing, REG = 1000 (r9), R/W = 001 which is rcx register)
	

	Implemented OpCodes:
		ADD reg/mem64, reg64
		RET

	To Implement:
		
		ADD reg64, reg/mem64
		ADD reg/mem64, imm32
		
		AND reg/mem64, imm32
		AND reg64, reg/mem64 

		CALL reg/mem64 (Near)
		CALL rel32off  (Near)

		CMOVO reg64, reg/mem64
		CMOVNO reg64, reg/mem64
		CMOVB reg64, reg/mem64
		CMOVC reg64, reg/mem64
		CMOVNAE reg64, reg/mem64
		CMOVNV reg64, reg/mem64
		CMOVNC reg64, reg/mem64
		CMOVAE
		CMOVZ
		CMOVE
		CMOVNZ
		CMOVNE
		CMOVBE
		CMOVNA
		CMOVNBE
		CMOVA
		CMOVS
		CMOVNS
		CMOVP
		CMOVPE
		CMOVNP
		CMOVPO
		CMOVL
		CMOVNGE
		CMOVNGE
		CMOVNL
		CMOVGE
		CMOVLE
		CMOVNG
		CMOVNLE
		CMOVG

		CMP reg/mem64, imm32
		CMP reg/mem64, imm8
		CMP reg/mem64, reg64
		CMP reg64, reg/mem64

		String comparisons functions (they require REP prefix)

		CMPXCHG reg/mem64, reg64 (this one looks weird)

		CPUID

		CRC32 sounds neat (looks complex though)

		DEC reg/mem64

		DIV reg/mem64 (unsigned)
		IDIV reg/mem64 (signed)
		IMUL reg/mem64 (signed)
		IMUL reg64, reg/mem64 (signed)

		INC reg/mem64

		INT imm8

		JO rel32off
		JNO rel32off
		JB
		JC
		JNAE
		JNB
		JNC
		JAE
		JZ
		JE
		JNZ
		JNE
		JBE
		JNA
		JNBE
		JA
		JS
		JNS
		JP
		JPE
		JNP
		JPO
		JL
		JNGE
		JNL
		JGE
		JLE
		JNG
		JNLE
		JG

		JRCXZ rel8off

		JMP rel32off (near)
		JMP reg/mem64 (near)

		JMP FAR mem16:32 (far)

		LFS reg64, mem16:64
		LGS reg64, mem16:64
		LSS reg64, mem16:64

		LEA reg64, mem

		LODS mem64 (String loading)

		LOOP rel8off
		LOOPE
		LOOPNE
		LOOPNZ
		LOOPZ

		MOV reg/mem64, reg64
		MOV reg64, reg/mem64
		MOV reg64, imm64
		MOV reg/mem64, imm32
		
		MOVBE reg64, mem64
		MOVBE mem64, reg64

		MOVD xmm, reg/mem64
		MOVD reg/mem64, xmm
		MOVD mmx, reg/mem64
		MOVD reg/mem64, mmx

		MOVNTI mem64, reg64

		MOVS mem64, mem64 (moving strings)

		MOVZX reg64, reg/mem16

		MUL reg/mem64 (unsigned)
	
		NEG reg/mem64 (two's complement for a register / memory location)

		NOP
		NOP reg/mem64

		NOT reg/mem64

		OR reg/mem64, imm32
		OR reg/mem64, reg64
		OR reg64, reg/mem64

		PAUSE (to do with spin loops)

		POP reg/mem64
		POP reg64

		POPA (pops all gprs)

		PUSH reg/mem64
		PUSH imm64
		PUSH reg64

		PUSHA (Push alls GPR onto stack)

		RCL reg/mem64, 1 (seems weird)
		RCR reg/mem64, 1

		RDRAND reg64 (random number generator)

		RETF
		RETF imm16
		
		ROL reg/mem64, 1
		ROR reg/mem64, 1

		SHL reg/mem64, 1
		SHL reg/mem64, imm8

		SAR reg/mem64, 1
		SAR reg/mem64, imm8

		SHR reg/mem64, 1
		SHR reg/mem64, imm8

		SUB reg/mem64, imm32
		SUB reg/mem64, reg64
		SUB reg64, reg/mem64

		XADD reg/mem64, reg64

		XCHG reg/mem64, reg64
		XCHG reg64, reg/mem64

		XOR reg/mem64, reg64
		XOR reg64, reg/mem64
		XOR reg/mem64, imm32

		INT3 (Breakpoints)

		IRET
		IRETD
		IRETQ

		SYSCALL


*/


// X64 Assembly:
//	Calling conventions (Windows 64):
//		first 4 arguments are registers after that stack is used.
//		RCX, RDX, R8, R9 in that order are integer arguments for the first 4
//		XMM0L, XMM1L, XMM2L, XMM3L for floating point registers.

X64Emitter::X64Emitter() {
	this->buffer = (unsigned char*)malloc(128*1024); // 128KB of instructions.
	this->ptr = 0;
}

X64Emitter::~X64Emitter() {
	assert(this->buffer!=nullptr && "X64 Emitter Buffer is nullptr!");
	free(this->buffer);
}

// For testing purposing so other code isn't broken unneccessarily.
void X64Emitter::movr64rm64(int reg, MEM_REF* regmem) {
	INSTRUCTION* instruction = new INSTRUCTION(0x48, 0x8b);

	bool reqSib = this->r64rm64(reg, regmem, instruction);

	this->pushInstruction(instruction, reqSib);
}

void X64Emitter::movrm64r64(MEM_REF* regmem, int reg) {
	INSTRUCTION* instruction = new INSTRUCTION(0x48, 0x89);

	bool reqSib = this->r64rm64(reg, regmem, instruction);
	this->pushInstruction(instruction, reqSib);
}

void X64Emitter::pushInstruction(INSTRUCTION* instr, bool reqSib) {
	this->push(instr->REX);
	this->push(instr->OPCODE);
	this->push(instr->MODRM);
	if (reqSib)
		this->push(instr->SIB);

	// If we have extra bytes (for displacements, etc)
	if (instr->EXTRAS!=nullptr) {
		for (int i = 0 ;i < instr->ExtrasCount;i++) {
			this->push(instr->EXTRAS[i]);
		}
	}
}

bool X64Emitter::r64rm64(int reg, MEM_REF* regmem, INSTRUCTION* instr) {
	bool isreg = !regmem->is_mem;
	bool reqSib = false;

	if(isreg)
		instr->MODRM = 0xc0;
	else {
		instr->MODRM = 0x0;
	}
	
	// If the scale is 1 and it is a memory reference from a register and we have an offset.
	// Then we can use modrm with a displacement of 16 bits for the offset.
	// Without having to have a SIB byte.
	
	// Combos:
	// r64, r64 ✔
	// r64, m64 (SIB) ✔
	// r64, m64 (!SIB) ✔
	// r64, m64+0 (!SIB) ✔
	// r64, m64*1+0 (!SIB) ✔
	// r64, m64*1+0 (SIB) ✔
	// Got all cases done, 
	// now just needs to be refactored so it's nicer and simpler to read.

	if (this->regNeedsSIB(regmem->reg) == false && regmem->is_mem && regmem->scale == 1 && regmem->offset!=0) {
		// r64, m64+0 (!SIB)
		if (regmem->offset <= 0x7f && regmem->offset >= -0x80) {
			instr->MODRM = 0x40; // 01 for the mod bit (this allows for 8 bit displacement)
			instr->EXTRAS = (char*)malloc(sizeof(char)*1); // 1 for a byte
			*(instr->EXTRAS) = regmem->offset&0xff; // Set to the value.
			instr->ExtrasCount = 1; // 1 bytes for the offset.
		} else {
			instr->MODRM = 0x80; // 10 for the mod bit. (this allows for 32 bit displacement)
			instr->EXTRAS = this->genBytes32(regmem->offset);
			instr->ExtrasCount = 4; // 4 bytes for the offset.
		}
		instr->MODRM |= (regmem->reg&0x7); // Operand 1 (result register) 
	}
	else if (this->regNeedsSIB(regmem->reg) == false && regmem->is_mem) {
		if (regmem->scale == 1 && regmem->offset == 0) {
			// r64, m64 (!SIB)
			instr->MODRM = 0;
			instr->MODRM |= regmem->reg&0x7; // Operand 1 (result register)
		} else { // any offset, any scale.
			// r64, m64*1+0 (SIB)
			instr->MODRM |= 0x4; // 100 (SIB Byte)
			reqSib = true;
			this->sib(instr, regmem);
		}
	}
	else if (this->regNeedsSIB(regmem->reg) && regmem->is_mem) {
		//instr->MODRM |= 0x40; // 100 (SIB Byte)
		reqSib = true;
		if (regmem->scale == 1 && regmem->offset == 0) {
			// SIB is only required with memory referencing
			// r64, m64 (SIB)
			if (regmem->reg>=REG_R8) instr->REX |= 1; // extra bit for register access
			instr->SIB=0;
			instr->SIB |= (regmem->reg&0x7)<<3;
			instr->SIB |= (regmem->reg&0x7);
		} else {
			// r64, m64*1+0	(SIB) any scale or offset.
			this->sib(instr, regmem);
		}
	}
	else if (isreg) { // No offset, no scale, no sib, no memory reference.
		// r64, r64
		instr->MODRM |= (regmem->reg&0x7); // Operand 1 (result register) 
	}

	// If it is a register which doesn't need SIB but has offset
	// Then set MODRM mod to 01 for disp16 for the extra bit.
	// This then needs the extra for the displacement bytes (offset)

	// If it is just a register, not memory
	// Then we don't care about SIB
	// Else it it is memory
	// We need only SIB for certain instructions
	// Unless that register is being scaled or offsetted
	// then it needs an SIB regardless, the base is 101

	if (regmem->reg>=REG_R8 && reqSib==false) instr->REX |= 1; // R/M Field extension bit
	if (reg>=REG_R8) instr->REX |= (1<<2); // REG Field extension bit 
	instr->MODRM |= (reg&0x7)<<3; // Operand 2 (register to add with)

	return reqSib;
}

bool X64Emitter::r64imm64(int reg, int64_t imm, INSTRUCTION* instr) {
	instr->MODRM = 0xc0; // Shows it's a register.
	if (reg>=REG_R8) instr->REX |= (1<<2); // REG Field extension bit 
	instr->MODRM |= (reg&0x7); // Put the register in.
	instr->ExtrasCount = 8;
	instr->EXTRAS = this->genBytes64(imm);
	return false; // No SIB required
}

bool X64Emitter::rm64imm32(MEM_REF* regmem, int32_t imm, INSTRUCTION* instr) {
	
	if (regmem->reg>=REG_R8) instr->REX |= (1); // REG Field extension bit
	if (regmem->is_mem==false) {
		instr->MODRM = 0xc0;
		instr->MODRM |= (regmem->reg)&0x7;
		instr->ExtrasCount=4;
		instr->EXTRAS = this->genBytes32(imm);
		return false;
	} else {
		// Some magic will have to happen.
		if (regmem->scale == 1) {
			instr->MODRM = 0;
			//i
		}
	}

	return false;
}

void X64Emitter::sib(INSTRUCTION* instr, MEM_REF* regmem) {
	if (regmem->reg>=REG_R8) instr->REX |= 0x2; // extra bit for register access SIB.
	instr->SIB = 0;
	switch (regmem->scale) {
	case 1:
		break;
	case 2:
		instr->SIB = 0x40;
		break;
	case 4:
		instr->SIB = 0x80;
		break;
	case 8:
		instr->SIB = 0xc0;
		break;
	default: // Invalid scale value.
		assert(false && "Invalid scale value for SIB, has to be 1, 2, 4, or 8 (register*scale+offset)");
		break;
	}

	instr->SIB |= (regmem->reg&0x7)<<3; // Index
	instr->SIB &= 0xf8; // mask out lowest three for the new base value
	instr->SIB |= 0x5; // 101 for the base. (then needs the extra 4 bytes)
	instr->EXTRAS = this->genBytes32(regmem->offset);
	instr->ExtrasCount = 4; // 4 bytes for the offset.
}

char* X64Emitter::genBytes32(int32_t disp) {
	char* dat = (char*)malloc(sizeof(char)*4);
	dat[0] = disp&0xff;
	dat[1] = (disp>>8)&0xff;
	dat[2] = (disp>>16)&0xff;
	dat[3] = (disp>>24)&0xff;
	return dat;
}

char* X64Emitter::genBytes64(int64_t imm64) {
	char* dat = (char*)malloc(sizeof(char)*4);
	dat[0] = imm64&0xff;
	dat[0] = (imm64>>8)&0xff;
	dat[0] = (imm64>>16)&0xff;
	dat[0] = (imm64>>24)&0xff;
	dat[0] = (imm64>>32)&0xff;
	dat[0] = (imm64>>40)&0xff;
	dat[0] = (imm64>>48)&0xff;
	dat[0] = (imm64>>56)&0xff;
	return dat;
}

void X64Emitter::sib(bool ismem, int scale, int reg, char* sib) {
	if (scale == 1)
		*sib = 0;
	else if (scale == 2)
		*sib = 0x40;
	else if (scale == 4)
		*sib = 0x80;
	else if (scale == 8)
		*sib = 0xc0;
	*sib |= (reg&0x7)<<3;
	if (ismem) // copy index into base
		*sib |= (reg&0x7);
	return;
}

int X64Emitter::getLength() {
	return this->ptr;
}

void X64Emitter::clear() {
	memset(this->buffer, 0, this->ptr);
	this->ptr = 0;
}

bool X64Emitter::regNeedsSIB(int reg) {
	reg &= 0xf;
	if (reg==REG_R12 || reg==REG_R13 || reg==REG_RBP || reg==REG_RSP) return true;
	return false;
}

void X64Emitter::pushimm64(int64_t imm64) {
	this->push(imm64&0xff);
	this->push((imm64>>8)&0xff);
	this->push((imm64>>16)&0xff);
	this->push((imm64>>24)&0xff);
	this->push((imm64>>32)&0xff);
	this->push((imm64>>40)&0xff);
	this->push((imm64>>48)&0xff);
	this->push((imm64>>56)&0xff);
}
	
void X64Emitter::pushimm32(int32_t imm32) {
	this->push(imm32&0xff);
	this->push((imm32>>8)&0xff);
	this->push((imm32>>16)&0xff);
	this->push((imm32>>24)&0xff);
}
	
void X64Emitter::pushimm16(int16_t imm16) {
	this->push(imm16&0xff);
	this->push(imm16>>8);
}
	
void X64Emitter::pushimm8(int8_t imm8) {
	this->push(imm8);
}

void X64Emitter::ret() {
	this->push(0xc3);
}

void X64Emitter::push(char c) {
	this->buffer[this->ptr++] = c;
}
