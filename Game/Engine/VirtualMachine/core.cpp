#include "core.h"
VirtualMachine::VirtualMachine(unsigned int ID) {
	mem = new Memory(0x10000);
	mem->Fill(0, 0, 0x10000);
	myID = ID;
	if (!LoadFromDisket()) {
		LoadSTDDisket();
		SaveToDisket();
	}
	matrix = new unsigned char*[80];
	for (size_t i = 0; i < 80; i++) {
		matrix[i] = &mem->memory[i*80];
	}
}
void VirtualMachine::SaveToDisket() {
	boost::filesystem::wofstream File(SAVE_FOLDER + std::to_wstring(myID) + L".iso");
	BYTE* dest;
	mem->Read(dest, 0x1500, 0, 0x7500);
	File << dest;
	File.close();
	delete dest;
}
bool VirtualMachine::LoadFromDisket() {
	if (!boost::filesystem::exists(SAVE_FOLDER + std::to_wstring(myID) + L".iso")) {
		return 0;
	}
	boost::filesystem::wifstream File(SAVE_FOLDER + std::to_wstring(myID) + L".iso");
	std::string _Val, _Val1;
	while (getline(File, _Val1)) {
		_Val += _Val1;
	}
	File.close();
	mem->Write(0x1500, _Val.c_str(), _Val.size());
	return 1;
}
void VirtualMachine::LoadSTDDisket() {
	boost::filesystem::wifstream File(SAVE_FOLDER + L"std.iso");
	std::string _Val, _Val1;
	while (getline(File, _Val1)) {
		_Val += _Val1;
	}
	File.close();
	mem->Write(0x1500, _Val.c_str(), _Val.size());
	return 1;
}
VirtualMachine::~VirtualMachine() {
	delete mem;
	delete[] matrix;
}
char**&	VirtualMachine::GetMatrix() { 
	return matrix;
}
void VirtualMachine::Update() {
	if (threadClosed)
		PrivateUpdate();
}
long long int VirtualMachine::ReadNumber(ADDR addr, BYTE size) {
	long long int returnValue = 0;
	for (BYTE i = 0; i < size; i++) {
		returnValue += mem->memory[addr + i] * std::pow(256, i);
	}
	return returnValue;
}
void VirtualMachine::ReceiveKey(const BYTE& key) {
	BYTE size = ReadNumber(0x10FF, 1);
	if (size >= 0xFF) {
		mem->Write(0x10FF, size - 0x10);
		mem->Write(0xFF0 + size, key);
	}
	mem->Write(0x1000 + size, &BYTE, 1);
	mem->Write(0x10FF, size + 1);
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
		std::bind(VirtualMachine::UpdateThread, this));
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
	
}