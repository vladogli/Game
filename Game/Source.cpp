#include <iostream> 
#include <thread>
#include "Engine/VirtualMachine/core.h"
int main(void)
{
	std::this_thread::sleep_for(std::chrono::hours(60));
	return 0;
}
#undef RELEASED

