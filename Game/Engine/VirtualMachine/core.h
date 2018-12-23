#pragma once
#include "stack.h"
#include <boost/filesystem.hpp>
#define ADDR unsigned int 
#define BYTE unsigned char
#define SIZE unsigned int
#define DISKET_SAVE_FOLDER L""
class VirtualMachine {
/* params */
private:
	Memory *mem;
	ADDR nullPage;
	ADDR rStack;
	ADDR nStack;
	ADDR inPort;
	ADDR outPort;
	unsigned int myID;
/* functions */
private:
	void SaveToDisket	();
	bool LoadFromDisket	();
	void LoadSTDDisket	();
public:

/* structors */
public:
	VirtualMachine(unsigned int ID, SIZE size = 0x10000);
	~VirtualMachine();
};
#include "core.cpp"
#undef SIZE
#undef BYTE
#undef ADDR