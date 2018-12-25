#pragma once
#include "stack.h"
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
#include <functional>

#define ADDR unsigned int 
#define BYTE unsigned char
#define SIZE unsigned int
#define READ_LAST_TYPED_KEY				   mem->Read(0x10FF)
#define WRITE_LAST_TYPED_KEY(x)            mem->Write(0x10FF, x)

#define READ_KEYBOARD_DATA_SIZE            mem->Read(0x10FE)
#define WRITE_KEYBOARD_DATA_SIZE(x)        mem->Write(0x10FE, x)


#define READ_CMD_PROC_MODE                 mem->Read(0x1500)
#define WRITE_CMD_PROC_MODE(x)             mem->Write(0x1500, x)

#define CONSOLE_CURSOR_X_MAX               80
#define CONSOLE_CURSOR_Y_MAX               51

#define READ_STACK_SIZE                    mem->Read(0x14FF)
#define WRITE_STACK_SIZE(x)                mem->Write(0x14FF, x)

#define READ_CONSOLE_CURSOR_X              mem->Read(0x1503)
#define WRITE_CONSOLE_CURSOR_X(x)          mem->Write(0x1503, x)

#define READ_CONSOLE_CURSOR_Y              mem->Read(0x1504)
#define WRITE_CONSOLE_CURSOR_Y(x)          mem->Write(0x1504, x)

class VirtualMachine {
public:
	enum exceptions {EXEC_ERROR, STACK_UNDERFLOW};
/* params */
private:
	/* 0x0000-0x1000       = matrix              */
	/* 0x1000-0x10FF       = keyboardData        */
	// 0x10FF              = N_PROCESS_KEY
	// 0x10FE              = keyboardDataSize
	// 0x1100-0x12FF       = Reserved
	/* 0x1300-0x14FB       = N Stack            */
	// 0x14FF              = N Stack size
	/* 0x1500-0x15FF       = HW Variables       */

	// 0x1500              = CMD/PROCESSING mode(0/1)					
	// 0x1501-0x1502       = INTERPRET_ADDRESS						
	// 0x1503-0x1504       = CONSOLE_CURSOR_INFO (0x1502 = x, 0x1503 = y)
	// 0x1504-0x1505       = LAST_WORD_ADDRESS
	/* WORD FORM 
	   0x0000-0x0001       = PREVIOUS WORD ADDRESS
	   0x0002-0x00XX       = word name
	   0x00XX              = Start Of Word, SpecSymbol, = 0x02
	   0x00XX+1            = WORD INFO, 0 = asm word, 1 - word, 2 = var, 3 = const var
	   0x00XX+2-0xZZZZ     = body of the word
	   0xZZZZ              = FF, End Of Word.
	*/
	/*	0x1500-0xFFFF = Program Data */
	Memory                    *mem;
	unsigned int              myID;
	unsigned char             **matrix;
	boost::thread             *myThread;
	bool                      threadClosed = 1;
	std::wstring              disketSaveFolder = L"";
/* functions */
private:
	void                      PrivateUpdate     ();                                 //iD
	void                      UpdateThread      ();                                 //RtW

	//MatrixFuncs
	void                      TypeWord          (std::string Word);                 //RtW
	void                      NextLine          ();                                 //RtW
	void                      Scroll            ();                                 //RtW
	void                      Page              ();                                 //RtW
	void                      Backspace         ();                                 //RtW

	//DisketFuncs
	void                      SaveToDisket      ();                                //RtW
	bool                      LoadFromDisket    ();                                //RtW
	void                      LoadSTDDisket	    ();                                //RtW
	
	//Read/write funcs
	BYTE                      Readu8            (ADDR) const;                      //RtW
	unsigned short int        Readu16           (ADDR) const;                      //RtW
	unsigned int              Readu32           (ADDR) const;                      //RtW
	unsigned long long int    Readu64           (ADDR) const;                      //RtW
	void                      Writeu8           (ADDR, BYTE);                      //RtW
	void                      Writeu16          (ADDR, unsigned short int);		   //RtW
	void                      Writeu32          (ADDR, unsigned int);			   //RtW
	void                      Writeu64          (ADDR, unsigned long long int);	   //RtW

	//StackFuncs
	void                      WriteToStack      (unsigned int);                     //iD
	unsigned int              ReadFromStack     ();                                 //iD

	unsigned int              StringToInt       (std::string _Val);                 //RtW
	bool                      isSymbol          (BYTE _Value);                      //RtW

	//Word interaction funcs
	std::string               ReadFuncName      (ADDR addr);                        //RtW
	void                      Execute           (ADDR addr);                        //iD


	//Instructions
	//0x00 BRK - break
	//
	//
	//
	//...
	void                      ReadLatestKey     ();
	void                      WriteKey          ();
	void                      WriteWord();
	//0xFF - End of word(return)
	//Assembly instructions
	std::function<BYTE(ADDR addr)> **funcs;
public:
	void                      OpenConsole		();                                 //RtW
	void                      CloseConsole      ();                                 //RtW
	void                      Update            ();                                 //RtW
	unsigned char**&          GetMatrix         ();                                 //RtW
	unsigned char***          GetMatrixPtr      ();                                 //RtW
	void                      ReceiveKey        (const BYTE& key);                  //iD
/* structors */
public:
	VirtualMachine(unsigned int ID);
	~VirtualMachine();
};
#include "core.cpp"
#undef SIZE
#undef BYTE
#undef ADDR