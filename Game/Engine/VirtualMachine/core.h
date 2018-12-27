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

#define READ_CONSOLE_CURSOR_X              mem->Read(0x1501)
#define WRITE_CONSOLE_CURSOR_X(x)          mem->Write(0x1501, x)

#define READ_CONSOLE_CURSOR_Y              mem->Read(0x1502)
#define WRITE_CONSOLE_CURSOR_Y(x)          mem->Write(0x1502, x)

#define READ_LAST_WORD_ADDRESS             Readu32(0x1503)
#define WRITE_LAST_WORD_ADDRESS(x)         Writeu32(0x1503, x)

#define FIRST_LOAD_BYTE 0x1503
class VirtualMachine {
public:
	enum exceptions { EXEC_ERROR, STACK_UNDERFLOW, UNKNOWN_TOKEN, UNKNOWN_ERROR};
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

	// 0x1500              = CMD/PROCESSING/COMPILING mode(0/1)				
	// 0x1501-0x1502       = CONSOLE_CURSOR_INFO (0x1501 = x, 0x1502 = y)	
	// 0x1503-0x1506       = LAST_WORD_ADDRESS
	/* WORD FORM
	   0x0000-0x0003       = PREVIOUS WORD ADDRESS
	   0x0004              = STR
	   0x0005-0x0045       = word name, (max 64 symbols)
	   0x0046              = word type, ptr links here 0 - asm, 1 - ptrs to words
	   0x0047-0xXXXX       = word body
	   for asm:
	   0xXXXX+1            = 0x02, RETURN
	   for ptr to words:
	   0xXXXX+1-0xXXXX+5   = 0xFFFFFFFF, RETURN
	   0xFFFFFFFE          = put next 4 bytes to stack
	   0xFFFFFFFD          = str start
	   0xFFFFFFFC          = IF START, next 4 bytes = else, next 4 bytes = then
	   ...
	   0x00                = str break
	*/
	/* VAR FORM
	   0x0000-0x0003       = PREVIOUS WORD ADDRESS
	   0x0004              = STR
	   0x0005-0x0045       = word name, (max 64 symbols)
	   0x0046              = word type, ptr links here. 2 - var, 3 - const var
	   0x0047-0x0048       = SIZE
	   0x004A-0xXXXX       = body of the word
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
	void                      PrivateUpdate();                                 //RtW
	void                      WriteKey();                                      //RtW
	void                      WriteWord();                                     //RtW
	void                      UpdateThread();                                  //RtW

	//MatrixFuncs
	void                      TypeWord(std::string Word);                      //RtW
	void                      NextLine();                                      //RtW
	void                      Scroll();                                        //RtW
	void                      Page();                                          //RtW
	void                      Backspace();                                     //RtW

	//DisketFuncs
	void                      SaveToDisket();                                  //RtW
	bool                      LoadFromDisket();                                //RtW
	void                      LoadSTDDisket();                                 //RtW

	//Read/write funcs
	BYTE                      Readu8(ADDR) const;                              //RtW
	unsigned short int        Readu16(ADDR) const;                             //RtW
	unsigned int              Readu32(ADDR) const;                             //RtW
	void                      Writeu8(ADDR, BYTE);                             //RtW
	void                      Writeu16(ADDR, unsigned short int);		       //RtW
	void                      Writeu32(ADDR, unsigned int);			           //RtW

	//StackFuncs
	void                      WriteToStack(unsigned int);                      //iD
	unsigned int              ReadFromStack();                                 //iD

	unsigned int              StringToInt(std::string);                        //RtW
	bool                      isSymbol(BYTE);                                  //RtW
    //Compile funcs
	void                      Compile(BYTE);
	bool                      CompileStr(ADDR &, ADDR, BYTE = 0);
	//Word interaction funcs
	struct WordInfo {
		std::string word;
		ADDR previousWord;
		ADDR myAddr;
	};
	WordInfo                  GetWordInfo(ADDR) const;                          //RtW
	ADDR                      FindWord(std::string);                      //RtW
	void                      Execute(ADDR);                              //iD
	void                      ExecuteWord(ADDR, ADDR = 0xFFFFFFFF);


	//Instructions
	//0x00 BRK - break
	//0x02 RET - return
	//0x04 STR - starts the line
	void Multiply(ADDR&);           // 0x20
	void Division(ADDR&);           // 0x21
	void Sum(ADDR&);                // 0x22
	void Difference(ADDR&);         // 0x23
	void Remainder(ADDR&);          // 0x24
	void DivRem(ADDR&);             // 0x25
    //Assembly instructions ptrs
	std::function<void(ADDR&)> **funcs;
public:
	void                      OpenConsole();                                  //RtW
	void                      CloseConsole();                                 //RtW
	void                      Update();                                       //RtW
	unsigned char**&          GetMatrix();                                    //RtW
	unsigned char***          GetMatrixPtr();                                 //RtW
	void                      ReceiveKey(const BYTE& key);                    //iD
/* structors */
public:
	VirtualMachine(unsigned int ID);
	~VirtualMachine();
};
#include "core.cpp"
#undef SIZE
#undef BYTE
#undef ADDR
#undef READ_LAST_TYPED_KEY
#undef WRITE_LAST_TYPED_KEY
#undef READ_KEYBOARD_DATA_SIZE
#undef WRITE_KEYBOARD_DATA_SIZE
#undef READ_CMD_PROC_MODE
#undef WRITE_CMD_PROC_MODE
#undef CONSOLE_CURSOR_X_MAX
#undef CONSOLE_CURSOR_Y_MAX
#undef READ_STACK_SIZE
#undef  WRITE_STACK_SIZE
#undef  READ_CONSOLE_CURSOR_X
#undef  WRITE_CONSOLE_CURSOR_X
#undef  READ_CONSOLE_CURSOR_Y
#undef  WRITE_CONSOLE_CURSOR_Y
#undef  READ_LAST_WORD_ADDRESS
#undef  WRITE_LAST_WORD_ADDRESS
#undef  FIRST_LOAD_BYTE