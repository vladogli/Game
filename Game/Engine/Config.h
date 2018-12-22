#pragma once
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#define CONFIG_ERROR_WRONG_ID	0xFE
class Config {
/* variables */
private:
	std::vector<std::pair<std::wstring, std::wstring>> params;
	const std::wstring pathToFile;
	mutable unsigned char lastError;
/* functions */
private:
	void										SetLastError(unsigned char) const;
public:
	template<typename T,
		typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type>
		size_t										AddParameter(const std::wstring, const T);

	bool										EditParameter(const size_t, const std::wstring);
	bool										EditParameter(const std::wstring, const  std::wstring);
	void										Load();
	void										Save()							const;
	const std::vector<std::pair<std::wstring, std::wstring>>& GetParameterList()const;
	std::wstring								GetParameter(const size_t)		const;
	std::wstring								GetParameter(const std::wstring)const;
	unsigned char								GetLastError()					const;
/* operators */
public:
	std::wstring operator[](std::wstring)const;
	std::wstring operator[](size_t)const;
/* constructors */
public:
	Config(std::wstring);
	~Config() { Save(); }
};
#include "Config.cpp"