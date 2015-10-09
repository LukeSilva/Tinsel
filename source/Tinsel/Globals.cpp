#include "Tinsel.h"
#include "../CodeGenerator/DefaultCodeGenerator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <vector>

//Allows functions to reset their address (definition comes after first mention)
void Tinsel::ResetGlobalVariableAddress(std::string Scope, long Address, long Value)
{
	//[FIX-FOR-REAL] Allow big endianess
	CodeGenerator->ChangeDataByte(Address, Value & 0xFF);
	CodeGenerator->ChangeDataByte(Address + 1, (Value & 0xFF00) >> 8);
	CodeGenerator->ChangeDataByte(Address + 2, (Value & 0xFF0000) >> 16);
	CodeGenerator->ChangeDataByte(Address + 3, (Value & 0xFF000000) >> 24);
}


//Defines a variable in Data segment
void Tinsel::DefineGlobalVariableInData(std::string Scope, Tinsel::Variable Var, long Value, std::string StringValue)
{
	long StringAddress = -1;
	long Address;
	switch (Var.Type)
	{
	case U8:
	case S8:
		Address = CodeGenerator->AddDataByte((char)(Value & 0xFF));
		break;
	case U16:
	case S16:
		Address = CodeGenerator->AddDataDByte((short)(Value & 0xFFFF));
		break;
	case U32:
	case S32:
		Address = CodeGenerator->AddDataQByte(Value);
		break;
	case INT:
		//[FIX-FOR-REAL] Change to AddDataInt when finished with DefaultCodeGenerator.
		Address = CodeGenerator->AddDataQByte(Value);
		break;
	case STRING:
		//First add the string literal to the data segment (will not support utf8 characters atm)
		for (int i = 0; i < StringValue.length(); ++i)
		{
			long TmpAddress = CodeGenerator->AddDataByte(StringValue.at(i) & 0xFF);
			if (StringAddress == -1)
				StringAddress = TmpAddress;
		}
		//Ensure a null byte at the end of the string
		CodeGenerator->AddDataByte(0);
		//Then add the address
		Address = CodeGenerator->AddDataInt(StringAddress);
		break;
	default:
		std::cerr << "[Tinsel] Unknown variable type to define globally" << std::endl;
		ErrorCode = 4;
		return;
	}

	//Push into vector
	GlobalReferences.push_back(std::make_tuple(Scope, Var, Address));
	
	//std::cout << "[Tinsel] Added variable to Data at address : " << Address << "Called " << Var.Name << std::endl;
}

//Defines a variable in BSS segment
void Tinsel::DefineGlobalVariableInBss(std::string Scope, Tinsel::Variable Var)
{
	long Address;
	switch (Var.Type)
	{
	case U8:
	case S8:
		Address = CodeGenerator->AddBssByte();
		break;
	case U16:
	case S16:
		Address = CodeGenerator->AddBssDByte();
		break;
	case U32:
	case S32:
		Address = CodeGenerator->AddBssQByte();
		break;
	case INT:
	case STRING:
		Address = CodeGenerator->AddBssInt();
		break;
	default:
		std::cerr << "[Tinsel] variable type to define globally" << std::endl;
		ErrorCode = 4;
		return;
	}
	GlobalReferences.push_back(std::make_tuple(Scope, Var, Address));
	//std::cout << "[Tinsel] Added variable to BSS at address : " << Address << std::endl;
}

