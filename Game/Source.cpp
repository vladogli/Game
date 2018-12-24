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
void function(VirtualMachine& VM) {
	Input I;
	while (1) {
		for (int i = 0; i < 128; i++) {
			if (I.indicator[i] == Input::KeyCombination::HOLDING) {
				VM.ReceiveKey(I.KeyMassive[i].first);
				for (size_t j = 0; j< 10 && I.indicator[i]==Input::KeyCombination::HOLDING; j++) {
					std::this_thread::sleep_for(std::chrono::milliseconds(25));
				}
				while (I.indicator[i] == Input::KeyCombination::HOLDING) {
					std::this_thread::sleep_for(std::chrono::milliseconds(75));
					if (I.indicator[i] == Input::KeyCombination::HOLDING) {
						VM.ReceiveKey(I.KeyMassive[i].first);
					}
				}
			}
		}
	}
}
int main(void)
{
	VirtualMachine VM(0xABCDEF);
	VM.OpenConsole();
	boost::thread thread(function, std::ref(VM));
	auto matr = VM.GetMatrix();
	unsigned char** saved = new (unsigned char*[51]);
	for (size_t j = 0; j < 51; j++) {
		saved[j] = new unsigned char[80];
		for (int i = 0; i < 80; i++) {
			saved[j][i] = 0;
		}
	}
	while (1) {
		for (int i = 0; i < 80; i++) {
			for (int j = 0; j < 50; j++) {
				if (matr[j][i] != saved[j][i])
				{
					saved[j][i] = matr[j][i];
					gotoxy(i, j);
					std::cout << matr[j][i];
				}
			}
		}
	}
	std::this_thread::sleep_for(std::chrono::hours(60));
	return 0;
}
#undef RELEASED

