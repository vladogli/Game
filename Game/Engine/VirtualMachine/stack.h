#pragma once
#include <string>
#include <vector>
#define ADDR unsigned int 
#define BYTE unsigned char
#define SIZE unsigned int
class Stack {
private:
/* variables */
	BYTE *memory;
	SIZE memSize;
	std::vector<ADDR> allocatedPtrs;
/* functions */
private:
	SIZE	ReadSize		(const ADDR) const;
	void	WriteSize		(const ADDR, const SIZE);
	void	ClearMemory		(const ADDR, const SIZE);
public:
	void	ClearMemory		();
	ADDR	AllocateMemory	(const SIZE);
	ADDR	AllocateMemory	(const SIZE, BYTE);
	void	Delete			(const ADDR);
	bool	Edit			(const ADDR, SIZE, BYTE*&, SIZE = 0);
	void	Read			(BYTE*&, const ADDR, SIZE = 0, SIZE = 0) const;
	void	Read			(BYTE*&, const ADDR) const;
	void	Dump(BYTE*& dest, SIZE size)		{
		const BYTE* _Val = (const BYTE*)memory;
		memcpy(dest, _Val, size);
	}
	const SIZE&	 Size()		{ return memSize; }
/* structors */
public:
	Stack(SIZE);
	~Stack();
};
#include "stack.cpp"
#undef SIZE
#undef BYTE
#undef ADDR