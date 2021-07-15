#include <stdio.h>
#include <stdlib.h>

// http://obelisk.me.uk/6502

using Byte = unsigned char;  // a char is a 1 byte data structure used for characters ranging from 0 to 255
using Word = unsigned short; // a short is a 2 byte data structure used for small integers ranging from 0 to 65535

using u32 = unsigned int;

struct Mem
{
    static constexpr u32 MAX_MEM = 1024 * 64;
    Byte data[MAX_MEM];

    void Initialize()
    {
        for (u32 i = 0; i < MAX_MEM; i++)
        {
            data[i] = 0;
        }
    }

    // read 1 byte
    Byte operator[](u32 address) const
    {
        return data[address];
    }

    // write 1 byte
    // returns a reference to the byte that we can write to
    Byte &operator[](u32 address)
    {
        return data[address];
    }

    // write 2 bytes
    void WriteWord(Word value, u32 address, u32 cycles)
    {
        data[address] = value & 0xFF;
        data[address + 1] = (value >> 8);
        cycles -= 2;
    }
};

// this is the main CPU struct that contains all the things we need.
struct CPU
{
    Word PC; // the program counter
    Word SP; // the stack pointer; the address of the current point of the stack that the program is using

    Byte A, X, Y; // registers
    // A - Accumulator
    // X - Index register X
    // Y - Index register Y

    // processor status
    // flags
    Byte C : 1; // carry
    Byte Z : 1; // zero
    Byte I : 1; // interrupt disable
    Byte D : 1; // decimal mode
    Byte B : 1; // break command
    Byte V : 1; // overflow
    Byte N : 1; // negative

    void Reset(Mem &memory)
    {
        PC = 0xFFFC;
        SP = 0x0100;
        C = Z = I = D = B = V = N = 0;
        A = X = Y = 0;
        memory.Initialize();
    }

    // returns one byte, incrementing the program counter
    Byte FetchByte(u32 &cycles, Mem &memory)
    {
        Byte data = memory[PC];
        PC++;
        cycles--;

        return data;
    }

    // returns one byte without incrementing the program counter
    Byte ReadByte(u32 &cycles, Byte address, Mem &memory)
    {
        Byte data = memory[address];
        cycles--;

        return data;
    }

    // returns one word, incrementing the program counter
    Word FetchWord(u32 &cycles, Mem &memory)
    {
        // 6502 is little endian
        Word data = memory[PC];
        PC++;

        data |= (memory[PC] << 8);
        PC++;

        cycles -= 2;

        // if you wanted to handle endianness
        // you would have to swap bytes here
        // if ( PLATFORM_BIG_ENDIAN ) {
        //     SwapBytesInWord(data);
        // }

        return data;
    }

    // CPU instructions (opcodes)
    // LDA - Load Accumulator - Instructions for setting stuff on the Accumulator
    static constexpr Byte INS_LDA_IM = 0xA9;  // LDA immediate
    static constexpr Byte INS_LDA_ZP = 0xA5;  // LDA zero page
    static constexpr Byte INS_LDA_ZPX = 0xB5; // LDA zero page x
    static constexpr Byte INS_JSR = 0x20;

    void lda_set_status()
    {
        Z = (A == 0);             // if accumulator is zero
        N = (A & 0b10000000) > 0; // if bit 7 of the accumulator is set then we set the N flag
    }

    void Execute(u32 cycles, Mem &memory)
    {
        while (cycles > 0)
        {
            Byte ins = FetchByte(cycles, memory);

            switch (ins)
            {
            case INS_LDA_IM:
            {
                Byte value = FetchByte(cycles, memory);
                A = value;
                lda_set_status();
            }
            break;
            case INS_LDA_ZP:
            {
                Byte zero_page_addr = FetchByte(cycles, memory);
                A = ReadByte(cycles, zero_page_addr, memory);
                lda_set_status();
            }
            break;
            case INS_LDA_ZPX:
            {
                Byte zero_page_addr = FetchByte(cycles, memory);
                zero_page_addr += X;
                cycles--;
                A = ReadByte(cycles, zero_page_addr, memory);
                lda_set_status();
            }
            break;
            case INS_JSR:
            {
                Word sub_addr = FetchWord(cycles, memory);
                memory.WriteWord(PC - 1, SP, cycles);
                PC = sub_addr;
                cycles--;
            }
            break;
            default:
            {
                printf("Instruction not handled %d\n", ins);
            }
            break;
            }
        }
    }
};

int main()
{
    Mem mem;
    CPU cpu;
    cpu.Reset(mem);
    // start - inline a little program
    mem[0xFFFC] = CPU::INS_JSR;
    mem[0xFFFD] = 0x42;
    mem[0xFFFE] = 0x42;
    mem[0x4242] = CPU::INS_LDA_IM;
    mem[0x4243] = 0x84;
    // end
    cpu.Execute(9, mem);

    return 0;
}