//Main code to create global variables and functions
void Tinsel::StartVariableGlobal(std::ifstream& InputFile, Tinsel::Token OriginalToken)
{
	//std::cout << "[Tinsel] StartVariableGlobal " << std::endl;

	//Get the variable type from the original token
	Tinsel::Variable Var;
	Var.Type = GetVariableTypeFromToken(OriginalToken);

	if (InputFile.good())
	{
		//Get the next token as the variables name
		Tinsel::Token Token;
		Token.ReadFromFile(InputFile);

		//Ensure that the variables name is not reserved
		if (Token.Type != UNKNOWN)
		{
			ErrorCode = 2;
			std::cerr << "[Tinsel] Invalid name for variable: " << Token.Name << std::endl;
			return;
		}

		Var.Name = Token.Name;
		
		//std::cout << "[Tinsel] Creating variable \"" << Var.Name << "\"" << std::endl;
		
		//Now we could have one of 4 potential tokens.
		// [ will begin an array type
		// = will begin a gloabal variable assignment
		// ( will begin a function type
		// ; will finish the variable definition
		if (InputFile.good())
		{
			//Read the next token
			Token.ReadFromFile(InputFile);

			if (Token.Type == SEPERATOR)
			{
				// If there is not a default value, store the variable in BSS, giving a default value of 0
				DefineGlobalVariableInBss("Scope", Var);
			}
			else if (Token.Type == ASSIGN)
			{
				//Two cases here, Variable is string, or an integer type.
				//If variable is a string, expect a string, if an integer, expect an integer.
				Tinsel::Token Literal;
				Literal.ReadFromFile(InputFile);
				if (Var.Type == STRING)
				{
					if (Literal.Type != STRING_LITERAL)
					{
						std::cerr << "[Tinsel] Expected a string literal when assigning the global string variable \"" << Var.Name << "\"" << std::endl;
						std::cerr << Literal.Type << " " << Literal.Name << std::endl;
						ErrorCode = 5;
						return;
					}
					else{
						//[FIX-FOR-REAL] Change the "Scope" to the filename
						DefineGlobalVariableInData("Scope", Var, 0, Literal.Name);
						Tinsel::Token Semi;
						Semi.ReadFromFile(InputFile);
						if (Semi.Type != SEPERATOR)
						{
							std::cerr << "[Tinsel] Expected semicolon, while creating initialising global variable" << std::endl;
							ErrorCode = 6;
							return;
						}
					}
				}
				else
				{
					if (Literal.Type != NUMBER_AS_INT)
					{
						std::cerr << "[Tinsel] Expected a number literal when assigning the global variable \"" << Var.Name << "\"" << std::endl;
						std::cerr << Literal.Type << " " << Literal.Name << std::endl;
						ErrorCode = 6;
						return;
					}
					else
					{
						//[FIX-FOR-REAL] Change the "Scope" to the filename
						DefineGlobalVariableInData("Scope", Var, Literal.Value, "");
						Tinsel::Token Semi;
						Semi.ReadFromFile(InputFile);
						if (Semi.Type != SEPERATOR)
						{
							std::cerr << "[Tinsel] Expected semicolon, while creating initialising global variable" << std::endl;
							ErrorCode = 6;
							return;
						}
					}
				}
			}
			else if (Token.Type = OBRACKET)
			{
				//If we get a function definition
				Token.ReadFromFile(InputFile);

				//Read through the function parameters, ignoring them (for now) [FIX-FOR-REAL]
				while (Token.Type != CBRACKET )
				{
					std::cout << "[Tinsel] function param word : " << Token.Type << std::endl;
					Token.ReadFromFile(InputFile);
					if (!InputFile.good())
					{
						std::cerr << "[Tinsel] InputFile was not good while parsing function params." << std::endl;
						ErrorCode = 8;
						return;
					}
				}

				//std::cout << "[Tinsel] End of function params, should now read code block!" << std::endl;
				
				//Parse a code block
				CodeBlock(InputFile, true, Var);
				return;
			}
		}
		else
		{
			ErrorCode = 3;
			std::cerr << "[Tinsel] InputFile was not good while attempting to finalise variable" << std::endl;
			return;
		}
	}
	else{
		ErrorCode = 1;
		std::cerr << "[Tinsel] InputFile was not good while attempting to find variable name." << std::endl;
		return;
	}
}

//Attempt to find a global reference by name 
std::vector <Tinsel::GlobalReference>::iterator Tinsel::FindGlobalReferenceByName(std::string Name)
{
	//Loop through all global references with an iterator
	std::vector <Tinsel::GlobalReference>::iterator it;
	for (it = GlobalReferences.begin(); it < GlobalReferences.end(); ++it)
	{
		Tinsel::GlobalReference A = *it;

		std::string VarName = std::get<1>(A).Name;
		
		//If the name of the variable matches what is required, return the iterator.
		if (VarName.compare(Name) == 0)
			return it;
	}
	
	//Otherwise notify the caller through ErrorCode, (Causing an error if the caller is expecting this to work 100% of the time)
	ErrorCode = -1;

	return it;
}
