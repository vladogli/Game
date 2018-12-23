#include "core.h"
VirtualMachine::VirtualMachine(unsigned int ID) {
	mem = new Memory(0x10000);
	mem->Fill(0, 0, 0x10000);
	myID = ID;
	if (!LoadFromDisket()) {
		LoadSTDDisket();
		SaveToDisket();
	}
	matrix = new unsigned char*[50];
	for (size_t i = 0; i < 50; i++) {
		matrix[i] = &mem->memory[i*50];
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
	if(_Val.size()>0) {
		const unsigned char* _v = reinterpret_cast<const unsigned char*>(_Val.c_str());
		mem->Write( 0x1600, _v, _Val.size());
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
unsigned char**&	VirtualMachine::GetMatrix(){
	return matrix;
}
unsigned char***	VirtualMachine::GetMatrixPtr() {
	return &matrix;
}
void VirtualMachine::Update() {
	if (threadClosed)
		PrivateUpdate();
}
long long int VirtualMachine::ReadNumber(ADDR addr, BYTE size) {
	long long int returnValue = 0;
	for (BYTE i = 0; i < size; i++) {
		returnValue += mem->memory[addr + i] * (long long int)std::pow(256, i);
	}
	return returnValue;
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
void VirtualMachine::PrivateUpdate() {
	if (mem->Read(0x1500) == 0) {
		BYTE byte = mem->Read(0x10FF);
		if (byte == 0) return;
		mem->Write(0x10FF, 0);
		if (byte == '\r' || byte == '\n') {
		//	mem->Write(0x1500, 1);
			return;
		}
		BYTE itr = mem->Read(0x10FE);
		mem->Write(0x1000 + itr, byte);
		mem->Write(0x10FE, itr + 1);
		TypeLetter(byte);
		return;
	}
	else {

	}
}
void VirtualMachine::TypeLetter(BYTE byte) {
	BYTE cursorX = mem->Read(0x1502);
	BYTE cursorY = mem->Read(0x1503);
	if (cursorX == 79) {
		cursorX = 0;
		if (cursorY == 50) {
			Scroll();
		}
		else {
			cursorY++;
			mem->Write(0x1503, cursorY);
		}
	} 
	else {
		cursorX++;
	}
  	mem->Write(0x1502, cursorX);
	matrix[cursorY][cursorX] = byte;

}
void VirtualMachine::Scroll() {
	for (int j = 50; j > 0; j++) {
		memcpy(matrix[j - 1], matrix[j], 80);
	}
}
void VirtualMachine::Page() {
	mem->Fill(0, 0, 0x1000);
}