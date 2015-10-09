#include "Tinsel.h"
#include "../CodeGenerator/DefaultCodeGenerator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <vector>

//Defines a local variable
void Tinsel::DefineLocalVariable(std::string Scope, Tinsel::Variable Var, long Value, std::string StringValue)
{
	//std::cout << "[Tinsel] Defining local variable called = \"" << Var.Name << "\"" << std::endl;
	
	long StringAddress = -1;
	long Address = StackOffset;
	if (Var.Type != STRING)
	{
		//Load the default value into the main register, and push onto the stack
		CodeGenerator->LoadLiteralIntoMainRegister(Var.Type, Value);
		PushMainRegister(Var.Type);
	}
	else
	{
		//Do not allow string types
		std::cerr << "[Tinsel] Can not create local strings at the moment!" << std::endl;
		ErrorCode = 3;
		return;
	}

	//Store the variable in the vector of local references
	LocalReferences.push_back(std::make_tuple(Var,Address));
	
	//std::cout << "[Tinsel] Added variable onto stack at offset : " << Address << std::endl;
}

//Main Code to parse a local variable segment
void Tinsel::StartVariableLocal(std::ifstream& InputFile, Tinsel::Token OriginalToken)
{
	//std::cout << "[Tinsel] StartVariableLocal " << std::endl;

	//Get the variable type
	Tinsel::Variable Var;
	Var.Type = GetVariableTypeFromToken(OriginalToken);
	if (InputFile.good())
	{
		Tinsel::Token Token;
		Token.ReadFromFile(InputFile);
		if (Token.Type != UNKNOWN)
		{
			ErrorCode = 2;
			std::cerr << "[Tinsel] Invalid name for variable: " << Token.Name << std::endl;
			return;
		}
		Var.Name = Token.Name;
		//std::cout << "[Tinsel] Creating variable \"" << Var.Name << "\"" << std::endl;
		//Now we could have one of 2 potential tokens.
		// = will begin a gloabal variable assignment
		// ; will set the local variable to be a default value of 0.
		if (InputFile.good())
		{
			Token.ReadFromFile(InputFile);
			if (Token.Type == SEPERATOR)
			{

				//[FIX-FOR-REAL] Change scope, potentially remove because local variables don't have a scope.
				DefineLocalVariable("Scope", Var, 0, "");
			}
			else if (Token.Type == ASSIGN)
			{
				//Get the literal value
				Tinsel::Token Literal;
				Literal.ReadFromFile(InputFile);

				//Error if the literal is not valid
				if (Literal.Type != NUMBER_AS_INT)
				{
					std::cerr << "[Tinsel] Expected a number literal when assigning the local variable \"" << Var.Name << "\"" << std::endl;
					std::cerr << Literal.Type << " " << Literal.Name << std::endl;
					ErrorCode = 6;
					return;
				}
				else
				{
					//[FIX-FOR-REAL] Change the "Scope" to the filename
					DefineLocalVariable("Scope", Var, Literal.Value, "");
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


//Attempt to find a local reference by name
std::vector <Tinsel::LocalReference>::iterator Tinsel::FindLocalReferenceByName(std::string Name)
{

	//Loop through all the local references
	std::vector <Tinsel::LocalReference>::iterator it;
	for (it = LocalReferences.begin(); it < LocalReferences.end(); ++it)
	{

		Tinsel::LocalReference A = *it;
		std::string VarName = std::get<0>(A).Name;
		
		//Returning the iterator if the name matches
		if (VarName.compare(Name) == 0)
			return it;
	}
	ErrorCode = -1;
	return it;
}


//Remove any invalid local variables
void Tinsel::RemoveInvalidLocals(int StackOffseta)
{
	std::vector<Tinsel::LocalReference>::iterator it;
	for (it = LocalReferences.begin(); it < LocalReferences.end(); ++it)
	{
		int VarStackOffset = std::get<1>(*it);
		if (StackOffseta - VarStackOffset <= 0)
		{
			LocalReferences.erase(it);

			//Because nothing follows standards -.- (In many compilers i've tried std::vector::erase does not return a valid iterator, causing crashes)
			RemoveInvalidLocals(StackOffseta);
			return;
		}
	}
}