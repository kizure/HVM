// -----------------------------------------------------------------------
// <copyright file="HvmEmitter.cs" company="">
// TODO: Update copyright text.
// </copyright>
// -----------------------------------------------------------------------

namespace HVMCompiler
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Runtime.InteropServices;
    using System.Diagnostics;

    public enum HopInstruction : byte
    {
        HOP_ADD = 0, // DONE
        HOP_SUB = 1, // DONE
        HOP_MUL = 2, // DONE
        HOP_DIV = 3, // DONE
        HOP_MOD = 4, // DONE
        HOP_OR = 5, // DONE
        HOP_XOR = 6, // DONE
        HOP_NOT = 7, // DONE
        HOP_AND = 8, // DONE
        HOP_JMP = 9, // DONE
        HOP_CALL = 10, // DONE
        HOP_RET = 11, // DONE
        HOP_TEST = 12, // DONE
        HOP_PUSH = 13, // DONE
        HOP_POP = 17, // DONE
        HOP_END = 14, // DONE
        HOP_SHL = 15, // DONE
        HOP_SHR = 16, // DONE
        HOP_BRKPT = 18, // DONE
        HOP_LDITM = 19, // Load Item (Array) DONE
        HOP_STITM = 20, // Store Item (Array) DONE
        HOP_ECALL = 21, // External call DONE
    }

    /// <summary>
    /// Emitter for writing instructions with data for HappyVM
    /// </summary>
    public class HvmEmitter
    {
        private byte[] _Data;
        private int _Pointer=0;

        public const byte HOP_INT_TYPE = 0;
        public const byte HOP_FLOAT_TYPE = 1;
        public const byte HOP_STRING_TYPE	=	2;
        public const byte HOP_ARRAY_TYPE=		3;
        public const byte HOP_POINTER_TYPE=	4;

        public const byte HOP_CONDITION_EQUAL		=			1;
        public const byte HOP_CONDITION_LESS	=				2;
        public const byte HOP_CONDITION_GREATER		=		3;
        public const byte HOP_CONDITION_NOT_EQUAL		=		4;
        public const byte HOP_CONDITION_LESS_THAN_EQUAL_TO	=5;
        public const byte HOP_CONDITION_GREATER_THAN_EQUAL_TO = 6;

        public HvmEmitter(int size)
        {
            _Data = new byte[size];
        }

        public void push(int value)
        {
            this.pushChar(encodeInstruction(HOP_INT_TYPE, (byte)HopInstruction.HOP_PUSH));
            this.pushInt(value);
        }

        public void pushPtr(int address)
        {
            this.pushChar(encodeInstruction(HOP_POINTER_TYPE, (byte)HopInstruction.HOP_PUSH));
            this.pushInt(address);
        }

        public void push(float value)
        {
            this.pushChar(encodeInstruction(HOP_FLOAT_TYPE, (byte)HopInstruction.HOP_PUSH));
            this.pushFloat(value);
        }

        /*
         * Push array to the list. Need an array object to do this nicely I think. 
         * 
         * 
         */

        public void push(string message)
        {
            Debug.Assert(message.Length <= 0xffff, "Emitter cannot push string of that size as a static string");
            this.pushChar(encodeInstruction(HOP_STRING_TYPE, (byte)HopInstruction.HOP_PUSH));
            this.pushChar((byte)( (message.Length>>8)&0xff ));
            this.pushChar((byte)(message.Length & 0xff));
            Array.Copy(message.ToCharArray().Select(a => (byte)a).ToArray(), 0, _Data, _Pointer, message.Length);
            _Pointer += message.Length;
        }

        public int label()
        {
            return this._Pointer;
        }

        public void pop()
        {
            this.pushChar((byte)HopInstruction.HOP_POP);
        }

        public void end()
        {
            this.pushChar((byte)HopInstruction.HOP_END); // end instruction.
        }

        public void add()
        {
            this.pushChar((byte)HopInstruction.HOP_ADD);
        }

        public void sub()
        {
            this.pushChar((byte)HopInstruction.HOP_SUB);
        }

        public void breakpoint()
        {
            this.pushChar((byte)HopInstruction.HOP_BRKPT);
        }

        public void mul()
        {
            this.pushChar((byte)HopInstruction.HOP_MUL);
        }

        public void div()
        {
            this.pushChar((byte)HopInstruction.HOP_DIV);
        }

        public void mod()
        {
            this.pushChar((byte)HopInstruction.HOP_MOD);
        }

        public void or()
        {
            this.pushChar((byte)HopInstruction.HOP_OR);
        }

        public void xor()
        {
            this.pushChar((byte)HopInstruction.HOP_XOR);
        }

        public void not()
        {
            this.pushChar((byte)HopInstruction.HOP_NOT);
        }

        public void and()
        {
            this.pushChar((byte)HopInstruction.HOP_AND);
        }

        public int jmp(int address)
        {
            int asmPos = _Pointer;
            this.pushChar((byte)HopInstruction.HOP_JMP);
            this.pushInt(address);
            return asmPos;
        }

        public void patchJump(int jmpAddress, int newAddress)
        {
            int tmp = _Pointer;
            this._Pointer = jmpAddress;
            this.jmp(newAddress);
            this._Pointer = newAddress;
        }

        public void call(int address)
        {
            this.pushChar((byte)HopInstruction.HOP_CALL);
            this.pushInt(address);
        }

        public void ecall(int address)
        {
            pushChar((byte)HopInstruction.HOP_ECALL);
            pushInt(address);
        }

        public void ret()
        {
            this.pushChar((byte)HopInstruction.HOP_RET);
        }

        public void test(int condition)
        {
            this.pushChar((byte)HopInstruction.HOP_TEST);
            this.pushChar((byte)condition);
        }

        public void shl()
        {
            this.pushChar((byte)HopInstruction.HOP_SHL);
        }

        public void shr()
        {
            this.pushChar((byte)HopInstruction.HOP_SHR);
        }

        public void lditm()
        {
            this.pushChar((byte)HopInstruction.HOP_LDITM);
        }

        public void stitm()
        {
            this.pushChar((byte)HopInstruction.HOP_STITM);
        }

        public byte[] complete()
        {
            return _Data;
        }

        public void Test()
        {
            FloatUnion testUnion = new FloatUnion();
            testUnion.value = 100.2f;

            int dummy = 0;
        }

        private void pushFloat(float f)
        {
            FloatUnion union = new FloatUnion();
            union.value = f;
            pushChar(union.b8);
            pushChar(union.b16);
            pushChar(union.b24);
            pushChar(union.b32);
        }

        private void pushInt(int val)
        {
            pushChar((byte)( (val >> 24) & 0xFF ));
            pushChar((byte)( (val >> 16) & 0xFF ));
            pushChar((byte)( (val >> 8) & 0xFF ));
            pushChar((byte)( (val) & 0xFF ));
        }

        [StructLayout(LayoutKind.Explicit)]
        private struct FloatUnion
        {
            [FieldOffset(0)]
            public byte b8;
            [FieldOffset(1)]
            public byte b16;
            [FieldOffset(2)]
            public byte b24;
            [FieldOffset(3)]
            public byte b32;
            [FieldOffset(0)] // Will this span all the bytes?
            public float value;
        }

        private byte encodeInstruction(byte dataType, byte instruction)
        {
            return (byte)(((dataType & 7) << 5) + (instruction & 0x1F));
        }

        private void pushChar(byte c)
        {
            if (_Pointer >= _Data.Length)
                Array.Resize<byte>(ref _Data, (_Data.Length * 3) / 2); // Increase the size of the array.
            _Data[_Pointer++] = c;
        }
    }
}
