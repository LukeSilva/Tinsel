#pragma once
#include "../Tinsel/Tinsel.h"
#include <iostream>
class DefaultCodeGenerator
{
public:
	/**  Here we have the default Code Generator for Tinsel
		 In order to use tinsel to compile code for a cpu, a couple of requirements have to be met:
			Ability to use a stack
			Ability to call and return, not just jump.
			Pointers must be able to be held in an integer type ( for now )

		Tinsel will compile for a RISC-like CPU, with at least 2 registers.
			Data will be loaded into registers before operations are performed between the two.
			For cpus that support direct memory-memory operations, optimisations could be performed to utlised these in _this_ file.

		If a cpu does not have the ability to use a stack or call and return, these requirements could be implemented in software.

		Seperate CPU Targets should use a modified version of these place holders
		Follow the instructions in the comments to properly implement the required functions

	**/

	long NextBssByte = 0;
	long NextDataByte = 0;
	long NextCodeByte = 0;

	// Setup the Code Generator, must not save anything to file. Is only really ment to setup any internal variables that need to be initialised.
	DefaultCodeGenerator()
	{
		IsLittleEndian = true;
	}

	//Deinit the code generator, just delete any internals that you need to delete.
	~DefaultCodeGenerator()
	{
	}

	//Is used in placeholders to properly add words to the executable.
	//Set and you won't have to replace as many functions.
	bool IsLittleEndian;


	//Changes a preexisting data byte stored in the data page.
	virtual void ChangeDataByte(long Address, char Byte)
	{
		std::cout << "[CODE-GEN] Changing data byte @ " << Address << " for " << std::hex << ((int)Byte & 0xFF) << std::dec << std::endl;
	}

	//Emits code to print an array of chars, where the pointer of which is stored in the main register.
	virtual void PrintCharArrayInMain()
	{
		std::cout << "[CODE-GEN] Printing Char Array in main " << std::endl;
	}

	//Obtains an integer from stdin, storeing the result in the main register
	virtual void GetIntIntoMain()
	{
		std::cout << "[CODE-GEN] Getting Int into main " << std::endl;
	}

	//Prints the interger stored in main as in integer to stdout.
	virtual void PrintIntInMain()
	{
		std::cout << "[CODE-GEN] Printing Int in main " << std::endl;
	}

	//Quits the program safely.
	virtual void ExitProgram()
	{
		std::cout << "[CODE-GEN] Exitting " << std::endl;
	}

	// Adds a byte of data to the executable, returning the address to it
	virtual long AddDataByte(char Byte)
	{
		int i = Byte;
		std::cout << "[CODE-GEN] Add data byte " << std::hex << (i & 0xFF) << std::dec << std::endl;
		return NextDataByte++;
	}
	
	// Adds a byte of data to the bss segment, returning the address of it
	virtual long AddBssByte()
	{
		std::cout << "[CODE-GEN] Add bss byte" << std::endl;
		return NextBssByte++;
	}

	// Adds two bytes of data to the executable, returning the address of it
	// This function does not need to be implemented if the IsLittleEndian flag is properly set.
	virtual long AddDataDByte(short DByte)
	{
		int Address = 0;
		if (IsLittleEndian)
		{
			Address = AddDataByte(DByte & 0xFF);
			AddDataByte((DByte & 0xFF00) >> 8);
		}
		else
		{
			Address = AddDataByte((DByte & 0xFF00) >> 8);
			AddDataByte(DByte & 0xFF);
		}
		return Address;
	}

	// Adds two bytes of data to the bss segment, returning the address of it
	// This function does not need to be implemented if the IsLittleEndian flag is properly set.
	virtual long AddBssDByte()
	{
		std::cout << "[CODE-GEN] Add bss dbyte" << std::endl;
		return 0;
	}

	// Adds four bytes of data to the executable, returning the address to it.
	// This function does not need to be implemented if the IsLittleEndian flag is properly set.
	virtual long AddDataQByte(long QByte)
	{
		int Address = 0;
		if (IsLittleEndian)
		{
			Address = AddDataDByte(QByte & 0xFFFF);
			AddDataDByte((QByte & 0xFFFF0000) >> 16);
		}
		else
		{
			Address = AddDataDByte((QByte & 0xFFFF0000) >> 16);
			AddDataDByte(QByte & 0xFFFF);
		}
		return Address;
	}

