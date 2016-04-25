#pragma once
#include "tnl2c.h" 

class String
{
public:
	int pos = 0;
	int size = 0;
	tinsel_array<u32> characters;

	/// Creates a string of size 0
	String()
	{
		characters.reset(0);
		pos = 0;
		size = 0;
	}

	/// Creates a string with a certain size
	String(int nsize)
	{
		size = nsize;
		pos = 0;
		characters.reset(nsize);
	}

	///Creates a string from a string literal ( C++ syntax )
	String(const char* string_literal)
	{
		pos = strlen(string_literal);
		size = strlen(string_literal);
		characters.reset(size);

		for (int i = 0; i < size; ++i)
		{
			characters[i]=string_literal[i];
		}
		
	}

	/// Gets the length of the string
	int length()
	{
		return pos;
	}

	/// Sets the string to ""
	void clear()
	{
		pos = 0;
		characters.reset(size);
	}

	///Allows the string to be copied by the = operator
	String& operator=(const String& that)
	{
		characters.reset(that.size);
		size = that.size;
		pos = that.pos;
		for (int i = 0; i < pos; ++i)
		{
			characters[i]=that.characters[i];
		}
	}	
	
	/// Gets a character from the string
	//@ return the character value
	u32 getchar(int npos)
	{
		return characters[npos];
	}

	/// Sets a character in the string
	//@ return true on success
	bool setchar(int npos, u32 character)
	{
		characters[npos] = character;
		return true;
	}

	/// Appends a character to the string
	//@ return true on success
	bool append(u32 character)
	{
		if (pos < characters.length)
			characters[pos] = character;
		else
			return false;
		++pos;
		return true;		
	}

	/// Appends a string to the string
	//@ return true on success
	bool concat(String str)
	{
		int npos = 0;
		while (npos < str.pos)
		{
			this->append(str.characters[npos]);
			++npos;
		}
	}

	/// Reverses the string
	//@ return true on success (always)
	bool reverse()
	{
		int npos = 0;
		while (npos < (pos)/2)
		{
			u32 ochar = characters[pos-npos-1];
			characters[pos-npos-1] = characters[npos];
			characters[npos] = ochar;
			++npos;
		}
		return true;
	}

	/// Compares the string with another string
	//@ return true if the strings match
	bool compare(String str)
	{
		if (pos != str.pos)
			return false;
		int npos = 0;
		while(npos < pos)
		{
			if (characters[npos] != str.characters[npos])
				return false;
			++npos;
		}
		return true;
	}
};
