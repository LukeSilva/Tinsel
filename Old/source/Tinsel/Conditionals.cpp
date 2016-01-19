#include "Tinsel.h"
#include "../CodeGenerator/DefaultCodeGenerator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <vector>

void Tinsel::DoIf(std::ifstream& InputFile, Tinsel::Variable Function)
{
	//If token should be eaten, expect an opening bracket
	Expect(InputFile, OBRACKET);

	//Now do the expression, ensuring it ends with a closing bracket
	Tinsel::Token t = ParseExpression(InputFile);
	if (ErrorCode) return;
	if (t.Type != CBRACKET)
	{
		ErrorCode = -1;
		std::cerr << "Expected closing bracket on if statement!" << std::endl;
		return;
	}

	//Create a jump instruction to skip over the codeblock, to be filled in later.
	long JumpSource = CodeGenerator->CreateFutureJump(true);

	//Parse the code block
	CodeBlock(InputFile, false, Function);

	//Now fill in the previous jump function
	CodeGenerator->FillFutureJump(JumpSource, CodeGenerator->GetFunctionAddress());

}