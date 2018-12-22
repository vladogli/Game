#include "Config.h"
Config::Config(std::wstring pathToFile) : pathToFile(pathToFile) {
	if (boost::filesystem::exists(pathToFile)) {
		Load();
	}
	else {
		Save();
	}
}
void Config::SetLastError(unsigned char _Value) const {
	lastError = _Value;
}
template<typename T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
	size_t Config::AddParameter(const std::wstring _Value, const T t){
	GetParameter(_Value);
	if (GetLastError() != CONFIG_ERROR_WRONG_ID)
		return -1;
	params.push_back(std::pair<std::wstring, std::wstring>(_Value, std::to_wstring(t)));
	return params.size() - 1;
}
bool Config::EditParameter(const size_t ID, const std::wstring _Value){
	if ( ID >= params.size() ) {
		SetLastError(0);
		params[ID].second = _Value;
		return 1;
	}
	SetLastError(CONFIG_ERROR_WRONG_ID);
	return 0;
}
bool Config::EditParameter(const std::wstring _fValue, const  std::wstring _eValue) {
	for (size_t i = 0; i < params.size(); i++) {
		if (params[i].first == _fValue) {
			params[i].second = _eValue;
			return 1;
		}
	}
	SetLastError(CONFIG_ERROR_WRONG_ID);
	return 0;
}
void Config::Load() {
	if (!boost::filesystem::exists(pathToFile)) return;
	boost::filesystem::wifstream file;
	file.open(pathToFile);
	std::wstring buffer;
	while (std::getline(file, buffer)) {
		std::pair<std::wstring, std::wstring> writeValue;
		size_t i;
		for (i = 0; i < buffer.size(); i++) {
			if (i != 0) {
				if (buffer[i - 1] != '\\' && buffer[i] == ';') {
					break;
				}
			}
			writeValue.first += buffer[i];
		}
		i++;
		for (;i < buffer.size(); i++) {
			writeValue.second += buffer[i];
		}
		params.push_back(writeValue);
	}
	file.close(); 
}
void Config::Save() const {
	boost::filesystem::wofstream File(pathToFile);
	for (size_t i = 0; i < params.size(); i++) {
		std::wstring Write = L"";
		for (size_t j = 0; j < params[i].first.size(); j++) {
			if (params[i].first[j] == ';')
				Write += L"\\;";
			else Write += params[i].first[j];
		}
		Write += ';';
		Write += params[i].second;
		File << Write << std::endl;
	}
	File.close();
}
std::wstring Config::GetParameter(const size_t ID) const{
	if (ID >= params.size()) {
		return params[ID].second;
	}
	SetLastError(CONFIG_ERROR_WRONG_ID);
	return L"";
}
std::wstring Config::GetParameter(const std::wstring _Value)const {
	for (size_t i = 0; i < params.size(); i++) {
		if (params[i].first == _Value) {
			return params[i].second;
		}
	}
	SetLastError(CONFIG_ERROR_WRONG_ID);
	return L"";
}
unsigned char Config::GetLastError()				const {
	unsigned char returnValue = lastError;
	lastError = 0;
	return returnValue;
}
const std::vector<std::pair<std::wstring, std::wstring>>& Config::GetParameterList() const { return params; }
std::wstring Config::operator[](size_t _Value)		const { return GetParameter(_Value); }
std::wstring Config::operator[](std::wstring _Value)const { return GetParameter(_Value); }