#include <iostream>
#include <fstream>



enum Instruction
{
	PUSH8 = 0x00, POP8 = 0x01, ADD8 = 0x02, SUB8 = 0x03, MUL8 = 0x04, DIV8 = 0x05, LIT8 = 0x06, LDR8 = 0x07, STR8 = 0x08, MOV8 = 0x09, AND8 = 0x0A, OR8 = 0x0B, XOR8 = 0x0C, NOT8 = 0x0D, LDRSP8 = 0x0E, STRSP8 = 0x0F,
	PUSH16 = 0x10, POP16 = 0x11, ADD16 = 0x12, SUB16 = 0x13, MUL16 = 0x14, DIV16 = 0x15, LIT16 = 0x16, LDR16 = 0x17, STR16 = 0x18, MOV16 = 0x19, AND16 = 0x1A, OR16 = 0x1B, XOR16 = 0x1C, NOT16 = 0x1D, LDRSP16 = 0x1E, STRSP16 = 0x1F,
	PUSH32 = 0x20, POP32 = 0x21, ADD32 = 0x22, SUB32 = 0x23, MUL32 = 0x24, DIV32 = 0x25, LIT32 = 0x26, LDR32 = 0x27, STR32 = 0x28, MOV32 = 0x29, AND32 = 0x2A, OR32 = 0x2B, XOR32 = 0x2C, NOT32 = 0x2D, LDRSP32 = 0x2E, STRSP32 = 0x2F,
	LOR = 0x30, LAND = 0x31, LNOT = 0x32, LT = 0x33, GT = 0x34, JMP = 0x35, JMPZ = 0x36,
	CALL = 0x40, CALLA = 0x41,
	PRINTSA = 0xE0, GETINT = 0xE1, PRINTIA = 0xE2,
	EXIT = 0xFD, RET = 0xFE, NOP = 0xFF
};

