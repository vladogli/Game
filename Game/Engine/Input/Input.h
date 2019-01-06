#pragma once
#include <Windows.h>
#include <WinUser.h>
#include <functional>
#include <boost/thread.hpp>
#include <vector>
#define KEY unsigned char
#define KeyPair std::pair<KEY, std::string>
class Input {
/* structures */
public:
	struct KeyCombination {
		enum  state { HOLDING, RELEASED } myState;
		KEY *values;
		unsigned char size;
		bool Called = 1;
		std::function<void( Input::KeyCombination::state& )> callBackFuncPtr;
		~KeyCombination();
		KeyCombination() {}
	};
/* variables */
private:
	unsigned char closeT = 0;
	boost::thread *myThread1 = NULL, *myThread2 = NULL;
	std::vector<KeyCombination*> combinations;
public:
	KeyCombination::state indicator[128];
	KeyPair *KeyMassive = new KeyPair[128]{
KeyPair(VK_LBUTTON,		"Left Mouse Button"),			KeyPair(VK_RBUTTON,		"Right Mouse Button"),
KeyPair(VK_CANCEL,		"Control-break processing"),	KeyPair(VK_MBUTTON,		"Middle mouse button (three-button mouse)"),
KeyPair(VK_BACK,		"Backspace"),					KeyPair(VK_TAB,			"Tab"),
KeyPair(VK_CLEAR,		"Clear"),						KeyPair(VK_RETURN,		"Enter"),
KeyPair(VK_SHIFT,		"Shift"),						KeyPair(VK_CONTROL,		"Control"),
KeyPair(VK_MENU,		"Alt"),							KeyPair(VK_PAUSE,		"Pause"),
KeyPair(VK_CAPITAL,		"Caps Lock"),					KeyPair(VK_ESCAPE,		"Esc"),
KeyPair(VK_SPACE,		"Spacebar"),
KeyPair(VK_PRIOR,		"Page Up"),						KeyPair(VK_NEXT,		"Page Down"),
KeyPair(VK_END,			"End"),							KeyPair(VK_HOME,		"Home"),
KeyPair(VK_LEFT,		"Left arrow"),					KeyPair(VK_UP,			"Up arrow"),
KeyPair(VK_RIGHT,		"Right arrow"),					KeyPair(VK_DOWN,		"Down arrow"),
KeyPair(VK_SELECT,		"Select"),						KeyPair(VK_PRINT,		"Print"),
KeyPair(VK_EXECUTE,		"Execute"),						KeyPair(VK_SNAPSHOT,	"PrintScreen"),
KeyPair(VK_INSERT,		"Insert"),						KeyPair(VK_DELETE,		"Delete"),
KeyPair(VK_HELP,		"Help"),						KeyPair(0x30,			"0"),
KeyPair(0x31,			"1"),							KeyPair(0x32,			"2"),
KeyPair(0x33,			"3"),							KeyPair(0x34,			"4"),
KeyPair(0x35,			"5"),							KeyPair(0x36,			"6"),
KeyPair(0x37,			"7"),							KeyPair(0x38,			"8"),
KeyPair(0x39,			"9"),
KeyPair('A',			"A"),							KeyPair('B',			"B"),
KeyPair('C',			"C"),							KeyPair('D',			"D"),
KeyPair('E',			"E"),							KeyPair('F',			"F"),
KeyPair('G',			"G"),							KeyPair('H',			"H"),
KeyPair('I',			"I"),							KeyPair('J',			"J"),
KeyPair('K',			"K"),							KeyPair('L',			"L"),
KeyPair('M',			"M"),							KeyPair('N',			"N"),
KeyPair('O',			"O"),							KeyPair('P',			"P"),
KeyPair('Q',			"Q"),							KeyPair('R',			"R"),
KeyPair('S',			"S"),							KeyPair('T',			"T"),
KeyPair('U',			"U"),							KeyPair('V',			"V"),
KeyPair('W',			"W"),							KeyPair('X',			"X"),
KeyPair('Y',			"Y"),							KeyPair('Z',			"Z"),
KeyPair(VK_LWIN,		"Left Windows Key"),			KeyPair(VK_RWIN,		"Right Windows Key"),
KeyPair(VK_NUMPAD0,		"Numeric keypad 0"),			KeyPair(VK_NUMPAD1,		"Numeric keypad 1"),
KeyPair(VK_NUMPAD2,		"Numeric keypad 2"),			KeyPair(VK_NUMPAD3,		"Numeric keypad 3"),
KeyPair(VK_NUMPAD4,		"Numeric keypad 4"),			KeyPair(VK_NUMPAD5,		"Numeric keypad 5"),
KeyPair(VK_NUMPAD6,		"Numeric keypad 6"),			KeyPair(VK_NUMPAD7,		"Numeric keypad 7"),
KeyPair(VK_NUMPAD8,		"Numeric keypad 8"),			KeyPair(VK_NUMPAD9,		"Numeric keypad 9"),
KeyPair(VK_MULTIPLY,	"Multiply"),					KeyPair(VK_ADD,			"Add"),
KeyPair(VK_SEPARATOR,	"Separator"),					KeyPair(VK_SUBTRACT,	"Subtract"),
KeyPair(VK_DECIMAL,		"Decimal"),						KeyPair(VK_DIVIDE,		"Divide"),
KeyPair(VK_F1,			"F1"),							KeyPair(VK_F2,			"F2"),
KeyPair(VK_F3,			"F3"),							KeyPair(VK_F4,			"F4"),
KeyPair(VK_F5,			"F5"),							KeyPair(VK_F6,			"F6"),
KeyPair(VK_F7,			"F7"),							KeyPair(VK_F8,			"F8"),
KeyPair(VK_F9,			"F9"),							KeyPair(VK_F10,			"F10"),
KeyPair(VK_F11,			"F11"),							KeyPair(VK_F12,			"F12"),
KeyPair(VK_F13,			"F13"),							KeyPair(VK_F14,			"F14"),
KeyPair(VK_F15,			"F15"),							KeyPair(VK_F16,			"F16"),
KeyPair(VK_F17,			"F17"),							KeyPair(VK_F18,			"F18"),
KeyPair(VK_F19,			"F19"),							KeyPair(VK_F20,			"F20"),
KeyPair(VK_F21,			"F21"),							KeyPair(VK_F22,			"F22"),
KeyPair(VK_F23,			"F23"),							KeyPair(VK_F24,			"F24"),
KeyPair(VK_NUMLOCK,		"Num lock"),					KeyPair(VK_SCROLL,		"Scroll lock"),
KeyPair(VK_LSHIFT,		"Left shift"),					KeyPair(VK_RSHIFT,		"Right shift"),
KeyPair(VK_LCONTROL,	"Left control"),				KeyPair(VK_RCONTROL,	"Right control"),
KeyPair(VK_LMENU,		"Left alt"),					KeyPair(VK_RMENU,		"Right alt"),
KeyPair(VK_OEM_1,		";"),							KeyPair(VK_OEM_PLUS,	"="),
KeyPair(VK_OEM_COMMA,	","),							KeyPair(VK_OEM_MINUS,	"-"),
KeyPair(VK_OEM_PERIOD,	"."),							KeyPair(VK_OEM_2,		"/"),
KeyPair(VK_OEM_3,		"`"),							KeyPair(VK_OEM_4,		"["),
KeyPair(VK_OEM_5,		"\\"),							KeyPair(VK_OEM_6,		"]"),
KeyPair(VK_OEM_7,		"'"),							KeyPair(VK_OEM_8,		"VK_OEM_8")
	};
/* functions */
private:
	void										FirstThreadFunc();
	void										SecondThreadFunc();
	void										Clear();
public:
												Input();
												~Input();
	void										Reload();
	template<typename... Keys, typename = KEY>
	void										AddNewCombination(const std::function<void(Input::KeyCombination::state&)>, Keys ...);
	void										AddNewCombination(const std::vector<KEY>, const std::function<void(Input::KeyCombination::state&)>);
	std::string									GetValueName(const KEY) const;
};
#include "Input.cpp"
#undef KeyPair