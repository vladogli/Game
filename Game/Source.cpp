#include <iostream> 
#include <thread>
#include "Engine/VirtualMachine/core.h"
#include "Engine/Input.h"
#include <Windows.h>
#include <conio.h>
#include <boost/filesystem.hpp>
#include <boost/thread.hpp>
void gotoxy(int x, int y)
{
	COORD coord;
	coord.X = x;
	coord.Y = y;
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}
char getCursorChar()
{
	char c = '\0';
	CONSOLE_SCREEN_BUFFER_INFO con;
	HANDLE hcon = GetStdHandle(STD_OUTPUT_HANDLE);
	if (hcon != INVALID_HANDLE_VALUE &&
		GetConsoleScreenBufferInfo(hcon, &con))
	{
		DWORD read = 0;
		if (!ReadConsoleOutputCharacterA(hcon, &c, 1,
			con.dwCursorPosition, &read) || read != 1
			)
			c = '\0';
	}
	return c;
}

int main(void)
{
	VirtualMachine VM(0xABCDEF);
	VM.OpenConsole();
	Input I;
	while (1) {
		auto matr = VM.GetMatrix();
		for (int i = 0; i < 80; i++) {
			for (int j = 0; j < 50; j++) {
				if (matr[j][i] != 0)
				{
					gotoxy(i, j);
					char c = getCursorChar();
					if (c != matr[j][i]) {
						std::cout << matr[j][i];
					}
				}
			}
		}
		for (int i = 0; i < 128; i++) {
			if (I.indicator[i] == Input::KeyCombination::HOLDING)
			{
				VM.ReceiveKey(I.KeyMassive[i].first);
				while (I.indicator[i] == Input::KeyCombination::HOLDING)
					std::this_thread::sleep_for(std::chrono::milliseconds(25));
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::hours(60));
	return 0;
}
#undef RELEASED

