#pragma once
#include "Graphics/Graphics.h"
#include "VirtualMachine/core.h"
#include "Input.h"
#include "Config.h"
#include <boost/filesystem.hpp>
//From 0 to 1000
#define u32 unsigned int
#define ERROR_VAL u32
#define GAME_ENGINE_ERROR_NOT_INITIALIZED 0x0000FFFF
class GameEngine {
private:
	ERROR_VAL lastError = GAME_ENGINE_ERROR_NOT_INITIALIZED;
	void		SetLastError(const ERROR_VAL);
	Graphics	graphics;
	Input		input;
	
	KEY* binds = new KEY[7]{
	VK_ESCAPE,				//Escape
	VK_UP,					//Move up
	VK_DOWN,				//Move down
	VK_RIGHT,				//Move right
	VK_LEFT,				//Move left
	VK_LBUTTON,				//LMB
	VK_RBUTTON				//RMB
	};
	unsigned char bindsSize;
	void SaveKeyConfig(const std::wstring PathToFile) const;
	void LoadKeyConfig(const std::wstring PathToFile);
public:
	GameEngine();
	ERROR_VAL		GetLastError()	const;
	void			Join();
};
#include "Engine.cpp"
#undef u32
#undef ERROR_VAL