	// Adds four bytes of data to the bss segment, returning the address of it.
	// This function does not need to be implemented if the IsLittleEndian flag is properly set.
	virtual long AddBssQByte()
	{
		std::cout << "[CODE-GEN] Add bss qbyte" << std::endl;
		return 0;
	}

	// Adds a integer of data to the bss segment, and returns the address of it
	virtual long AddBssInt()
	{
		std::cout << "[CODE-GEN] Add bss int" << std::endl;
		return 0;
	}

	// Adds an integer of data to the executable, and returns the address of it
	virtual long AddDataInt(long Integer)
	{
		return AddDataQByte(Integer);
	}

	// Returns the next address in the code segment for jumping
	virtual long GetFunctionAddress()
	{
		return 0;
	}

	// Emits code to load literal into the main register
	virtual void LoadLiteralIntoMainRegister(Tinsel::VariableType LiteralType, long Value)
	{
		std::cout << "[CODE-GEN] Loading \"" << Value << "\" Into Main Register" << std::endl;
	}

	// Emits code to return from  a subroutine
	virtual void EmitReturn()
	{
		std::cout << "[CODE-GEN] Emitting Return" << std::endl;
	}

	// Emits code to load a value into the main register from an address, of a specific size
	virtual void LoadVariableIntoMainRegister(Tinsel::VariableType VariableType, long Address,Tinsel::VariableType LoadAsType)
	{
		std::cout << "[CODE-GEN] Loading variable from address \"" << Address << "\" Into Main Register " << std::endl;
	}

	// Emits code to store the main register as a certain variable into the address of a variable
	virtual void StoreMainRegisterIntoVariable(Tinsel::VariableType VariableType, long Address, Tinsel::VariableType MainRegisterType)
	{
		std::cout << "[CODE-GEN] Storing Main Register into \"" << Address << "\"" << std::endl;
	}
	
	// Emits code to store the main register to a position relative to the stack.
	virtual void StoreMainRegisterRelativeStack(Tinsel::VariableType VariableType, long StackOffset, Tinsel::VariableType MainRegisteType)
	{
		std::cout << "[CODE-GEN] Storing Main Register relative to the stack \"" << StackOffset << "\"" << std::endl;
	}

	// Emits code to load the main register from a position relative to the stack.
	virtual void LoadVariableRelativeStackIntoMainRegister(Tinsel::VariableType VariableType, long StackOffset, Tinsel::VariableType MainRegisterType)
	{
		std::cout << "[CODE-GEN] Loading Stack Relative into main register \"" << StackOffset << "\"" << std::endl;
	}

	// Emits code to push the main register onto the stack
	virtual void PushMainRegisterOntoStack(Tinsel::VariableType MainRegisterType)
	{
		std::cout << "[CODE-GEN] Pushing Main Register onto stack " << std::endl;
	}

	// Emits code to pop a variable from the stack into the secondary register
	virtual void PopSecondaryRegisterFromStack(Tinsel::VariableType StackVariableType)
	{
		std::cout << "[CODE-GEN] Popping Stack to Secondary Register " << std::endl;
	}

	// Emits code to add the main and secondary registers together
	virtual void AddMainAndSecondaryRegisters(Tinsel::VariableType TypeOfRegisters)
	{
		std::cout << "[CODE-GEN] Adding Main and Secondary Registers " << std::endl;
	}
	// Emits code to subtract the main register from the secondary register, leaving result in Main Register
	virtual void SubMainFromSecondary(Tinsel::VariableType TypeOfRegisters)
	{
		std::cout << "[CODE-GEN] Subtracting Main from Secondary " << std::endl;
	}

	// Emits code to multiply the main and secondary registers
	virtual void MultiplyMainAndSecondary(Tinsel::VariableType RegisterTypes)
	{
		std::cout << "[CODE-GEN] Multiplying main and secondary registers" << std::endl;
	}

