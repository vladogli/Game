#include "core.h"
VirtualMachine::VirtualMachine(unsigned int ID, SIZE size) {
	mem = new Memory(size);
	mem->Fill(0, 0, size);
	myID = ID;
	if (!LoadFromDisket()) {
		LoadSTDDisket();
		SaveToDisket();
	}
}
void VirtualMachine::SaveToDisket() {
	boost::filesystem::wofstream File(SAVE_FOLDER + std::to_wstring(myID) + L".iso");
	BYTE* dest;
	mem->Read(dest, 0x50A, 0, size - 0x50A);
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
	mem->Write(0x500, _Val.c_str(), size - 0x500);
	return 1;
}
void VirtualMachine::LoadSTDDisket() {
	boost::filesystem::wifstream File(SAVE_FOLDER + L"std.iso");
	std::string _Val, _Val1;
	while (getline(File, _Val1)) {
		_Val += _Val1;
	}
	File.close();
	mem->Write(0x500, _Val.c_str(), size - 0x500);
	return 1;
}
VirtualMachine::~VirtualMachine() {
	delete mem;
}