#include "Engine.h"
#include <iostream>
#define u32 unsigned int
#define ERROR_VAL u32
void GameEngine::SetLastError(const ERROR_VAL _Value) {
	lastError = _Value;
}
ERROR_VAL GameEngine::GetLastError() const {
	return lastError;
}
GameEngine::GameEngine() {
}
void GameEngine::Join() {
	u32 _Value = lastError & 0xFFFF;
	if (_Value == 0xFFFF) {
		return;
	}
}
void GameEngine::SaveKeyConfig(const std::wstring PathToFile) const {
	boost::filesystem::ofstream file(PathToFile);
	file << bindsSize;
	file << '0';
	for (int i = 0; i < bindsSize; i++) {
		file << binds[i];
	}
	file.close();
}
void GameEngine::LoadKeyConfig(const std::wstring PathToFile) {  
	if (boost::filesystem::exists(PathToFile)) {
		boost::filesystem::ifstream file(PathToFile);
		std::string _Value1, _Value2;
		while (std::getline(file, _Value2)) {
			_Value1 += _Value2;
		}
		file.close();
		if (_Value1.size() > 1) {
			delete binds;
			bindsSize = _Value1[0];
			for (size_t i = 1; i < _Value1.size(); i++) {
				binds[i - 1] = _Value1[i];
			}
		}
	}
}
#undef u32
#undef ERROR_VAL