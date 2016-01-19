#include "Tinsel.h"
#include "../CodeGenerator/DefaultCodeGenerator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <vector>


//Returns true if the string is an integer
bool Tinsel::IsStringInt(const std::string& s)
{
	//Returns false if the string is empty, can't possibly be an integer.
	if (s.empty()) return false;

	//Attempt to convert the string to a long
	char * p;
	strtol(s.c_str(), &p, 0);

	//If the conversion used up all characters in the string, the string was an integer.
	return (*p == 0);
}

//Returns true if an error has occured
bool Tinsel::Error()
{
	return ErrorCode != 0;
}

//Returns true if the token has the type of a variable.
bool Tinsel::IsVariableType(Tinsel::Token Token)
{
	switch (Token.Type)
	{
	case TOKEN_U8:
	case TOKEN_S8:
	case TOKEN_U16:
	case TOKEN_S16:
	case TOKEN_U32:
	case TOKEN_S32:
	case TOKEN_STRING:
	case TOKEN_INT:
		return true;
		break;
	default:
		return false;
	}
	return false; // Just in case ;)
}

//Converts Tinsel::Token::TokenType to Tinsel::VariableType
Tinsel::VariableType Tinsel::GetVariableTypeFromToken(Tinsel::Token Token)
{
	Tinsel::VariableType VariableType;
	switch (Token.Type)
	{
	case TOKEN_U8:
		VariableType = U8;
		break;
	case TOKEN_S8:
		VariableType = S8;
		break;
	case TOKEN_U16:
		VariableType = U16;
		break;
	case TOKEN_S16:
		VariableType = S16;
		break;
	case TOKEN_U32:
		VariableType = U32;
		break;
	case TOKEN_S32:
		VariableType = S32;
		break;
	case TOKEN_INT:
		VariableType = INT;
		break;
	case TOKEN_STRING:
		VariableType = STRING;
		break;
	default:
		VariableType = UNKNOWN_TYPE;
		break;
	}
	return VariableType;
}


//Ensures that the next token read from a stream is what is expected.
bool Tinsel::Expect(std::ifstream& Stream, Tinsel::TokenTypes Type)
{
	//Create a token, and read it from the stream
	Tinsel::Token t;
	t.ReadFromFile(Stream);

	//If this token is not expected error.
	if (t.Type != Type)
	{
		std::cerr << "[Tinsel] Unexpected Token" << std::endl;
		ErrorCode = 10;
		return true;
	}
	
	//Otherwise continue
	return false;
}

//Push the main register onto the stack, taking note of where local variables are stored.
void Tinsel::PushMainRegister(Tinsel::VariableType MainRegisterType, int Bytes)
{
	
	StackOffset += Bytes;
	//std::cout << "[TINSEL] Push stack offset " << StackOffset << std::endl;
	CodeGenerator->PushMainRegisterOntoStack(MainRegisterType);

}


//Helper function, obtains number of bytes taken by a variable type
void Tinsel::PushMainRegister(Tinsel::VariableType MainRegisterType)
{
	int Bytes = 0;
	switch (MainRegisterType)
	{
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::STRING:
		Bytes = 4;
		break;
	case Tinsel::VariableType::S16:
	case Tinsel::VariableType::U16:
		Bytes = 2;
		break;
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		Bytes = 1;
		break;
	}
	PushMainRegister(MainRegisterType, Bytes);
}

//Pops the main register from the stack, taking note of where local variables are stored.
void Tinsel::PopSecondaryRegister(Tinsel::VariableType SecondaryRegisterType, int Bytes)
{
	StackOffset -= Bytes;
//	std::cout << "[TINSEL] Pop stack offset " << StackOffset << std::endl;
	CodeGenerator->PopSecondaryRegisterFromStack(SecondaryRegisterType);
}

//Helper function, obtains number of bytes taken by a variable type
void Tinsel::PopSecondaryRegister(Tinsel::VariableType MainRegisterType)
{
	int Bytes = 0;
	switch (MainRegisterType)
	{
	case Tinsel::VariableType::INT:
	case Tinsel::VariableType::S32:
	case Tinsel::VariableType::U32:
	case Tinsel::VariableType::STRING:
		Bytes = 4;
		break;
	case Tinsel::VariableType::S16:
	case Tinsel::VariableType::U16:
		Bytes = 2;
		break;
	case Tinsel::VariableType::U8:
	case Tinsel::VariableType::S8:
		Bytes = 1;
		break;
	}
	PopSecondaryRegister(MainRegisterType, Bytes);
}

//Resets the StackOffset to a particular point. (Removing all the local variables after a code block).
void Tinsel::ZeroStackOffset(int TargetStackOffset)
{
	//While the stack offset is 4 bytes or more, pop a U32 (4 bytes long)
	while (StackOffset >= TargetStackOffset + 4)
	{
		PopSecondaryRegister(Tinsel::VariableType::U32);
	}

	//While the stack offset is 2 bytes or more, pop a U16 (2 bytes long)
	while (StackOffset >= TargetStackOffset + 2)
		PopSecondaryRegister(Tinsel::VariableType::U16);
	
	//While the stack offset is 1 bytes or more, pop a U8 (1 bytes long)
	while (StackOffset >= TargetStackOffset + 1)
		PopSecondaryRegister(Tinsel::VariableType::U8);
}

