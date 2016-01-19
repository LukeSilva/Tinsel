#include "VMCodeGenerator.h"
#include <fstream>

VMCodeGenerator::VMCodeGenerator()
{
	//Allows DefaultCodeGenerator to implement a lot of data functions
	IsLittleEndian = true;


	CodePage = new unsigned char[SizeOfCodePage + 1];
	DataPage = new unsigned char[SizeOfDataPage + 1];
	EntryPoint = 0;
	Error = 0;
}


VMCodeGenerator::~VMCodeGenerator()
{
	delete[] CodePage;
	delete[] DataPage;
}

long VMCodeGenerator::AddDataByte(char Byte)
{
	if (DP > SizeOfDataPage) return 0;

	long Address = SizeOfCodePage + DP + 1;
	DataPage[DP++] = Byte;
	return Address;
}

void VMCodeGenerator::PrintIntInMain()
{
	AttemptPushCodePage(PRINTIA);
}

void VMCodeGenerator::ExitProgram()
{
	AttemptPushCodePage(EXIT);
}

void VMCodeGenerator::PrintCharArrayInMain()
{
	AttemptPushCodePage(PRINTSA);
}

void VMCodeGenerator::GetIntIntoMain()
{
	AttemptPushCodePage(GETINT);
}

void VMCodeGenerator::ChangeDataByte(long Address, char Byte)
{
//	std::cout << "Changing data to " << (int)Byte << " @ " << Address << std::endl;
	DataPage[Address - SizeOfCodePage - 1] = Byte;
//	std::cout << "Data is " << (int)DataPage[Address - SizeOfCodePage - 1] << std::endl;
}

long VMCodeGenerator::AddBssByte()
{
	return AddDataByte(0);
}

long VMCodeGenerator::AddBssDByte()
{
	return AddDataDByte(0);
}

long VMCodeGenerator::AddBssQByte()
{
	return AddDataQByte(0);
}

long VMCodeGenerator::AddBssInt()
{
	return AddDataInt(0);
}

long VMCodeGenerator::GetFunctionAddress()
{
	return CP;
}

void VMCodeGenerator::AttemptPushCodePage(unsigned char Byte)
{
	if (CP > SizeOfCodePage)
	{
		Error = 1;
		std::cerr << "Ran out of space in code page" << std::endl;
		return;
	}
	CodePage[CP++] = Byte;
}

