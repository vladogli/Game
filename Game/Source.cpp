#include <iostream> 
#include <thread>
#include "Engine/Engine.h"
#include <Windows.h>
#include <sstream>
#include "Engine/SpaceObjects/Object.h"
#include "Engine/SpaceObjects/Planet/Planet.h"
int main() {
	uint8_t *ptr;
	CalculateSeed::Calculate(ptr, "hello world");
	Planet p(ptr,1,1,1);
	std::this_thread::sleep_for(std::chrono::seconds(500));
	return 0;
}
#undef RELEASED