int TinselVMRun(unsigned char* Memory, long EntryPoint)
{
	long PC = EntryPoint;
	long A = 0;
	long B = 0;
	unsigned char* Stack = new unsigned char[4096];
	long SP = 0;
	while (SP >= 0)
	{

		//Get Instruction from memory
		unsigned char Instruction = Memory[PC++];

		//std::cout << "I = " << std::hex << (int)Instruction << std::dec << "; A = " << A << "; B = " << B << ";" << std::endl;
		int Register;

		//Do a massive switch statement, performing the required action, generally with a ton of bit shifting magic stuff...
		switch (Instruction)
		{
		case PUSH8:
			Stack[SP++] = Memory[PC++] == 0 ? A & 0xFF : B & 0xFF;
			break;
		case POP8:
			if (Memory[PC++] == 0)
				A = Stack[--SP];
			else
				B = Stack[--SP];
			break;
		case PUSH16:
			Register = Memory[PC++];
			Stack[SP++] = Register == 0 ? A & 0xFF : B & 0xFF;
			Stack[SP++] = Register == 0 ? (A & 0xFF00) >> 8 : (B & 0xFF00) >> 8;
			break;
		case PUSH32:
			Register = Memory[PC++];
			Stack[SP++] = Register == 0 ? A & 0xFF : B & 0xFF;
			Stack[SP++] = Register == 0 ? (A & 0xFF00) >> 8 : (B & 0xFF00) >> 8;
			Stack[SP++] = Register == 0 ? (A & 0xFF0000) >> 16 : (B & 0xFF0000) >> 16;
			Stack[SP++] = Register == 0 ? (A & 0xFF000000) >> 24 : (B & 0xFF000000) >> 24;
			break;
		case POP16:
			Register = Memory[PC++];
			if (Register == 0)
			{
				A = 0;
				A |= Stack[--SP] << 8;
				A |= Stack[--SP];
			}
			else
			{
				B = 0;
				B |= Stack[--SP] << 8;
				B |= Stack[--SP];
			}
			break;
		case POP32:
			Register = Memory[PC++];
			if (Register == 0)
			{
				A = 0;
				A |= Stack[--SP] << 24;
				A |= Stack[--SP] << 16;
				A |= Stack[--SP] << 8;
				A |= Stack[--SP];
			}
			else
			{
				B = 0;
				B |= Stack[--SP] << 24;
				B |= Stack[--SP] << 16;
				B |= Stack[--SP] << 8;
				B |= Stack[--SP];
			}
			break;
		case ADD8:
		case ADD16:
		case ADD32:
			if (Memory[PC++] == 0)
				A += Memory[PC++] == 0 ? A : B;
			else
				B += Memory[PC++] == 0 ? A : B;
			break;
		case SUB8:
		case SUB16:
		case SUB32:
			if (Memory[PC++] == 0)
				A -= Memory[PC++] == 0 ? A : B;
			else
				B -= Memory[PC++] == 0 ? A : B;
			break;
		case MUL8:
		case MUL16:
		case MUL32:
			if (Memory[PC++] == 0)
				A *= Memory[PC++] == 0 ? A : B;
			else
				B *= Memory[PC++] == 0 ? A : B;
			break;
		case DIV8:
		case DIV16:
		case DIV32:
			if (Memory[PC++] == 0)
				A /= Memory[PC++] == 0 ? A : B;
			else
				B /= Memory[PC++] == 0 ? A : B;
			break;
		case LIT32:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Rd = 0;
			Rd |= Memory[PC++];
			Rd |= Memory[PC++] << 8;
			Rd |= Memory[PC++] << 16;
			Rd |= Memory[PC++] << 24;
			break;
		}
		case LIT16:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Rd = 0;
			Rd |= Memory[PC++];
			Rd |= Memory[PC++] << 8;
			break;
		}
		case LIT8:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Rd = 0;
			Rd |= Memory[PC++];
			break;
		}
		case LDR32:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			int Address = Memory[PC++];
			Address |= Memory[PC++] << 8;
			Address |= Memory[PC++] << 16;
			Address |= Memory[PC++] << 24;
			Rd = Memory[Address++];
			Rd |= Memory[Address++] << 8;
			Rd |= Memory[Address++] << 16;
			Rd |= Memory[Address++] << 32;
			break;
		}
		case LDR16:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			int Address = Memory[PC++];
			Address |= Memory[PC++] << 8;
			Address |= Memory[PC++] << 16;
			Address |= Memory[PC++] << 24;
			Rd = Memory[Address++];
			Rd |= Memory[Address++] << 8;
			break;
		}
		case LDR8:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			int Address = Memory[PC++];
			Address |= Memory[PC++] << 8;
			Address |= Memory[PC++] << 16;
			Address |= Memory[PC++] << 24;
			Rd = Memory[Address++];
			break;
		}
		case STR32:
		{
			int Reg = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			int Address = Memory[PC++];
			Address |= Memory[PC++] << 8;
			Address |= Memory[PC++] << 16;
			Address |= Memory[PC++] << 24;
			Memory[Address++] = Rs & 0xFF;
			Memory[Address++] = (Rs & 0xFF00) >> 8;
			Memory[Address++] = (Rs & 0xFF0000) >> 16;
			Memory[Address++] = (Rs & 0xFF000000) >> 24;
			break;
		}
		case STR16:
		{
			int Reg = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			int Address = Memory[PC++];
			Address |= Memory[PC++] << 8;
			Address |= Memory[PC++] << 16;
			Address |= Memory[PC++] << 24;
			Memory[Address++] = Rs & 0xFF;
			Memory[Address++] = (Rs & 0xFF00) >> 8;
			break;
		}
		case STR8:
		{
			int Reg = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			int Address = Memory[PC++];
			Address |= Memory[PC++] << 8;
			Address |= Memory[PC++] << 16;
			Address |= Memory[PC++] << 24;
			Memory[Address++] = Rs & 0xFF;
			break;
		}
		case LDRSP8:
		{
			int Reg = Memory[PC++];
			int Diff = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Rs = 0;
			Rs += Stack[SP - Diff];
			break;
		}
		case LDRSP16:
		{
			int Reg = Memory[PC++];
			int Diff = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Rs = 0;
			Rs += Stack[SP - Diff];
			Rs += Stack[SP - Diff + 1] << 8;
			break;
		}
		case LDRSP32:
		{

			int Reg = Memory[PC++];
			int Diff = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Rs = 0;
			Rs += Stack[SP - Diff];
			Rs += Stack[SP - Diff + 1] << 8;
			Rs += Stack[SP - Diff + 2] << 16;
			Rs += Stack[SP - Diff + 3] << 24;
			break;
		}
		case STRSP32:
		{
			int Reg = Memory[PC++];
			int Diff = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Stack[SP - Diff] = Rs & 0xFF;
			Stack[SP - Diff + 1] = (Rs & 0xFF00) >> 8;
			Stack[SP - Diff + 2] = (Rs & 0xFF0000) >> 16;
			Stack[SP - Diff + 3] = (Rs & 0xFF000000) >> 24;
			break;
		}
		case STRSP16:
		{
			int Reg = Memory[PC++];
			int Diff = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Stack[SP - Diff] = Rs & 0xFF;
			Stack[SP - Diff + 1] = (Rs & 0xFF00) >> 8;
			break;
		}
		case STRSP8:
		{
			int Reg = Memory[PC++];
			int Diff = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Stack[SP - Diff] = Rs & 0xFF;
			break;
		}
		case MOV8:
		case MOV16:
		case MOV32:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Reg = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Rd = Rs;
			break;
		}
		case RET:
		{
		//	std::cout << "SP == " << SP << std::endl;
			PC = 0;
			PC |= Stack[--SP] << 24;
			PC |= Stack[--SP] << 16;
			PC |= Stack[--SP] << 8;
			PC |= Stack[--SP];
		//	std::cout << "Returning to " << std::hex << PC << std::dec << std::endl;
			break;
		}
		case OR8:
		case OR16:
		case OR32:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Reg = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Rd = Rd | Rs;
			break;
		}
		case AND8:
		case AND16:
		case AND32:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Reg = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Rd = Rd & Rs;
			break;
		}
		case XOR8:
		case XOR16:
		case XOR32:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Reg = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Rd = Rd ^ Rs;
			break;
		}

		case NOT8:
		case NOT16:
		case NOT32:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Rd = ~Rd;
			break;
		}
		case LOR:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Reg = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Rd = Rd || Rs;
			break;
		}
		case LAND:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Reg = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Rd = Rd && Rs;
			break;
		}
		case LNOT:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Rd = !Rd;
			break;
		}
		case JMP:
		{
			int Address = Memory[PC++];
			Address |= Memory[PC++] >> 8;
			Address |= Memory[PC++] >> 16;
			Address |= Memory[PC++] >> 24;
			PC = Address;
			break;
		}
		case JMPZ:
		{
			int Address = Memory[PC++];
			Address |= Memory[PC++] >> 8;
			Address |= Memory[PC++] >> 16;
			Address |= Memory[PC++] >> 24;
			if (A == 0)
				PC = Address;
			break;
		}
		case LT:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Reg = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Rd = Rd < Rs;
			break;
		}
		case GT:
		{
			int Reg = Memory[PC++];
			long& Rd = (Reg == 0) ? A : B;
			Reg = Memory[PC++];
			long& Rs = (Reg == 0) ? A : B;
			Rd = Rd > Rs;
			break;
		}
		case CALLA:
		{
			//Need to push the value of PC onto the stack
			Stack[SP++] = PC & 0xFF;
			Stack[SP++] = (PC & 0xFF00) >> 8;
			Stack[SP++] = (PC & 0xFF0000) >> 16;
			Stack[SP++] = (PC & 0xFF000000) >> 24;
			PC = A;
			break;
		}
		case GETINT:
		{
			std::cin >> A;
			break;
		}
		case PRINTIA:
		{
			std::cout << std::dec << (int)A;
			break;
		}
		case PRINTSA:
		{
			int Index = A;
			while (Memory[Index] != 0)
			{
				if ((char)Memory[Index] == '\\')
				{
					if ((char)Memory[Index + 1] == '\\')
					{
						std::cout << '\\';
						Index += 2;
					}
					else if ((char)Memory[Index + 1] == 'n')
					{
						std::cout << std::endl;
						Index += 2;
					}
				}
				else
				{
					std::cout << (char)Memory[Index++];
				}
			}
			break;
		}
		case EXIT:
		{
			delete Stack;
			return (int)A;
		}
		}
	}
	
	delete Stack;

	std::cout << std::endl << "Stack underflow! (Probably caused by returning from the first executed function)" << std::endl;
	std::cout << "A: " << A << std::endl;
	std::cout << "B: " << B << std::endl;


	return (int)A;
}