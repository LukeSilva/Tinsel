#pragma once
#include "tnl2c.h" 

#include "String.h"

//@ table Error Values
//@ data Value | Error
//@ data    -1 | End of Stream / File

class IOStream
{
public:	
	bool _isOpen = false;

	/// Opens the Stream
	//@ return true on success
	virtual bool open()
	{
		_isOpen = true;
		return true;
	}

	/// Closes the Stream
	//@ return true on success
	virtual bool close()
	{
		_isOpen = false;
		return true;
	}

	/// Gets a character from the stream
	//@ return negative value on error 
	//@ return character value on success
	virtual s32 getchar()
	{
		return -1;
	}

	/// Puts a character into the stream
	//@ return true on success
	virtual bool putchar(u32 character)
	{
		return false;
	}

	/// Puts a String into the stream
	//@ return true on success
	bool putstring(String msg)
	{
		bool retval = true;
		int pos = 0;
		while (pos < msg.pos && retval == true)
		{
			retval = putchar(msg.getchar(pos)) && retval;
			pos += 1;
		}
		return retval;
	}

	/// Puts an integer into the stream
	//@ return true on success
	bool putint(s32 integer)
	{
		String output_string(34);

		int sign = integer;
		if (integer < 0)
			integer = -integer;

		while (integer > 0)
		{
			output_string.append('0' + integer%10);
			integer /= 10;
		}
	
		if (sign < 0)
			output_string.append('-');
		else if (sign == 0)
			output_string.append('0');

		output_string.reverse();

		return putstring(output_string);
	}

};
