#include "Tinsel.h"
#include "../CodeGenerator/DefaultCodeGenerator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <vector>

//This approach to parsing expressions was taken from Let's Build a Compiler by Jack W. Crenshaw, Ph.D.

//Level 1: < > 
//Level 2: + - | ^ ||
//Level 3: * / & && 
//Level 4: ~ ! (<expression>) <variable> <integer>


//This function parses level 4 operands, to be renamed
bool Tinsel::ParseLevel4(std::ifstream& InputFile)
{

	//First read a token
	Tinsel::Token t;
	t.ReadFromFile(InputFile);

	//If the token is an integer literal
	if (t.Type == NUMBER_AS_INT)
	{
		//Load it into the main register
		CodeGenerator->LoadLiteralIntoMainRegister(ExpressionType, t.Value);
		return false;
	}
	else if (t.Type == GETINT)
	{
		//If we need to get an int from stdin, request the code generator to produce that code.
		CodeGenerator->GetIntIntoMain();
		return false;
	}
	else if (t.Type == CALL)
	{
		//If we need to call get a token of the function name
		Tinsel::Token FName;
		FName.ReadFromFile(InputFile);

		//Attempt to find a global variable with that name
		std::vector<GlobalReference>::iterator it;
		it = FindGlobalReferenceByName(FName.Name);

		//If we couldn't find a variable with the name of the function, create a variable holding the functions address
		if (ErrorCode == -1)
		{
			//[FIX-FOR-REAL] Change Scope -.-
			Tinsel::Variable Function;
			Function.IsArray = false;
			Function.Type = INT;
			Function.Name = FName.Name;

			//Store a blank address for now.
			DefineGlobalVariableInData("Scope", Function, 0x05,"");
			ErrorCode = 0;
			
			//Ensure the iterator is valid, and we can find the variable be created
			it = FindGlobalReferenceByName(FName.Name);
			if (ErrorCode) return true;
		}

		//Load the functions address into the main register
		CodeGenerator->LoadVariableIntoMainRegister(std::get<1>(*it).Type, std::get<2>(*it), ExpressionType);

		//Call the function, which will return its result in the main register
		CodeGenerator->CallMainRegister();
		return false;
	}
	else if (t.Type == NOT)
	{
		//If we need to invert (bitwise) a regiser, get the next number
		ParseLevel4(InputFile);

		//And request the code generator to invert it. 
		//[OPTIMISATIONS] If we want to get a literal, we could load the inverse of the literal straight away
		CodeGenerator->NotMainRegister(ExpressionType);
		return false;
	}
	else if (t.Type == BOOLNOT)
	{
		//If we need to invert (logical) a regiser, get the next number
		ParseLevel4(InputFile);

		//And request the code generator to invert it
		CodeGenerator->LogicalNotMain(ExpressionType);
		return false;
	}
	else if (t.Type == OBRACKET)
	{
		//If we get an open parenthesis, parse the next expression
		Tinsel::Token tc = ParseExpression(InputFile);
		
		//And ensure that the expression ends with a close parenthesis
		if (tc.Type != CBRACKET)
		{
			std::cerr << "[Tinsel] Unknown token to close expression \"" << t.Name << "\"" << std::endl;
			ErrorCode = 19;
			return true;
		}

	}
	else if (t.Type == UNKNOWN)
	{
		//If we have a variable name, look it up
		std::vector<LocalReference>::iterator itl;
		std::vector<GlobalReference>::iterator it;
		ErrorCode = 0;
		bool local = true;
		itl = FindLocalReferenceByName(t.Name);
		if (ErrorCode == -1)
		{
			local = false;
			ErrorCode = 0;
			it = FindGlobalReferenceByName(t.Name);
		}

		//If we couldn't find the variable, error
		if (ErrorCode == -1)
		{
			std::cerr << "[Tinsel] Unknown token as variable \"" << t.Name << "\"" << std::endl;
			return true;
		}
		
		//Load the variable into the main register
		if (!local)
			CodeGenerator->LoadVariableIntoMainRegister(std::get<1>(*it).Type, std::get<2>(*it), ExpressionType);
		else
			CodeGenerator->LoadVariableRelativeStackIntoMainRegister(std::get<0>(*itl).Type, StackOffset - std::get<1>(*itl), ExpressionType);
		return false;
	}
	return false;
}