	// Emits code to divide the secondary register by the main register
	virtual void DivideSecondaryByMain(Tinsel::VariableType RegisterTypes)
	{
		std::cout << "[CODE-GEN] Diving secondary register by the main register " << std::endl;
	}

	// Emits code to perform a bitwise and between the main and secondary registers, storing the result in the main register
	virtual void AndMainAndSecondary(Tinsel::VariableType RegisterTypes)
	{
		std::cout << "[CODE-GEN] bitwise and-ing the main and secondary registers." << std::endl;
	}

	// Emits code to perform a bitwise or between the main and secondary register, storing the result in the main register
	virtual void OrMainAndSecondary(Tinsel::VariableType RegisterTypes)
	{
		std::cout << "[CODE-GEN] bitwise or-ing the main and secondary registers. " << std::endl;
	}

	// Emits code to perform a bitwise exclusive or between the main and secondary registers, storing the result in the main register.
	virtual void XorMainAndSecondary(Tinsel::VariableType RegisterType)
	{
		std::cout << "[CODE-GEN] bitwise xoring the main and secondary registers. " << std::endl;
	}

	// Emits code to perform a bitwise not between the main, storing the result in the main register.
	virtual void NotMainRegister(Tinsel::VariableType RegisterTypes)
	{
		std::cout << "[CODE-GEN] Inverting the main register" << std::endl;
	}

	// Emits code to perform a logical and between the main and secondary registers.
	virtual void LogicalAndMainAndSecondary(Tinsel::VariableType RegisetrTypes)
	{
		std::cout << "[CODE-GEN] Logical and-ing main and secondary registers. " << std::endl;
	}

	// Emits code to perform a logical or between the main and secondary registers.
	virtual void LogicalOrMainAndSecondary(Tinsel::VariableType RegisterTypes)
	{
		std::cout << "[CODE-GEN] Logical or-ing main and secondary registers. " << std::endl;
	}

	// Emits code to perform invert the main register (boolean)
	virtual void LogicalNotMain(Tinsel::VariableType RegisterTypes)
	{
		std::cout << "[CODE-GEN] Inverting Main register - bool " << std::endl;
	}

	// Emits code to perform a less than comparision. If inv, Secondary < Main, otherwise Main < Secondary, the result of this should be stored in the main register (for JPZ)
	virtual void LogicalLessThan(Tinsel::VariableType RegisterTypes, bool inv)
	{
		std::cout << "[CODE-GEN] Performing Less than, inv = " << inv << std::endl;
	}

	virtual void CallMainRegister()
	{
		std::cout << "[CODE-GEN] Calling main Register " << std::endl;
	}

	// Emits code to perform a greater than comparision. If inv, Secondary > Main, otherwise Main > Secondary, the result of this should be stored in the main register (for JPZ)
	virtual void LogicalGreaterThan(Tinsel::VariableType RegisterTypes, bool inv)
	{
		std::cout << "[CODE-GEN] Performing Greater than, inv = " << inv << std::endl;
	}

	// Leaves blank space for a jump to a code point in the future, returning the address of the code position, for the jump point to be filled in later.
	// In a real situation you may want this to immediately emit code to jump to an errorhandler, in case that the jump point is not filled in later.
	// If this jump should only be performed on the main register being zero, the CheckForZFlag will be set
	virtual long CreateFutureJump(bool CheckForZFlag)
	{
		std::cout << "[CODE-GEN] Creating Future Jump space " << std::endl;
		return 0x4545;
	}

	// Fills in a jump command created with the previous function.
	virtual void FillFutureJump(long JumpSource, long JumpTarget)
	{
		std::cout << "[CODE-GEN] Filling in future jump, to " << JumpTarget << std::endl;
	}

	// Returns true/false to determine if you can execute code in memory (aka JIT)
	virtual bool CanExecute()
	{
		return false;
	}

	// Executes code in memory if applicable.
	virtual int Execute(long Address)
	{
		return -1;
	}

	//Returns true/false to determine if you can compile to code to a file
	virtual bool CanCompileIntoFile()
	{
		return false;
	}

	//Compiles code to a file, if applicable.
	virtual void CompileIntoFile(std::string filename)
	{

	}
};

