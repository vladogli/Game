#pragma once
#include "stack.h"
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <functional>
#define ADDR unsigned int 
#define BYTE unsigned char
#define SIZE unsigned int
class VirtualMachine {
/* params */
private:
	/* 0x0000-0x1000 = matrix		*/
	/* 0x1000-0x1100 = keyboardData */
	// 0x10FF = N_PROCESS_KEY
	// 0x10FE = keyboardDataSize
	/* 0x1200-0x1300 = R Stack		*/
	/* 0x1400-0x1500 = N Stack		*/
	/* 0x1500-0x1600 = HW Variables */

	// 0x1500 = COMMAND_PROMPT/INTERPRET_STATE(0/1)					
	// 0x1501-0x1502 = INTERPRET_ADDRESS						
	// 0x1502-0x1504 = CONSOLE_CURSOR_INFO (0x1502 = x, 0x1503 = y)
	/* */
	/* 0x1500-0xFFFF = Program Data */
	Memory *mem;
	unsigned int myID;
	unsigned char **matrix;
	boost::thread *myThread;
	bool threadClosed = 1;
	std::wstring disketSaveFolder = L"";
/* functions */
private:
	void				PrivateUpdate	();
	void				UpdateThread	();

	//MatrixFuncs
	void				TypeLetter		(BYTE);
	void				Scroll			();
	void				Page			();

	void				SaveToDisket	();
	bool				LoadFromDisket	();
	void				LoadSTDDisket	();
	long long int		ReadNumber		(ADDR, BYTE);
public:
	void				OpenConsole		();
	void				CloseConsole	();
	void				Update			();
	unsigned char**&				GetMatrix		();
	unsigned char***				GetMatrixPtr	();
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