//This function implements Level 3 of the expression parser, to be renamed
Tinsel::Token Tinsel::ParseLevel3(std::ifstream& InputFile)
{
	Tinsel::Token unkt;
	unkt.Type = UNKNOWN;

	//Gets the next number in the main register, quitting on error.
	if (ParseLevel4(InputFile))
	{
		return unkt;
	}

	//Read the next token from file
	Tinsel::Token t;
	t.ReadFromFile(InputFile);

	//While we still have a level 3 token
	while (t.Type == MULTIPLICATION || t.Type == DIVISION || t.Type == AND || t.Type == BOOLAND)
	{
		//Push the main register
		PushMainRegister(ExpressionType);
		
		//Get the next number in the main register
		if (ParseLevel4(InputFile))
			return unkt;
		
		//Pop the old value into the second register
		PopSecondaryRegister(ExpressionType);

		//Perform required operation
		if (t.Type == MULTIPLICATION)
		{
			CodeGenerator->MultiplyMainAndSecondary(ExpressionType);
		}
		else if (t.Type == DIVISION)
		{
			CodeGenerator->DivideSecondaryByMain(ExpressionType);
		}
		else if (t.Type == AND)
		{
			CodeGenerator->AndMainAndSecondary(ExpressionType);
		}
		else if (t.Type == BOOLAND)
		{
			CodeGenerator->LogicalAndMainAndSecondary(ExpressionType);
		}

		//Read the next token
		t.ReadFromFile(InputFile);
	}
	return t;
}

//This function parses the 2nd level of tinsels expression
Tinsel::Token Tinsel::ParseLevel2(std::ifstream& InputFile)
{
	//Get the first number in the main register, (ParseLevel3 returns the next token)
	Tinsel::Token t;
	t = ParseLevel3(InputFile);

	if (t.Type == UNKNOWN) return t;

	//While we have a level 2 token
	while (t.Type == ADDITION || t.Type == SUBTRACTION || t.Type == OR || t.Type == XOR || t.Type == BOOLOR)
	{
		Tinsel::TokenTypes v = t.Type;
	
		//Push the first number onto the stack
		PushMainRegister(ExpressionType);

		//Get the second number
		t = ParseLevel3(InputFile);
		if (t.Type == UNKNOWN) return t;

		//Pop the first number from the stack
		PopSecondaryRegister(ExpressionType);

		//Perform the required operation
		if (v == ADDITION)
		{
			CodeGenerator->AddMainAndSecondaryRegisters(ExpressionType);
		}
		else if (v == SUBTRACTION)
		{
			CodeGenerator->SubMainFromSecondary(ExpressionType);
		}
		else if (v == OR)
		{
			CodeGenerator->OrMainAndSecondary(ExpressionType);
		}
		else if (v == XOR)
		{
			CodeGenerator->XorMainAndSecondary(ExpressionType);
		}
		else if (v == BOOLOR)
		{
			CodeGenerator->LogicalOrMainAndSecondary(ExpressionType);
		}

	}
	return t;
}

//This function performs the level 1 parsing of tinsels expressions
Tinsel::Token Tinsel::ParseExpression(std::ifstream& InputFile)
{
	Tinsel::Token t;
	
	//Get the first number
	t = ParseLevel2(InputFile);
	if (t.Type == UNKNOWN) return t;

	//While we have a level 1 token
	while (t.Type == LT || t.Type == GT)
	{
		Tinsel::TokenTypes v = t.Type;

		//Push the first number onto the stack
		PushMainRegister(ExpressionType);

		//Get the second number
		t = ParseLevel2(InputFile);
		if (t.Type == UNKNOWN) return t;

		//Pop the first number into the second register
		PopSecondaryRegister(ExpressionType);

		//Perform the required operation
		if (v == LT)
		{
			CodeGenerator->LogicalLessThan(ExpressionType, true);
		}
		else if (v == GT)
		{
			CodeGenerator->LogicalGreaterThan(ExpressionType, true);
		}
	
	}
	return t;
	
}



