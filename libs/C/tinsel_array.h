#pragma once
template <class _type> class tinsel_array
{
public:
	int length = 0;
	_type* ptr;

	tinsel_array()
	{
		length = 0;
		ptr = (_type*) 0xDEADBEEF;
	}
	
	tinsel_array(tinsel_array const & that)
	{
		this->length = that.length;
		this->ptr = new _type[length];
		for (int i = 0; i < this->length; ++i)
			this->ptr[i] = that.ptr[i];
	}

	void reset(int nlength)
	{
		/*if (length > 0)
			delete[] ptr;*/
		if (nlength != 0)
			ptr = new _type[nlength];
		length = nlength;
	}
	
	void resize(int nlength)
	{
		// Create the starting ptr
		_type* nptr = (_type*)0xDEADBEEF;

		// Figure out how many elements need to be copied
		int copylen = length;
		if (nlength < length)
			copylen = nlength;

		// Allocate the new memory position, if needed
		if (nlength != 0)
			nptr = new _type[nlength];

		// Copy the old data into the new memory position
		for (int i = 0; i < copylen; ++i)
			nptr[i] = ptr[i];

		// Free the old data
/*		if (length != 0)
			delete[] ptr;*/
		
		// Setup the variables
		ptr = nptr;
		length = nlength;
	}

	_type& operator[](int pos) const
	{
		return ptr[pos];
	}
};
