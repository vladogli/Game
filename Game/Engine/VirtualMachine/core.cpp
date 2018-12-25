#include "core.h"
VirtualMachine::VirtualMachine(unsigned int ID) {
	mem = new Memory(0x10000);
	mem->Fill(0, 0, 0x10000);
	myID = ID;
	if (!LoadFromDisket()) {
		LoadSTDDisket();
		SaveToDisket();
	}
	matrix = new unsigned char*[CONSOLE_CURSOR_Y_MAX];
	for (size_t i = 0; i < CONSOLE_CURSOR_Y_MAX; i++) {
		matrix[i] = &mem->memory[i*CONSOLE_CURSOR_X_MAX];
	}
	funcs = new std::function<BYTE(ADDR addr)>*[0x100];
	for (size_t i = 0; i < 0x100; i++) {
		funcs[i] = nullptr;
	}
}
void VirtualMachine::SaveToDisket() {
	boost::filesystem::ofstream File(disketSaveFolder + std::to_wstring(myID) + L".iso");
	BYTE* dest;
	mem->Read(0x1600, dest, 0x7400);
	File << dest;
	File.close();
	delete dest;
}
bool VirtualMachine::LoadFromDisket() {
	if (!boost::filesystem::exists(disketSaveFolder + std::to_wstring(myID) + L".iso")) {
		return 0;
	}
	boost::filesystem::ifstream File(disketSaveFolder + std::to_wstring(myID) + L".iso");
	std::string _Val, _Val1;
	while (getline(File, _Val1)) {
		_Val += _Val1;
	}
	File.close();
	if (_Val.size() > 0) {
		const unsigned char* _v = reinterpret_cast<const unsigned char*>(_Val.c_str());
		mem->Write(0x1600, _v, _Val.size());
		delete _v;
	}
	return 1;
}
void VirtualMachine::LoadSTDDisket() {
	if (!boost::filesystem::exists(disketSaveFolder + L"std.iso")) {
		return;
	}
	boost::filesystem::ifstream File(disketSaveFolder + L"std.iso");
	std::string _Val, _Val1;
	while (getline(File, _Val1)) {
		_Val += _Val1;
	}
	File.close();
	const unsigned char* _v = reinterpret_cast<const unsigned char*>(_Val.c_str());
	mem->Write(0x1600, _v, _Val.size());
	delete _v;
	return;
}
VirtualMachine::~VirtualMachine() {
	delete mem;
	delete[] matrix;
}
unsigned char**&	VirtualMachine::GetMatrix() {
	return matrix;
}
unsigned char***	VirtualMachine::GetMatrixPtr() {
	return &matrix;
}
void VirtualMachine::Update() {
	if (threadClosed)
		PrivateUpdate();
}
BYTE VirtualMachine::Readu8(ADDR addr) const {
	return mem->memory[addr];
}
unsigned short int	VirtualMachine::Readu16(ADDR addr) const {
	return	mem->memory[addr]
		+ mem->memory[addr + 1] * 256;
}
unsigned int VirtualMachine::Readu32(ADDR addr) const {
	return	mem->memory[addr]
		+ mem->memory[addr + 1] * 256
		+ mem->memory[addr + 2] * 256 * 256
		+ mem->memory[addr + 3] * 256 * 256 * 256;

}
unsigned long long int VirtualMachine::Readu64(ADDR addr) const {
	return	mem->memory[addr]
		+ mem->memory[addr + 1] * 256
		+ mem->memory[addr + 2] * 256 * 256
		+ mem->memory[addr + 3] * 256 * 256 * 256
		+ mem->memory[addr + 4] * 256 * 256 * 256 * 256
		+ mem->memory[addr + 5] * 256 * 256 * 256 * 256 * 256
		+ mem->memory[addr + 6] * 256 * 256 * 256 * 256 * 256 * 256
		+ mem->memory[addr + 7] * 256 * 256 * 256 * 256 * 256 * 256 * 256;
}
void VirtualMachine::Writeu8(ADDR addr, BYTE number) {
	mem->Write(addr, number);
}
void VirtualMachine::Writeu16(ADDR addr, unsigned short int number) {
	mem->Write(addr, number % 256);
	mem->Write(addr + 1, (number / 256) % 256);
}
void VirtualMachine::Writeu32(ADDR addr, unsigned int number) {
	mem->Write(addr, number % 256);
	mem->Write(addr + 1, (number /= 256) % 256);
	mem->Write(addr + 2, (number /= 256) % 256);
	mem->Write(addr + 3, (number /= 256) % 256);
}
void VirtualMachine::Writeu64(ADDR addr, unsigned long long int number) {
	mem->Write(addr, number % 256);
	mem->Write(addr + 1, (number /= 256) % 256);
	mem->Write(addr + 2, (number /= 256) % 256);
	mem->Write(addr + 3, (number /= 256) % 256);
	mem->Write(addr + 4, (number /= 256) % 256);
	mem->Write(addr + 5, (number /= 256) % 256);
	mem->Write(addr + 6, (number /= 256) % 256);
	mem->Write(addr + 7, (number /= 256) % 256);
}
void VirtualMachine::ReceiveKey(const BYTE& key) {
	mem->Write(0x10FF, key);
}
void VirtualMachine::UpdateThread() {
	while (!threadClosed) {
		PrivateUpdate();
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
	threadClosed = 0;
}
void VirtualMachine::OpenConsole() {
	threadClosed = 0;
	myThread = new boost::thread(
		std::bind(&VirtualMachine::UpdateThread, this));
}
void VirtualMachine::CloseConsole() {
	threadClosed = 1;
	while (threadClosed != 0) {
		std::this_thread::sleep_for(std::chrono::milliseconds(25));
	}
	delete myThread;
	threadClosed = 1;
}
bool VirtualMachine::isSymbol(BYTE _Value) {
	return _Value > 32 && _Value <= 126;
}
unsigned int VirtualMachine::StringToInt(std::string _Val) {
	unsigned int returnValue = 0;
	BYTE _V = 0;
	for (size_t i = _Val.size() - 1; i != size_t(-1); i--, _V++) {
		if (_Val[i] >= '0' && _Val[i] <= '9') {
			returnValue += (_Val[i] - '0') * (unsigned int)std::pow(10, _V);
		}
		else {
			break;
		}
	}
	return returnValue;
}
void VirtualMachine::PrivateUpdate() {
	if (READ_CMD_PROC_MODE == 0) {
		BYTE byte = READ_LAST_TYPED_KEY;	//Reading a last typed key
		if (byte == 0) {
			return;				//If we doesn't have any key - exit
		}
		WRITE_LAST_TYPED_KEY(0);

		if (byte == '\r' || byte == '\n') {	//If Return
			WRITE_CMD_PROC_MODE(1);			//Sets PROCCESSING_MODE
			return;
		}

		if (byte == '\b') {// backspace 
			if (READ_KEYBOARD_DATA_SIZE >= 1) {
				WRITE_KEYBOARD_DATA_SIZE(READ_KEYBOARD_DATA_SIZE - 1);
				Backspace();
			}
			return;
		}

		if (byte >= 32 && byte <= 126) {
			BYTE itr = READ_KEYBOARD_DATA_SIZE;             //Reading kbItr
			mem->Write(0x1000 + itr, byte);					//Writing key to kbData
			WRITE_KEYBOARD_DATA_SIZE(itr + 1);              //kbItr++
			TypeWord(std::string(1, char(byte)));			//show key
		}
		return;
	}
	else {
		BYTE size = READ_KEYBOARD_DATA_SIZE; // Read kbSize
		std::string word;
		for (BYTE i = 0; i <= size; i++) {
			if (size != i && isSymbol(mem->Read(0x1000 + i))) {
				word += mem->Read(0x1000 + i);
				continue;
			}
			//else {
			if (word[0] >= '0' && word[0] <= '9') { // if word is integer
				WriteToStack(StringToInt(word)); // Write to stack this integer					
				word = "";
				continue;
			}
			//else 
			if (word != "")
			{
				if (word == ".") {
					unsigned int _Value;
					try {
						_Value = ReadFromStack();
					}
					catch (exceptions exc) {
						if (exc == STACK_UNDERFLOW) {
							NextLine();
							TypeWord("-1 stack undeflow");
							goto exit;
						}
						return;
					}
					TypeWord(" " + std::to_string(_Value));
					word = "";
					continue;
				}
				auto lastWordAddr = Readu16(0x1504);		//Reading lastWordAddr
				while (ReadFuncName(lastWordAddr) != word) {
					lastWordAddr = Readu16(lastWordAddr);

					if (lastWordAddr < 0x1500) {
						NextLine();
						TypeWord(" UNKNOWN TOKEN: " + word);
						goto exit;
					}
				}
				word = "";
				try {
					Execute(lastWordAddr);
				}
				catch (exceptions exc) {
					if (exc == EXEC_ERROR) {
						NextLine();
						TypeWord(" EXECUTION ERROR: " + word);
						goto exit;
					}
				}
				//}
			}
		}
		TypeWord(" ok");
	exit:
		mem->Fill(0x1000, 0, 0x100);
		WRITE_CMD_PROC_MODE(0);
		NextLine();
	}
}
void VirtualMachine::TypeWord(std::string Word) {
	BYTE x, y;
	x = READ_CONSOLE_CURSOR_X;
	y = READ_CONSOLE_CURSOR_Y;
	for (int i = 0; i < Word.size(); i++)
	{
		x++;
		if (x >= CONSOLE_CURSOR_X_MAX)
		{
			y++;
			x = 0;
		}
		if (y >= CONSOLE_CURSOR_Y_MAX) {
			Scroll();
			y = READ_CONSOLE_CURSOR_Y;
		}
		matrix[y][x - 1] = (unsigned char)Word[i];
	}
	WRITE_CONSOLE_CURSOR_X(x);
	WRITE_CONSOLE_CURSOR_Y(y);
	//Show word at coords
}
void VirtualMachine::NextLine() {
	BYTE y = READ_CONSOLE_CURSOR_Y;
	if (y == 49) {
		WRITE_CONSOLE_CURSOR_Y(51);
	}
	else {
		WRITE_CONSOLE_CURSOR_Y(y + 1);
	}
	WRITE_CONSOLE_CURSOR_X(0);
	if (!READ_CMD_PROC_MODE) {
		TypeWord(">");
	}
	//To Next Line
}
void VirtualMachine::Scroll() {
	for (int j = 0; j < CONSOLE_CURSOR_X_MAX; j++) {
		matrix[CONSOLE_CURSOR_Y_MAX - 1][j] = 0;
	}
	for (int j = 1; j < CONSOLE_CURSOR_Y_MAX; j++) {
		memcpy(matrix[j - 1], matrix[j], CONSOLE_CURSOR_X_MAX);
	}
	WRITE_CONSOLE_CURSOR_Y(READ_CONSOLE_CURSOR_Y - 2);
}
void VirtualMachine::Page() {
	mem->Fill(0, 0, 0x1000);
}
void VirtualMachine::WriteToStack(unsigned int val) {
	BYTE _Val = READ_STACK_SIZE;
	if (_Val >= 127) {
		memcpy(&mem->memory[0x1300], &mem->memory[0x1304], 0x1F8);
		Writeu32(0x14FC, val);
		return;
	}
	Writeu32(0x1300 + (_Val * 4), val);
	WRITE_STACK_SIZE(_Val + 1);
}
unsigned int VirtualMachine::ReadFromStack() {
	char _Val = READ_STACK_SIZE - 1;
	if (_Val < 0) {
		throw STACK_UNDERFLOW;
		return 0;
	}
	WRITE_STACK_SIZE(_Val);
	return Readu32(0x1300 + (_Val * 4));
}
std::string VirtualMachine::ReadFuncName(ADDR addr) {
	if (addr < 1000) {
		return "";
	}
	std::string word;
	int itr = 2;
	BYTE _Value = mem->Read(addr + itr);
	while (_Value != 0x02 && itr != 0x40) {
		word += _Value;
		_Value = mem->Read(addr + itr);
		itr++;
	}
	return word;
}
void VirtualMachine::Execute(ADDR addr) {
	ADDR previousWordAddr = addr;
	std::string word_name = ReadFuncName(addr);
	size_t itr = 2;
	BYTE _Value = mem->Read(addr + itr);
	while (_Value != 0x02) {
		if (itr == 0x40) {
			throw EXEC_ERROR;
			return;
		}
		word_name += _Value;
		_Value = mem->Read(addr + itr);
		itr++;
	}
	while (mem->Read(addr + itr) != 0xFF && itr <= mem->memSize - 32) {
		for (int i = 0; i < 0x100; i++) {
			if (funcs[mem->Read(addr + itr)] == nullptr) {
				throw EXEC_ERROR;
				return;
			}
			else {
				itr += (*funcs[mem->Read(addr + itr)])(addr + itr);
			}
		}
	}
}
void VirtualMachine::Backspace() {
	BYTE x = READ_CONSOLE_CURSOR_X - 1;
	if (x < 0) return;
	BYTE y = READ_CONSOLE_CURSOR_Y;
	matrix[y][x] = ' ';
	if (x == 1) {
		if (READ_CMD_PROC_MODE) {
			WRITE_CONSOLE_CURSOR_X(x);
		}
	}
	else {
		WRITE_CONSOLE_CURSOR_X(x);
	}
}