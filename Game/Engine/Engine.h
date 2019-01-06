#pragma once
#include "Graphics/Graphics.h"
#include "Input/Input.h"
#include "Config/Config.h"
#include "SpaceObjects/CalcSeed/CalcSeed.h"
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
	
	KEY* binds = new KEY[8]{
	VK_ESCAPE,				//Escape
	VK_UP,					//Move up
	VK_DOWN,				//Move down
	VK_RIGHT,				//Move right
	VK_LEFT,				//Move left
	VK_LBUTTON,				//LMB
	VK_RBUTTON,				//RMB
	'T'				        //Open chat
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