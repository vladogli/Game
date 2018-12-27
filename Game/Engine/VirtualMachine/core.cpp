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
	funcs = new std::function<void(ADDR&)>*[0x100];
	for (size_t i = 0; i < 0x100; i++) {
		funcs[i] = nullptr;
	}
	funcs[0x20] = new std::function<void(ADDR&)>(std::bind(&VirtualMachine::Multiply, this, std::placeholders::_1));
	funcs[0x21] = new std::function<void(ADDR&)>(std::bind(&VirtualMachine::Division, this, std::placeholders::_1));
	funcs[0x22] = new std::function<void(ADDR&)>(std::bind(&VirtualMachine::Sum, this, std::placeholders::_1));
	funcs[0x23] = new std::function<void(ADDR&)>(std::bind(&VirtualMachine::Difference, this, std::placeholders::_1));
	funcs[0x24] = new std::function<void(ADDR&)>(std::bind(&VirtualMachine::Remainder, this, std::placeholders::_1));
	funcs[0x25] = new std::function<void(ADDR&)>(std::bind(&VirtualMachine::DivRem, this, std::placeholders::_1));
}
void VirtualMachine::SaveToDisket() {
	boost::filesystem::ofstream File(disketSaveFolder + std::to_wstring(myID) + L".iso", std::ios_base::binary);
	for (size_t i = FIRST_LOAD_BYTE; i < mem->memSize; i++) {
		File << mem->memory[i];
	}
	File.close();
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
		mem->Write(FIRST_LOAD_BYTE, _v, _Val.size());
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
	mem->Write(FIRST_LOAD_BYTE, _v, _Val.size());
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
		WriteKey();
		return;
	}
	WriteWord();
}
void VirtualMachine::WriteWord()
{
	BYTE size = READ_KEYBOARD_DATA_SIZE; // Read kbSize
	std::string word;
	for (BYTE i = 0; i <= size; i++) {
		if (size != i && isSymbol(mem->Read(0x1000 + i))) {
			word += mem->Read(0x1000 + i);
			continue;
		}

		if (word[0] >= '0' && word[0] <= '9') { 
			WriteToStack(StringToInt(word)); 		
			word = "";
			continue;
		}
		if (word == ":") { // compile
			Compile(i);
			goto exit;
		}
		if (word != "")
		{
			if (word == ".") {
				unsigned int _Value;
				try {
					_Value = ReadFromStack();
				}
				catch (exceptions exc) {
					if (exc == STACK_UNDERFLOW) {
						TypeWord(" -1 stack undeflow");
						goto exit;
					}
					return;
				}
				TypeWord(" " + std::to_string(_Value));
				word = "";
				continue;
			}
			if (word == "SAVE") {
				SaveToDisket();
				continue;
			}
			if (word == "LOAD") {
				LoadFromDisket();
				Page();
				return;
			}
			try {
				Execute(FindWord(word));
				word = "";
			}
			catch (exceptions exc) {
				if (exc == UNKNOWN_TOKEN) {
					TypeWord(" UNKNOWN_TOKEN");
				}
				if (exc == STACK_UNDERFLOW) {
					TypeWord(" -1 stack undeflow");
				}
				if (exc == EXEC_ERROR) {
					TypeWord(" EXECUTION ERROR.");
				}
				goto exit;
			}
		}
	}
	TypeWord(" ok");

exit:
	mem->Fill(0x1000, 0, 0x100);
	WRITE_CMD_PROC_MODE(0);
	NextLine();
}
void VirtualMachine::WriteKey()
{
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
	mem->Fill(0, 0, 0x1100);
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
VirtualMachine::WordInfo VirtualMachine::GetWordInfo(ADDR addr) const {
	WordInfo val;
	if (addr < 0x1500) {
		val.myAddr = 0;
		val.previousWord = 0;
		return val;
	}
	val.myAddr = addr;
	BYTE itr = 0;
	addr--;
		while (Readu8(addr) != 4 && addr != 0 && itr <= 64) {
		val.word += Readu8(addr);
		addr--;
		itr++;
	}
	std::reverse(val.word.begin(), val.word.end());
	val.previousWord = Readu32(addr - 4);
	return val;
}
void VirtualMachine::Execute(ADDR addr) {
	if (Readu8(addr) > 3) {
		throw EXEC_ERROR;
		return;
	}
	if (Readu8(addr) >1) {
		WriteToStack(addr);
		return;
	}
	if (Readu8(addr) == 0) { // assembler
		addr++;
		while (Readu8(addr) != 0x02) {
			if (funcs[Readu8(addr)] == nullptr) {
				throw EXEC_ERROR;
				return;
			}
			(*funcs[Readu8(addr)])(std::ref(addr));
		}
		return;
	}
	addr++;
	ExecuteWord(addr);
}
void VirtualMachine::ExecuteWord(ADDR addr, ADDR to) {
	auto _Value = Readu32(addr);
	while (addr < to) {
		if (_Value < 0xFFFFFFFC) {
			try {
				Execute(_Value);
				addr += 4;
			}
			catch (exceptions exc) {
				throw exc;
				return;
			}
		}
		else if (_Value == 0xFFFFFFFD) {
			WriteToStack(addr += 4);
			while (Readu8(addr) != 0x00) {
				addr++;
			}
			addr++;
		}
		else if (_Value == 0xFFFFFFFE) {
			WriteToStack(Readu32(addr + 4));
			addr += 8;
		}
		else if (_Value == 0xFFFFFFFC) {
			ADDR _eV = Readu32(addr += 4); // else addr
			ADDR _tV = Readu32(addr += 4); // then addr
			if (_eV != 0) {
				if (ReadFromStack()) {
					ExecuteWord(addr += 4, _eV);
				}
				else {
					ExecuteWord(_eV, _tV);
				}
			}
			else if (ReadFromStack()) {
				ExecuteWord(addr += 4, _tV);
			}
			addr = _tV;
		}
		else {
			return;
		}
		_Value = Readu32(addr);
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
ADDR VirtualMachine::FindWord(std::string Name) {
	auto _Value = GetWordInfo(READ_LAST_WORD_ADDRESS);
	while (_Value.word != Name) {
		if (_Value.previousWord < 0x1500) {
			throw UNKNOWN_TOKEN;
			return -1;
		}
		_Value = GetWordInfo(_Value.previousWord);
	}
	return _Value.myAddr;
}
void VirtualMachine::Compile(BYTE iterator) {
	ADDR nowAddr = READ_LAST_WORD_ADDRESS;
	if (Readu8(nowAddr) == 0) {
		while (Readu8(nowAddr) != 0x02) {
			if (nowAddr > mem->memSize) {
				throw UNKNOWN_ERROR;
				return;
			}
			nowAddr++;
		}
		nowAddr++;
	} 
	else if (Readu8(nowAddr) == 1) {
		nowAddr++;
		while (1) {
			if (nowAddr + 16 > mem->memSize) {
				throw UNKNOWN_ERROR;
				return;
			}
			auto _Value = Readu32(nowAddr);
			if (_Value < 0xFFFFFFFD) {
				nowAddr += 4;
			}
			else if (_Value == 0xFFFFFFFD) {
				WriteToStack(nowAddr += 4);
				while (Readu8(nowAddr) != 0x00) {
					nowAddr++;
				}
				nowAddr++;
			}
			else if (_Value == 0xFFFFFFFE) {
				WriteToStack(Readu32(nowAddr + 4));
				nowAddr += 8;
			}
			else {
				nowAddr += 4;
				break;
			}
		}
	}
	else {
		nowAddr += Readu16(nowAddr += 1) + 1;
	}
	Writeu32(nowAddr, READ_LAST_WORD_ADDRESS);
	nowAddr += 4;
	iterator++;
	std::string word;
	BYTE size = READ_KEYBOARD_DATA_SIZE;
	for (; iterator <= size; iterator++) {
		if (size != iterator && isSymbol(mem->Read(0x1000 + iterator))) {
			word += mem->Read(0x1000 + iterator);
			continue;
		}
		else {
			break;
		}
	}
	Writeu8(nowAddr, 4);
	nowAddr++;
	for (size_t i = 0; i < word.size(); i++, nowAddr++) {
		Writeu8(nowAddr, (unsigned char)(word[i]));
	}
	if (word.size() < 1) {
		NextLine();
		TypeWord("ERROR. UNKNOWN_TOKEN");
		return;
	}
	Writeu8(nowAddr, 1); // ENT point to ptrs word
	ADDR execPart = nowAddr;
	nowAddr++;
	try{
		if (CompileStr(nowAddr, execPart, iterator)) { // +2 because iterator on :
			WRITE_LAST_WORD_ADDRESS(execPart);
			return;
		}
	}
	catch (exceptions exc) {
		if (exc == UNKNOWN_TOKEN) {
			NextLine();
			TypeWord("ERROR. UNKNOWN_TOKEN");
		}
		return;
	}
	NextLine();
	TypeWord("compile: ");
	mem->Fill(0x1000, 0, 0x100);
	while (1) {
		BYTE byte = READ_LAST_TYPED_KEY;
		if (byte == 0) {
			continue;				
		}
		WRITE_LAST_TYPED_KEY(0);

		if (byte == '\r' || byte == '\n') {	//If Enter
			try {
				if (CompileStr(nowAddr, execPart)) {
					WRITE_LAST_WORD_ADDRESS(execPart);
					return;
				}
				mem->Fill(0x1000, 0, 0x100);
			}
			catch (exceptions exc) {
				if (exc == UNKNOWN_TOKEN) {
					NextLine();
					TypeWord("ERROR. UNKNOWN_TOKEN");
				}
				return;
			}
			NextLine();
			TypeWord("compile: ");
			mem->Fill(0x1000, 0, 0x100);
			continue;
		}
		if (byte == '\b') {// backspace 
			if (READ_KEYBOARD_DATA_SIZE >= 1) {
				WRITE_KEYBOARD_DATA_SIZE(READ_KEYBOARD_DATA_SIZE - 1);
				Backspace();
			}
			continue;
		}
		if (byte >= 32 && byte <= 126) {
			BYTE itr = READ_KEYBOARD_DATA_SIZE;             //Reading kbItr
			mem->Write(0x1000 + itr, byte);					//Writing key to kbData
			WRITE_KEYBOARD_DATA_SIZE(itr + 1);              //kbItr++
			TypeWord(std::string(1, char(byte)));			//show key
		}
	}
}
bool VirtualMachine::CompileStr(ADDR &nowAddr, ADDR execpart, BYTE iterator) {
	BYTE size = READ_KEYBOARD_DATA_SIZE; // Read kbSize
	std::vector<std::string> words;
	for (std::string word; iterator <= size; iterator++) {
		if (size != iterator && isSymbol(mem->Read(0x1000 + iterator))) {
			word += mem->Read(0x1000 + iterator);
			continue;
		} else 
		if (word != "")	{
			words.push_back(word);
			word = "";
		}
	}
	for (size_t i = 0; i < words.size(); i++) {
		if (words[i] == ";") {
			Writeu32(nowAddr, 0xFFFFFFFF);
			return 1; // end;
		}
		if (words[i] == "IF") {
			Writeu32(nowAddr, 0xFFFFFFFC);
			Writeu32(nowAddr += 4, 0x0);
			Writeu32(nowAddr += 4, 0x0);
			nowAddr += 4;
			continue;
		}
		if (words[i] == "ELSE") {
			ADDR elseAddr = nowAddr;
			while (1) {
				if (execpart == nowAddr) {
					throw UNKNOWN_TOKEN;
					return 0;
				}
				nowAddr--;
				if (Readu32(nowAddr) == 0xFFFFFFFC && 
					Readu32(nowAddr + 4) == 0x0    && 
					Readu32(nowAddr + 8) == 0x0) {
					Writeu32(nowAddr + 4, elseAddr);
					break;
				}
    		}
			nowAddr = elseAddr;
			continue;
		}
		if (words[i] == "THEN") {
			ADDR thenAddr = nowAddr;
			while (1) {
				nowAddr--;
				if (execpart == nowAddr) {
					throw UNKNOWN_TOKEN;
					return 0;
				}
				if (Readu32(nowAddr) == 0xFFFFFFFC && Readu32(nowAddr + 8) == 0x0) {
					Writeu32(nowAddr + 8, thenAddr);
					break;
				}
			}
			nowAddr = thenAddr;
			continue;
		}
		if (words[i][0] >= '0' && words[i][0] <= '9') {
			Writeu32(nowAddr, 0xFFFFFFFE);
			Writeu32(nowAddr + 4, StringToInt(words[i]));
			nowAddr += 8;
			continue;
		}
		ADDR _Value;
		try {
			_Value = FindWord(words[i]);
		}
		catch(...) {
			throw UNKNOWN_TOKEN;
		}
		Writeu32(nowAddr, _Value);
		nowAddr += 4;
	}
	return 0;
}
void VirtualMachine::Multiply(ADDR &addr) {
	WriteToStack(ReadFromStack() * ReadFromStack());
	addr++;
}
void VirtualMachine::Division(ADDR &addr) {
	auto _Val1 = ReadFromStack();
	auto _Val2 = ReadFromStack();
    WriteToStack(_Val2 / _Val1);
	addr++;
}
void VirtualMachine::Sum(ADDR &addr) {
	WriteToStack(ReadFromStack() + ReadFromStack());
	addr++;
}
void VirtualMachine::Difference(ADDR &addr) {
	auto _Val1 = ReadFromStack();
	auto _Val2 = ReadFromStack();
	WriteToStack(_Val2 - _Val1);
	addr++;
}
void VirtualMachine::Remainder(ADDR &addr) {
	auto _Val1 = ReadFromStack();
	auto _Val2 = ReadFromStack();
	WriteToStack(_Val2 % _Val1);
	addr++;
}
void VirtualMachine::DivRem(ADDR &addr) {
	auto _Val1 = ReadFromStack();
	auto _Val2 = ReadFromStack();
	WriteToStack(_Val2 % _Val1);
	WriteToStack(_Val2 / _Val1);
	addr++;
}


