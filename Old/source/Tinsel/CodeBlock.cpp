#include "Tinsel.h"
#include "../CodeGenerator/DefaultCodeGenerator.h"	
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <vector>

//The main code which parses and calls the code generator to compile a block of code.
//Returns true on error.
bool Tinsel::CodeBlock(std::ifstream& InputFile, bool IsFunction, Tinsel::Variable Var)
{
	//Get the stating address of the block
	long FunctionAddress = CodeGenerator->GetFunctionAddress();

	//Ensure that a { is present in the file
	if (Expect(InputFile, OBLK)) return true;
	//std::cout << "[TINSEL] Parsing code block" << std::endl;
	bool HasReturned = false;

	//Make note of what the stack offset was
	int OldStackOffset = StackOffset;

	//If this is a new function, clear all of the local variables
	if (IsFunction)
	{
		OldStackOffset = 0;
		StackOffset = 0;
		LocalReferences.clear();
	}
		
	//Main parsing loop
	while (InputFile.good())
	{
		//Read a token from the file
		Tinsel::Token Token;
		Token.ReadFromFile(InputFile);

		//Break from the loop if the token is }
		if (Token.Type == CBLK)
			break;
		else if (Token.Type == IF)
		{
			//If the token is if, call DoIf (in Conditionals.cpp)
			DoIf(InputFile, Var);
		}
		else if (IsVariableType(Token))
		{
			//If the token is a variable type, create and define the variable (Locals.cpp)
			StartVariableLocal(InputFile, Token);
		}
		else if (Token.Type == EXIT)
		{
			//If the token is Exit, ask the CodeGenerator to generate the require code to quit.
			CodeGenerator->ExitProgram();
		}
		else if (Token.Type == PRINTSTR)
		{
			//If the token is print string [FIX-FOR-REAL] Allow print string to take string literals

			//Create a new token to hold the name of the string variable
			Tinsel::Token t;
			t.ReadFromFile(InputFile);

			//Ensure the token is a variable name
			if (ErrorCode) return true;
			if (t.Type != UNKNOWN)
			{
				std::cerr << "Expected variable name for printstring " << std::endl;
				ErrorCode = -1;
				return true;
			}

			//Attempt to find a global variable. (Can't create a local string variable yet, no point searching that)
			std::vector<GlobalReference>::iterator it;
			it = FindGlobalReferenceByName(t.Name);
			if (ErrorCode) return true;

			//If the variable found is not a string, error.
			if (std::get<1>(*it).Type != STRING)
			{
				std::cerr << "Printstring requires a global string variable " << std::endl;
				ErrorCode = -1;
				return true;
			}

			//If we found a variable, load the address of the string into the main register
			CodeGenerator->LoadVariableIntoMainRegister(STRING, std::get<2>(*it), STRING);

			//And ask the code generator to print it to the screen.
			CodeGenerator->PrintCharArrayInMain();

		}
		else if (Token.Type == PRINTINT)
		{
			//If we need to print and integer,
			//Parse an expression, which will leave the result in the main register.
			ExpressionType = Tinsel::VariableType::INT;
			ParseExpression(InputFile);

			//Ask the code generator to print the integer in the main register to the screen.
			CodeGenerator->PrintIntInMain();

		}
		else if (Token.Type == UNKNOWN)
		{
			//If we get an unknown token
			if (ErrorCode) return true;

			//Attempt to find a local and global reference by the token's name
			ErrorCode = 0;
			std::vector<GlobalReference>::iterator it;
			std::vector<LocalReference>::iterator itl;
			bool local = true;
			itl = FindLocalReferenceByName(Token.Name);
			if (ErrorCode == -1)
			{
				local = false;
				ErrorCode = 0;
				it = FindGlobalReferenceByName(Token.Name);
			}

			//Error if we are unable to find a valid variable
			if (ErrorCode == -1)
			{
				std::cerr << "[Tinsel] Unknown token as variable \"" << Token.Name << "\"" << std::endl;
				return true;
			}

			//We have found a variable that actually exists, now we can parse an experssion
			//The next token in this case would be the ASSIGN token.
			if (Expect(InputFile, ASSIGN))
			{
				std::cerr << "[Tinsel] Expected ASSIGN after variable name! " << std::endl;
				ErrorCode = 12;
				return true;
			}
			
			//Now we parse an expression, leaving the result in the Main Register
			//First get the type of the expression, which is needed by ParseExpression();
			if (!local)
				ExpressionType = std::get<1>(*it).Type;
			else
				ExpressionType = std::get<0>(*itl).Type;
			
			//Parse the expression
			Tinsel::Token t = ParseExpression(InputFile);

			//Expect a ;
			if (t.Type != SEPERATOR)
			{
				std::cerr << "[Tinsel] Expected SEPERATOR after expression! " << std::endl;
				ErrorCode = 13;
				return true;
			}

			//Store the result of the expression into the variable found above.
			if (!local)
				CodeGenerator->StoreMainRegisterIntoVariable(std::get<1>(*it).Type, std::get<2>(*it), std::get<1>(*it).Type);
			else
				CodeGenerator->StoreMainRegisterRelativeStack(std::get<0>(*itl).Type, StackOffset - std::get<1>(*itl), std::get<0>(*itl).Type);

		}
		else if (Token.Type == CALL)
		{
			//If the token is a call token
			//Read the name of the requested function
			Tinsel::Token FName;
			FName.ReadFromFile(InputFile);

			//Attempt to find a variable by the functions name
			std::vector<GlobalReference>::iterator it;
			it = FindGlobalReferenceByName(FName.Name);

			//If we did not find a variable by the functions name, add it with a blank address for now
			if (ErrorCode == -1)
			{
				//[FIX-FOR-REAL] Change Scope -.-
				Tinsel::Variable Function;
				Function.IsArray = false;
				Function.Type = INT;
				Function.Name = FName.Name;

				//Define the function address
				DefineGlobalVariableInData("Scope", Function, 0x05, "");
				
				ErrorCode = 0;

				//Ensure that the iterator is valid, and find the variable that we just added.
				it = FindGlobalReferenceByName(FName.Name);
				if (ErrorCode) return true;
			}

			//Load the address of the function into the main register
			CodeGenerator->LoadVariableIntoMainRegister(std::get<1>(*it).Type, std::get<2>(*it), ExpressionType);

			//Call the function
			CodeGenerator->CallMainRegister();
		}
		else if (Token.Type == RETURN)
		{
			//If the token is return, make note of the fact that the current function has returned
			HasReturned = true;

			//Read the return argument
			Tinsel::Token Argument;
			Argument.ReadFromFile(InputFile);

			//Two cases here, we return a literal, or a variable (local / global)
			if (Var.Type != STRING)
			{
				if (Argument.Type == UNKNOWN)
				{
					//Look up variable, and load it into the main register, and emit code to return.
					bool local = true;
					ErrorCode = 0;
					std::vector<LocalReference>::iterator itl;
					std::vector<GlobalReference>::iterator it;
					itl = FindLocalReferenceByName(Argument.Name);
					if (ErrorCode == -1)
					{
						local = false;
						ErrorCode = 0;
						it = FindGlobalReferenceByName(Argument.Name);
					}

					//If we could not find a variable by the name requested, error
					if (ErrorCode == -1)
					{
						std::cerr << "[Tinsel] Could not find variable \"" << Argument.Name << "\"" << std::endl;
						return true;
					}
					
					//Load the variable in the main register
					if (!local)
						CodeGenerator->LoadVariableIntoMainRegister(std::get<1>(*it).Type, std::get<2>(*it), std::get<1>(*it).Type);
					else
						CodeGenerator->LoadVariableRelativeStackIntoMainRegister(std::get<0>(*itl).Type, StackOffset - std::get<1>(*itl), std::get<0>(*itl).Type);

					//Reset the stack, deleting any local variables
					ZeroStackOffset(0);

					//Return from the function
					CodeGenerator->EmitReturn();
				}
				else if (Argument.Type == NUMBER_AS_INT)
				{
					//Load the literal into the main register
					CodeGenerator->LoadLiteralIntoMainRegister(Var.Type, Argument.Value);

					//Reset the stack, deleting any local variables
					ZeroStackOffset(0);
					//Return from the function
					CodeGenerator->EmitReturn();
				}
			}
			else{
				//Error if user wants to return a string.
				std::cerr << "[Tinsel] Returning strings is not supported at the moment." << std::endl;
				return true;
			}
		}
	}

	//If the function finishes without a return statement
	if (!HasReturned)
	{
		//Reset the stack deleting any local variables defined in this code block
		ZeroStackOffset(OldStackOffset);

		//Emit a return, only if we are in a function.
		if (IsFunction)
			CodeGenerator->EmitReturn();
	}
	
	//Remove any locals that were created in this codeblock
	RemoveInvalidLocals(OldStackOffset);

	if (ErrorCode) return true;

	if (IsFunction)
	{
		//First check that the function isn't already declared
		std::vector<GlobalReference>::iterator it = FindGlobalReferenceByName(Var.Name);

		//If the function was already declared, reset it's address to the address for this function
		if (ErrorCode != -1)
		{
			// [FIX-FOR-REAL] Change Scope
			ResetGlobalVariableAddress("Scope", std::get<2>(*it), FunctionAddress);
		}
		else //Otherwise, add it's address
		{
			// [FIX-FOR-REAL] Change Scope	
			DefineGlobalVariableInData("Scope", Var, FunctionAddress, "");
			ErrorCode = 0;
		}
		StackOffset = 0;
	}
	else
		//Reset the stack offset to its former value
		StackOffset = OldStackOffset;
	
	return false;
}