#include <iostream>
#include <fstream>

//This VM is probably a the worst code imagined

static unsigned char version = 0x01;
int TinselVMRun(unsigned char* Memory, long EntryPoint);

int main(int argc, char* argv[])
{
	//Print version
	std::cout << "Tinsel VM version " << (unsigned int)version << std::endl;

	//Print usage if required
	if (argc != 2)
	{
		std::cout << "Usage: " << argv[0] << " <tnl-file>" << std::endl;
		return -1;
	} 

	//Attempt to open file
	std::ifstream is(argv[1]);

	//Check MAGIC
	if (is.get() != 'T')
	{
		std::cerr << "[ERROR] MAGIC mismatch";
		return -1;
	}

	//Check VERSION
	if (is.get() != version)
	{
		std::cerr << "[ERROR] Version mismatch";
		return -1;
	}

	//Read header
	long CodeSegmentSize = 0;
	long DataSegmentSize = 0;
	long EntryPoint = 0;

	CodeSegmentSize |= is.get();
	CodeSegmentSize |= is.get() << 8;
	DataSegmentSize |= is.get();
	DataSegmentSize |= is.get() << 8;
	EntryPoint |= is.get();
	EntryPoint |= is.get() << 8;

	//Print information
	if (is.good())
	{
		std::cout << "Tinsel VM finished reading header, all good" << std::endl;
		std::cout << "CodeSegmentSize: " << CodeSegmentSize << std::endl;
		std::cout << "DataSegmentSize: " << DataSegmentSize << std::endl;
		std::cout << "EntryPoint: " << EntryPoint << std::endl;
	}
	else
	{
		std::cerr << "Tinsel VM finished reading header, input file is not good." << std::endl;
		is.close();
		return -1;
	}

	//Load memory from file
	unsigned char* Memory = new unsigned char[CodeSegmentSize + DataSegmentSize + 2];
	for (int i = 0; i <= CodeSegmentSize; ++i)
	{
		if (is.good())
			Memory[i] = is.get();
		else
		{
			std::cerr << "IO error while reading in code segment" << std::endl;
			delete Memory;
			is.close();
			return -1;
		}
	}
	for (int i = 0; i <= DataSegmentSize; ++i)
	{
		if (is.good())
		{
			Memory[i + CodeSegmentSize + 1] = is.get();
		}
		else
		{
			std::cerr << "IO error while reading in data segment" << std::endl;
			delete Memory;
			is.close();
			return -1;
		}
	}

	//Now run
	std::cout << "Read the file" << std::endl;
	std::cout << "Running the Code!" << std::endl << std::endl;

	int Ret = TinselVMRun(Memory, EntryPoint);
	
	//Clean up memory
	delete Memory;
	//Close file
	is.close();

	//Return return value from the program
	return Ret;
} 