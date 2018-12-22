#pragma once
#include <boost/thread.hpp>
#define u32 unsigned int
#define ERROR_VAL u32
#define GRAPHICS_ERROR_NOT_INITIALIZED 0x0001FFFF
class Graphics {
	boost::thread renderThread;
	ERROR_VAL lastError = GRAPHICS_ERROR_NOT_INITIALIZED;
	void			SetLastError(const ERROR_VAL);
public:
	ERROR_VAL		GetLastError() const;
	Graphics();
};
#include "Graphics.cpp"
#undef u32
#undef ERROR_VAL