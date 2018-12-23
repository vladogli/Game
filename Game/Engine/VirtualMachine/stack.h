#pragma once
#include <string>
#include <vector>
#define ADDR unsigned int 
#define BYTE unsigned char
#define SIZE unsigned int
class Memory {
/* structures */
public:
	enum Exception {
		STACK_OVERFLOW,
		WRONG_ADDRESS,
		TOO_BIG_SIZE
	};
private:
/* variables */
	BYTE *memory;
	SIZE memSize;
/* functions */
public:
	void			Write			(const ADDR, BYTE*&, const SIZE);
	void			Fill			(const ADDR, const BYTE, const SIZE);
	void			Clear			(const ADDR, const SIZE);
	void			Read			(const ADDR, BYTE*&, const SIZE)	const;
	const	SIZE&	MaxMemorySize	()									const;
/* structors */
public:
	Memory(SIZE);
	~Memory();
};
#include "stack.cpp"
#undef SIZE
#undef BYTE
#undef ADDR