void VMCodeGenerator::LoadLiteralIntoMainRegister(Tinsel::VariableType VarType, long Value)
{
	switch (VarType)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(LIT8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(Value & 0xFF);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(LIT16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(Value & 0xFF);
		AttemptPushCodePage((Value >> 8) & 0xFF);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(LIT32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(Value & 0xFF);
		AttemptPushCodePage((Value >> 8) & 0xFF);
		AttemptPushCodePage((Value >> 16) & 0xFF);
		AttemptPushCodePage((Value >> 24) & 0xFF);
		break;
	}
}

void VMCodeGenerator::EmitReturn()
{
	AttemptPushCodePage(RET);
}

void VMCodeGenerator::LoadVariableIntoMainRegister(Tinsel::VariableType VariableType, long Address, Tinsel::VariableType LoadAsType)
{
	switch (VariableType)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(LDR8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(Address & 0xFF);
		AttemptPushCodePage((Address >> 8) & 0xFF);
		AttemptPushCodePage((Address >> 16) & 0xFF);
		AttemptPushCodePage((Address >> 24) & 0xFF);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(LDR16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(Address & 0xFF);
		AttemptPushCodePage((Address >> 8) & 0xFF);
		AttemptPushCodePage((Address >> 16) & 0xFF);
		AttemptPushCodePage((Address >> 24) & 0xFF);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(LDR32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(Address & 0xFF);
		AttemptPushCodePage((Address >> 8) & 0xFF);
		AttemptPushCodePage((Address >> 16) & 0xFF);
		AttemptPushCodePage((Address >> 24) & 0xFF);
		break;

	}
}

void VMCodeGenerator::StoreMainRegisterIntoVariable(Tinsel::VariableType VariableType, long Address, Tinsel::VariableType LoadAsType)
{
	switch (VariableType)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(STR8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(Address & 0xFF);
		AttemptPushCodePage((Address >> 8) & 0xFF);
		AttemptPushCodePage((Address >> 16) & 0xFF);
		AttemptPushCodePage((Address >> 24) & 0xFF);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(STR16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(Address & 0xFF);
		AttemptPushCodePage((Address >> 8) & 0xFF);
		AttemptPushCodePage((Address >> 16) & 0xFF);
		AttemptPushCodePage((Address >> 24) & 0xFF);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(STR32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(Address & 0xFF);
		AttemptPushCodePage((Address >> 8) & 0xFF);
		AttemptPushCodePage((Address >> 16) & 0xFF);
		AttemptPushCodePage((Address >> 24) & 0xFF);
		break;

	}
}

void VMCodeGenerator::PushMainRegisterOntoStack(Tinsel::VariableType VariableType)
{
	switch (VariableType)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(PUSH8);
		AttemptPushCodePage(0);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(PUSH16);
		AttemptPushCodePage(0);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(PUSH32);
		AttemptPushCodePage(0);
		break;
	}
}

void VMCodeGenerator::PopSecondaryRegisterFromStack(Tinsel::VariableType VariableType)
{
	switch (VariableType)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(POP8);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(POP16);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(POP32);
		AttemptPushCodePage(1);
		break;
	}
}

void VMCodeGenerator::AddMainAndSecondaryRegisters(Tinsel::VariableType VariableType)
{
	switch (VariableType)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(ADD8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(ADD16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(ADD32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	}
}

void VMCodeGenerator::MultiplyMainAndSecondary(Tinsel::VariableType VariableType)
{
	switch (VariableType)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(MUL8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(MUL16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(MUL32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	}
}

void VMCodeGenerator::SubMainFromSecondary(Tinsel::VariableType VariableType)
{
	switch (VariableType)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(SUB8);
		AttemptPushCodePage(1);
		AttemptPushCodePage(0);
		AttemptPushCodePage(MOV8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(SUB16);
		AttemptPushCodePage(1);
		AttemptPushCodePage(0);
		AttemptPushCodePage(MOV16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(SUB32);
		AttemptPushCodePage(1);
		AttemptPushCodePage(0);
		AttemptPushCodePage(MOV32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	}
}

void VMCodeGenerator::StoreMainRegisterRelativeStack(Tinsel::VariableType VariableType, long StackOffset, Tinsel::VariableType MainRegisterType)
{
	switch (VariableType)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(STRSP8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(StackOffset&0xFF);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(STRSP16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(StackOffset & 0xFF);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(STRSP32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(StackOffset & 0xFF);
		break;
	}
}

void VMCodeGenerator::LoadVariableRelativeStackIntoMainRegister(Tinsel::VariableType VariableType, long StackOffset, Tinsel::VariableType MainRegisterType)
{
	switch (VariableType)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(LDRSP8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(StackOffset & 0xFF);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(LDRSP16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(StackOffset & 0xFF);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(LDRSP32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(StackOffset & 0xFF);
		break;
	}
}
void VMCodeGenerator::DivideSecondaryByMain(Tinsel::VariableType VariableType)
{
	switch (VariableType)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(DIV8);
		AttemptPushCodePage(1);
		AttemptPushCodePage(0);
		AttemptPushCodePage(MOV8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(DIV16);
		AttemptPushCodePage(1);
		AttemptPushCodePage(0);
		AttemptPushCodePage(MOV16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(DIV32);
		AttemptPushCodePage(1);
		AttemptPushCodePage(0);
		AttemptPushCodePage(MOV32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	}
}

void VMCodeGenerator::AndMainAndSecondary(Tinsel::VariableType RegisterTypes)
{
	switch (RegisterTypes)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(AND8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(AND16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(AND32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	}
}

void VMCodeGenerator::OrMainAndSecondary(Tinsel::VariableType RegisterTypes)
{
	switch (RegisterTypes)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(OR8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(OR16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(OR32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	}
}
void VMCodeGenerator::XorMainAndSecondary(Tinsel::VariableType RegisterTypes)
{
	switch (RegisterTypes)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(XOR8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(XOR16);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(XOR32);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
		break;
	}
}

void VMCodeGenerator::NotMainRegister(Tinsel::VariableType RegisterTypes)
{
	switch (RegisterTypes)
	{
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		AttemptPushCodePage(NOT8);
		AttemptPushCodePage(0);
		break;
	case Tinsel::VariableType::U16:
	case Tinsel::VariableType::S16:
		AttemptPushCodePage(NOT16);
		AttemptPushCodePage(0);
		break;
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::STRING:
		AttemptPushCodePage(NOT32);
		AttemptPushCodePage(0);
		break;
	}
}

void VMCodeGenerator::LogicalAndMainAndSecondary(Tinsel::VariableType RegisterTypes)
{
	AttemptPushCodePage(LAND);
	AttemptPushCodePage(0);
	AttemptPushCodePage(1);
}

void VMCodeGenerator::LogicalOrMainAndSecondary(Tinsel::VariableType RegisterTypes)
{
	AttemptPushCodePage(LOR);
	AttemptPushCodePage(0);
	AttemptPushCodePage(1);
}

void VMCodeGenerator::LogicalNotMain(Tinsel::VariableType RegisterTypes)
{
	AttemptPushCodePage(LNOT);
	AttemptPushCodePage(0);
}

void VMCodeGenerator::LogicalLessThan(Tinsel::VariableType RegisterTypes, bool inv)
{
	AttemptPushCodePage(LT);
	AttemptPushCodePage(inv ? 1 : 0);
	AttemptPushCodePage(inv ? 0 : 1);
	if (inv)
	{
		AttemptPushCodePage(MOV8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
	}
}

void VMCodeGenerator::LogicalGreaterThan(Tinsel::VariableType RegisterTypes, bool inv)
{
	AttemptPushCodePage(GT);
	AttemptPushCodePage(inv ? 1 : 0);
	AttemptPushCodePage(inv ? 0 : 1);
	if (inv)
	{
		AttemptPushCodePage(MOV8);
		AttemptPushCodePage(0);
		AttemptPushCodePage(1);
	}
}

long VMCodeGenerator::CreateFutureJump(bool ReqZ)
{
	long ret = CP;
	if (ReqZ)
		AttemptPushCodePage(JMPZ);
	else
		AttemptPushCodePage(JMP);

	AttemptPushCodePage(5);
	AttemptPushCodePage(0);
	AttemptPushCodePage(0);
	AttemptPushCodePage(0);
	return ret;
}

void VMCodeGenerator::CallMainRegister()
{
	AttemptPushCodePage(CALLA);
}

void VMCodeGenerator::FillFutureJump(long Source, long Target)
{
	if (CodePage[Source] != JMP && CodePage[Source] != JMPZ)
	{
		std::cerr << "[VM-CODE-GEN] Attempting to fill future jump, but no jump instruction written to code page!" << std::endl;
		Error = 1;
		return;
	}
	CodePage[Source + 1] = Target & 0xFF;
	CodePage[Source + 2] = (Target & 0xFF00) >> 8;
	CodePage[Source + 3] = (Target & 0xFF0000) >> 16;
	CodePage[Source + 4] = (Target & 0xFF000000) >> 24;
}

void VMCodeGenerator::CompileIntoFile(std::string filename)
{
//	std::cout << "Compiling into " << filename << std::endl;
	
	std::ofstream ofs(filename, std::ofstream::out | std::ofstream::binary);
	ofs.imbue(std::locale::classic());

	//Print Version
	ofs.put('T');
	ofs.put(0x01);


	//Print CodePageSize
	ofs.put(SizeOfCodePage & 0xFF);
	ofs.put((SizeOfCodePage >> 8) & 0xFF);
	
	//Print DataPageSize
	ofs.put(SizeOfDataPage & 0xFF);
	ofs.put((SizeOfDataPage >> 8) & 0xFF);

	//Print EntryPoint
	ofs.put(EntryPoint & 0xFF);
	ofs.put((EntryPoint >> 8) & 0xFF);

	//Now store the Code Page
	for (int i = 0; i <= SizeOfCodePage; ++i)
	{
		ofs.put(CodePage[i]);
	}

	//Now store the Data Page
	for (int i = 0; i <= SizeOfDataPage; ++i)
	{
		ofs.put(DataPage[i]);
	}

	ofs.close();
}