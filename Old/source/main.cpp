#include <iostream>
#include "Tinsel/Tinsel.h"
int main(int argc, char* argv[])
{
	//Print usage information if required
	if (argc != 3){
		std::cerr << "Invalid command line arguments" << std::endl;
		std::cout << "Usage:" << std::endl;
		std::cout << argv[0] << " <file> <output>" << std::endl;
		return -1;
	}
	
	//Parse the file
	Tinsel p;
	p.LoadFile(argv[1]);


	//Error if required
	if (p.Error())
	{
		std::cout << "Error Occured..." << std::endl;
		return -1;
	}


	//Atempt to compile the file into argv[2]
	p.Compile(argv[2]);

	return 0;
}