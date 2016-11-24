#include "stdafx.h"
#include "x86emitter.h"
#include <Windows.h>
#include <assert.h>

X86Emitter::X86Emitter() {
	this->buffer = (unsigned char*)malloc(128*1024); // 128 KB
}

X86Emitter::~X86Emitter() {
	if (this->buffer != nullptr) {
		free(this->buffer);
	}
}

void X86Emitter::emit(Instruction* instruction) {		
	// Order of bytes:
	// Prefixes (Currently not in use.)
	// OpCode	(Currently only a single byte)
	// ModRM
	// SIB
	// Displacement
	// Immediate
	this->push(instruction->OpCode);
	this->push(instruction->ModRM);
	if (instruction->RequiresSib)
		this->push(instruction->SIB);
	if (instruction->RequiresDisplacement)
		for (int i = 0;i < instruction->DisplacementSize;i++)
			this->push(*(instruction->Displacement+i));
	if (instruction->RequiresImmediate)
		for (int i = 0;i < instruction->ImmediateSize;i++)
			this->push(*(instruction->Immediate+i));
}

Instruction* X86Emitter::genInstruction(unsigned char opcode, int direction, int size, Operand* a, Operand* b) {
	// Define some variable names:
	//	A is the destination thing
	//	B is the source thing

	// Not using the size argument for generating instructions at the moment.
	// Not using displacement 8 for (-128 to 127 offset)? at the moment, unneccessary complication at the moment.

	// IMPORTANT INFORMATION: ESP CANNOT BE ADDRESSED DUE TO COLLIDING WITH SOMETHING
	// SOME INFORMATION ABOUT IT ON A WEBSITE. C-JUMP.COM

	// EXPLICIT FOR EBP
	//	r32, [r32]					(should work)
	//	r32, [r32+offset]			(should work)
	//  r32, [r32*n+offset]			(should work)
	//	r32, [r32*n+r32]			(should work)
	//  r32, [r32*n+r32+offset]		(should work)
	//  [r32], r32					(should work d=0)
	//  [r32+offset], r32			(should work d=0)
	//	[r32*n+offset], r32			(should work d=0)
	//	[r32*n+r32], r32			(should work d=0)
	//  [r32*n+r32+offset], r32		(should work d=0)
	//	[r32], imm32				(should work d=0, 1<<8 on opcode maybe)
	//  [r32+offset], imm32			(should work d=0, 1<<8 on opcode maybe)
	//	[r32*n+offset], imm32		(should work d=0, 1<<8 on opcode maybe)
	//	[r32*n+r32], imm32			(should work d=0, 1<<8 on opcode maybe)
	//  [r32*n+r32+offset], imm32	(should work d=0, 1<<8 on opcode maybe)
	//  [r32]						
	//  [r32+offset]				
	//	[r32*n+offset]				
	//	[r32*n+r32]					
	//  [r32*n+r32+offset]			

	// EXPLICIT FOR ESP (this requires SIB byte to work correctly.)
	//	r32, [r32]					
	//	r32, [r32+offset]			
	//  r32, [r32*n+offset]			
	//	r32, [r32*n+r32]			
	//  r32, [r32*n+r32+offset]		
	//  [r32], r32					
	//  [r32+offset], r32			
	//	[r32*n+offset], r32			
	//	[r32*n+r32], r32			
	//  [r32*n+r32+offset], r32		
	//	[r32], imm32				
	//  [r32+offset], imm32			
	//	[r32*n+offset], imm32		
	//	[r32*n+r32], imm32			
	//  [r32*n+r32+offset], imm32	
	//  [r32]						
	//  [r32+offset]				
	//	[r32*n+offset]				
	//	[r32*n+r32]					
	//  [r32*n+r32+offset]			

	// Combos (for every other register):
	//	imm32
	//	r32, r32					(should work)
	//	r32, [r32]					(should work)
	//	r32, [r32+offset]			(should work)
	//  r32, [r32*n+offset]			(should work)
	//	r32, [r32*n+r32]			(should work)
	//  r32, [r32*n+r32+offset]		(should work)
	//  [r32], r32					(should work d=0)
	//  [r32+offset], r32			(should work d=0)
	//	[r32*n+offset], r32			(should work d=0)
	//	[r32*n+r32], r32			(should work d=0)
	//  [r32*n+r32+offset], r32		(should work d=0)
	//  r32, imm32					(should work d=0, 1<<8 on opcode maybe)
	//	[r32], imm32				(should work d=0, 1<<8 on opcode maybe)
	//  [r32+offset], imm32			(should work d=0, 1<<8 on opcode maybe)
	//	[r32*n+offset], imm32		(should work d=0, 1<<8 on opcode maybe)
	//	[r32*n+r32], imm32			(should work d=0, 1<<8 on opcode maybe)
	//  [r32*n+r32+offset], imm32	(should work d=0, 1<<8 on opcode maybe)
	//  [r32]						
	//  [r32+offset]				
	//	[r32*n+offset]				
	//	[r32*n+r32]					
	//  [r32*n+r32+offset]			

	Instruction* instruction = new Instruction();

	// Single operand instruction.
	if (b == nullptr && a != nullptr) {
		// Do the single operand cases!
	}

	// Direction and size will be blank for opcodes which don't use them.
	instruction->OpCode = opcode | (direction&1) << 1 | (size & 1);

	bool Areg = a->mem == false;
	bool Breg = b->mem == false;
	bool Bimm = b->isImmediate;

	if (!Areg && !Breg) assert(false && "Cannot have two memory operands");
	if (a->isImmediate) assert(false && "Destination operand cannot be an immediate value");

	if (Areg && (Breg || Bimm)) // reg, reg
		instruction->ModRM |= MODRM_REGISTER_REGISTER;
	else {
		if (!Areg) { // A reg is memory

			if (a->hasbaseReg==false && a->offset!=0 && a->scale == 1) // [reg+offset]
				instruction->ModRM |= MODRM_SIB_DISPLACEMENT_32_SIGNED;
			else if (a->hasbaseReg==false && a->scale!=1)
				instruction->ModRM |= MODRM_MEM_REFERENCE;
			else if (a->hasbaseReg==true && a->offset==0) // reg32+reg32*n
				instruction->ModRM |= MODRM_MEM_REFERENCE;
			else if (a->hasbaseReg==true && a->offset != 0) // reg32+disp32+reg32*n
				instruction->ModRM |= MODRM_SIB_DISPLACEMENT_32_SIGNED;
			else if (a->hasbaseReg==false && a->offset == 0 && a->scale == 1) // [r32]
				instruction->ModRM |= MODRM_MEM_REFERENCE;
			else
				assert(false && "ModRM couldn't be made correctly, might need another condition implementing.");

		} else if (!Breg) { // B reg is memory

			if (b->hasbaseReg==false && b->offset!=0 && b->scale == 1) // [reg+offset]
				instruction->ModRM |= MODRM_SIB_DISPLACEMENT_32_SIGNED;
			else if (b->hasbaseReg==false && b->scale!=1)
				instruction->ModRM |= MODRM_MEM_REFERENCE;
			else if (b->hasbaseReg==true && b->offset==0) // reg32+reg32*n
				instruction->ModRM |= MODRM_MEM_REFERENCE;
			else if (b->hasbaseReg==true && b->offset != 0) // reg32+disp32+reg32*n
				instruction->ModRM |= MODRM_SIB_DISPLACEMENT_32_SIGNED;
			else if (b->hasbaseReg==false && b->offset == 0 && b->scale == 1) // [r32]
				instruction->ModRM |= MODRM_MEM_REFERENCE;
			else
				assert(false && "ModRM couldn't be made correctly, might need another condition implementing.");
		}
	}

	if (direction==0) // modrm <= reg 
	{
		instruction->ModRM |= ( (b->reg & 7) << 3);
		instruction->ModRM |= (a->reg & 7);
	}
	else if (direction==1) // then reg <= modrm
	{ 
		instruction->ModRM |= (b->reg & 7);
		instruction->ModRM |= ( (a->reg & 7) << 3);
	}
	else
	{
		assert(false && "Invalid direction bit, either 0 or 1");
	}

	if (!Areg) { // [b], r32/imm32

		if (Bimm) { // [r32], imm32
			instruction->RequiresImmediate = true;
			instruction->ImmediateSize = 4;
			instruction->Immediate = this->genInt32(b->immediate);
		}

		if (a->offset==0 && a->scale == 1 && a->hasbaseReg==false) { // [r32]
			if (a->reg==X86_REGISTER_32_EBP) {
				instruction->ModRM&=0x3f; // clear mod bit.
				instruction->ModRM |= MODRM_SIB_DISPLACEMENT_8_SIGNED;
				instruction->RequiresDisplacement = true;
				instruction->DisplacementSize = 1;
				instruction->Displacement = this->genByte(0); // Displacement of zero.
			}
			return instruction;
		} else if (a->offset != 0 && a->scale == 1 && a->hasbaseReg==false) { // [r32+offset]
			instruction->RequiresDisplacement = true;
			instruction->DisplacementSize = 4;
			instruction->Displacement = this->genInt32(a->offset);
			return instruction;
		} else if (a->scale != 1) {
			instruction->RequiresSib=true;
			instruction->ModRM &= 0xf8; // Mask out lower 3 bits.
			instruction->ModRM |= MODRM_SET_SIB; // This is SIB_BIT

			switch (a->scale) {
			case 1:
				instruction->SIB = SIB_SCALE_1;
				break;
			case 2:
				instruction->SIB = SIB_SCALE_2;
				break;
			case 4:
				instruction->SIB = SIB_SCALE_4;
				break;
			case 8:
				instruction->SIB = SIB_SCALE_8;
				break;
			default:
				assert(false && "Invalid scale used for register value");
			}

			// Need a better way to write it than this.
			if (a->hasbaseReg==true && a->offset==0)
				int dummy = 0;
			else 
			{
				instruction->RequiresDisplacement = true;
				instruction->DisplacementSize = 4;
				instruction->Displacement = this->genInt32(a->offset);
			}

			if (a->reg == X86_REGISTER_32_ESP)
				assert(false && "ESP cannot be register in scale+offset [esp*scale+<offset>] not allowed!");
			instruction->SIB |= ( (a->reg & 7) << 3);

			// Add the base register if it exists.
			if (a->hasbaseReg==true) {
				instruction->SIB |= (a->extrareg&7);
			} else {
				instruction->SIB |= SIB_NO_BASE_REGISTER;
			}
		}
	} else if (!Breg) { // r32,[b]
		if (b->offset==0 && b->scale == 1 && b->hasbaseReg==false) { // [r32]
			if (b->reg==X86_REGISTER_32_EBP) {
				instruction->ModRM&=0x3f; // clear mod bit.
				instruction->ModRM |= MODRM_SIB_DISPLACEMENT_8_SIGNED;
				instruction->RequiresDisplacement = true;
				instruction->DisplacementSize = 1;
				instruction->Displacement = this->genByte(0); // Displacement of zero.
			}
			return instruction;
		} else if (b->offset != 0 && b->scale == 1 && b->hasbaseReg==false) { // [r32+offset]
			instruction->RequiresDisplacement = true;
			instruction->DisplacementSize = 4;
			instruction->Displacement = this->genInt32(b->offset);
			return instruction;
		} else if (b->scale != 1) {
			instruction->RequiresSib=true;
			instruction->ModRM &= 0xf8; // Mask out lower 3 bits.
			instruction->ModRM |= MODRM_SET_SIB; // This is SIB_BIT

			switch (b->scale) {
			case 1:
				instruction->SIB = SIB_SCALE_1;
				break;
			case 2:
				instruction->SIB = SIB_SCALE_2;
				break;
			case 4:
				instruction->SIB = SIB_SCALE_4;
				break;
			case 8:
				instruction->SIB = SIB_SCALE_8;
				break;
			default:
				assert(false && "Invalid scale used for register value");
			}

			// Need a better way to write it than this.
			if (b->hasbaseReg==true && b->offset==0)
				int dummy = 0;
			else 
			{
				instruction->RequiresDisplacement = true;
				instruction->DisplacementSize = 4;
				instruction->Displacement = this->genInt32(b->offset);
			}

			if (b->reg == X86_REGISTER_32_ESP)
				assert(false && "ESP cannot be register in scale+offset [esp*scale+<offset>] not allowed!");
			instruction->SIB |= ( (b->reg & 7) << 3);

			// Add the base register if it exists.
			if (b->hasbaseReg==true) {
				instruction->SIB |= (b->extrareg&7);
			} else {
				instruction->SIB |= SIB_NO_BASE_REGISTER;
			}
		}
	}

	if (Bimm) {
		instruction->RequiresImmediate = true;
		instruction->ImmediateSize = 4;
		instruction->Immediate = this->genInt32(b->immediate);
	}

	return instruction;
}