#pragma once
#include "tnl2c.h" 

#include "IO.h"

class Console : public IOStream
{
public:
	/// Attempts to open a console in the requested mode
	//@ return true on success
	bool open()
	{
		//Attempt to open the file, return false on error
		return true;
	}

	/// Attempts to flush and close the file
	//@ return true on success
	bool close()
	{
		return true;
	}

	/// Attempts to get the next character in the file
	//@ return negative value on error
	//@ return character value on success
	s32 getchar()
	{
		return getc(stdin);
	}

	/// Puts a character into the file
	//@ return true on success
	bool putchar(u32 character)
	{
		printf("%c",character);
		return true;
	}
};
