#pragma once
#include "stack.h"
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
#include <functional>
#define ADDR unsigned int 
#define BYTE unsigned char
#define SIZE unsigned int
#define DISKET_SAVE_FOLDER L""
class VirtualMachine {
/* params */
private:
	/* 0x0000-0x1000 = matrix		*/
	/* 0x1000-0x1100 = keyboardData */
	/* 0x1200-0x1300 = R Stack		*/
	/* 0x1400-0x1500 = N Stack		*/
	/* 0x1500-0xFFFF = Program Data */
	Memory *mem;
	unsigned int myID;
	unsigned char **matrix;
	boost::thread *myThread;
	bool threadClosed = 1;
/* functions */
private:
	void				PrivateUpdate	();
	void				UpdateThread	();

	void				SaveToDisket	();
	bool				LoadFromDisket	();
	void				LoadSTDDisket	();
	long long int		ReadNumber		(ADDR, BYTE);
public:
	void				OpenConsole		();
	void				CloseConsole	();
	void				Update			();
	char**&				GetMatrix		();
	void				ReceiveKey		(const BYTE& key);
/* structors */
public:
	VirtualMachine(unsigned int ID);
	~VirtualMachine();
};
#include "core.cpp"
#undef SIZE
#undef BYTE
#undef ADDR