#include "Tinsel.h"
#include "../CodeGenerator/DefaultCodeGenerator.h"
#include "../CodeGenerator/VMCodeGenerator.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <iterator>
#include <vector>


Tinsel::Tinsel()
{
	//Sets the error code to be 0 by default
	ErrorCode = 0;

	//Loads the applicable code generator.
	CodeGenerator = new VMCodeGenerator();
}

//Loads, parses and compiles a file
int Tinsel::LoadFile(std::string FileName)
{
	//Open the file for input.
	std::ifstream InputFile(FileName.c_str(),std::ifstream::in);

	//While there is still code in the file, and there has not been an error, continue to parse
	while (InputFile.good() && ErrorCode == 0 )
	{
		//Read a token from the file.
		Tinsel::Token t;
		t.ReadFromFile(InputFile);

		//std::cout << "[Tinsel-Load-File] Word: \"" << t.Name << "\" type = " << t.Type << std::endl;
		
		//If the token is a variable file, create a global variable
		if (IsVariableType(t))
		{
			StartVariableGlobal(InputFile, t);
		}
	}
	return 0;
}

bool Tinsel::Compile(std::string filename)
{
	//Check that the code generator supports compilation.
	if (CodeGenerator->CanCompileIntoFile())
	{
		//If so, compile the code into a file.
		CodeGenerator->CompileIntoFile(filename);
		return false;
	}
	else
	{
		//If not, error.
		std::cout << "Cannot compile into a file" << std::endl;
		return true;
	}
}
Tinsel::~Tinsel()
{
	//Delete the code generator when the tinsel object dies.
	delete CodeGenerator;
}
