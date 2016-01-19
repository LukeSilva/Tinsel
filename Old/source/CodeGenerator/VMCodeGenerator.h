#pragma once
#include "DefaultCodeGenerator.h"
class VMCodeGenerator :
	public DefaultCodeGenerator
{
private:
	enum Instruction
	{
		PUSH8 = 0x00, POP8 = 0x01, ADD8 = 0x02, SUB8 = 0x03, MUL8 = 0x04, DIV8 = 0x05, LIT8 = 0x06, LDR8 = 0x07, STR8 = 0x08, MOV8 = 0x09, AND8 = 0x0A, OR8 = 0x0B, XOR8 = 0x0C, NOT8 = 0x0D, LDRSP8 = 0x0E, STRSP8 = 0x0F,
		PUSH16 = 0x10, POP16 = 0x11, ADD16 = 0x12, SUB16 = 0x13, MUL16 = 0x14, DIV16 = 0x15, LIT16 = 0x16, LDR16 = 0x17, STR16 = 0x18, MOV16 = 0x19, AND16 = 0x1A, OR16 = 0x1B, XOR16 = 0x1C, NOT16 = 0x1D, LDRSP16 = 0x1E, STRSP16 = 0x1F,
		PUSH32 = 0x20, POP32 = 0x21, ADD32 = 0x22, SUB32 = 0x23, MUL32 = 0x24, DIV32 = 0x25, LIT32 = 0x26, LDR32 = 0x27, STR32 = 0x28, MOV32 = 0x29, AND32 = 0x2A, OR32 = 0x2B, XOR32 = 0x2C, NOT32 = 0x2D, LDRSP32 = 0x2E, STRSP32 = 0x2F,
		LOR = 0x30, LAND = 0x31, LNOT = 0x32, LT = 0x33, GT = 0x34, JMP = 0x35, JMPZ = 0x36,
		CALL = 0x40, CALLA = 0x41,
		PRINTSA = 0xE0, GETINT = 0xE1, PRINTIA = 0xE2,
		EXIT	= 0xFD,	RET		= 0xFE, NOP		= 0xFF
	};
	unsigned char* CodePage;
	unsigned char* DataPage;
	const static int SizeOfCodePage = 0xFFFF;
	const static int SizeOfDataPage = 0xFFFF;
	int EntryPoint;


	//Code Pointer
	int CP;
	//Data Pointer
	int DP;
	//Error checker
	int Error;
	//Boilerplate
	void AttemptPushCodePage(unsigned char Byte);
public:

	//Data
	long AddDataByte(char Byte);
	//Bss
	long AddBssByte();
	long AddBssDByte();
	long AddBssQByte();
	long AddBssInt();
	//Function
	long GetFunctionAddress();

	//CodeGen
	void LoadLiteralIntoMainRegister(Tinsel::VariableType LiteralType, long Value);
	void EmitReturn();
	void LoadVariableIntoMainRegister(Tinsel::VariableType VariableType, long Address, Tinsel::VariableType LoadAsType);
	void LoadVariableRelativeStackIntoMainRegister(Tinsel::VariableType VariableType, long StackOffset, Tinsel::VariableType LoadAsType);
	void StoreMainRegisterIntoVariable(Tinsel::VariableType VariableType, long Address, Tinsel::VariableType MainRegisterType);
	void StoreMainRegisterRelativeStack(Tinsel::VariableType VariableType, long StackOffset, Tinsel::VariableType MainRegisterType);
	void PushMainRegisterOntoStack(Tinsel::VariableType MainRegisterType);
	void PopSecondaryRegisterFromStack(Tinsel::VariableType StackVariableType);
	void AddMainAndSecondaryRegisters(Tinsel::VariableType TypeOfRegisters);
	void SubMainFromSecondary(Tinsel::VariableType TypeOfRegisters);
	void MultiplyMainAndSecondary(Tinsel::VariableType RegisterTypes);
	void DivideSecondaryByMain(Tinsel::VariableType RegisterTypes);

	void AndMainAndSecondary(Tinsel::VariableType RegisterTypes);
	void OrMainAndSecondary(Tinsel::VariableType RegisterTypes);
	void XorMainAndSecondary(Tinsel::VariableType RegisterType);
	void NotMainRegister(Tinsel::VariableType RegisterTypes);
	void LogicalAndMainAndSecondary(Tinsel::VariableType RegisetrTypes);
	void LogicalOrMainAndSecondary(Tinsel::VariableType RegisterTypes);
	void LogicalNotMain(Tinsel::VariableType RegisterTypes);
	void LogicalLessThan(Tinsel::VariableType RegisterTypes, bool inv);
	void LogicalGreaterThan(Tinsel::VariableType RegisterTypes, bool inv);
	void CallMainRegister();
	void PrintCharArrayInMain();
	void GetIntIntoMain();
	void PrintIntInMain();
	void ExitProgram();


	long CreateFutureJump(bool CheckForZFlag);
	void FillFutureJump(long JumpSource, long JumpTarget);

	void ChangeDataByte(long Address, char Byte);
	//Compilation
	bool CanCompileIntoFile(){ return true; }
	void CompileIntoFile(std::string filename);
	VMCodeGenerator();
	~VMCodeGenerator();
};

