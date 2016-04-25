#pragma once
template <class _type> class tinsel_array
{
public:
	int length = 0;
	_type* ptr;

	void reset(int nlength)
	{
		if (length != 0)
			free(ptr);
		if (nlength != 0)
			ptr = (_type*)malloc(nlength * sizeof(_type));
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
			nptr = (_type*)malloc(nlength * sizeof(_type));

		// Copy the old data into the new memory position
		for (int i = 0; i < copylen; ++i)
			nptr[i] = ptr[i];

		// Free the old data
		if (length != 0)
			free(ptr);
		
		// Setup the variables
		ptr = nptr;
		length = nlength;
	}

	_type& operator[](int pos) const
	{
		return ptr[pos];
	}
};
