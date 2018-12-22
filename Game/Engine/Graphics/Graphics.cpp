#include "Graphics.h"
#define u32 unsigned int
#define ERROR_VAL u32
void Graphics::SetLastError(const ERROR_VAL _Value) {
	lastError = _Value;
}
ERROR_VAL Graphics::GetLastError() const {
	return lastError;
}
Graphics::Graphics() {

}
#undef u32
#undef ERROR_VAL
