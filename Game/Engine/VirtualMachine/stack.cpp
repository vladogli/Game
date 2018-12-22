#include "stack.h"
void Stack::ClearMemory() {
	std::memset( memory, 0, memSize );
}
void Stack::ClearMemory(const ADDR from, const SIZE size) {
	std::memset( memory + from, 0, size );
}
SIZE Stack::ReadSize(ADDR addr)  const {
	addr -= 4;
	return memory[addr] +
		memory[addr + 1] * 256 +
		memory[addr + 2] * 256 * 256 +
		memory[addr + 3] * 256 * 256 * 256;
}
void Stack::WriteSize(const ADDR addr, SIZE size) {
	memory[addr] = size % 256;
	size /= 256;
	memory[addr + 1] = size % 256;
	size /= 256;
	memory[addr + 2] = size % 256;
	size /= 256;
	memory[addr + 3] = size % 256;
}
void VectorEmplace(std::vector<ADDR>& vector, ADDR Val, size_t itr) {
	std::vector<ADDR> _V;
	int i = 0;
	for (; i < itr; i++) {
		_V.push_back(vector[i]);
	}
	_V.push_back(Val);
	for (; i < vector.size(); i++) {
		_V.push_back(vector[i]);
	}
	vector = _V;
}
ADDR Stack::AllocateMemory(const SIZE size) {
	ADDR AllocAddr = 0;
	if (allocatedPtrs.size() != 0) {
		AllocAddr = allocatedPtrs[allocatedPtrs.size() - 1] + 
			ReadSize(allocatedPtrs[allocatedPtrs.size() - 1]);
	}
	for (size_t i = 1; i < allocatedPtrs.size(); i++) {	
		if (int(int(allocatedPtrs[i]) - int(allocatedPtrs[i-1]) - int(ReadSize(allocatedPtrs[i-1]))) >= int(size + 4) ) {
			AllocAddr = allocatedPtrs[i - 1] + ReadSize(allocatedPtrs[i - 1]);
			WriteSize(AllocAddr, size);
			VectorEmplace(allocatedPtrs, AllocAddr + 4, i);
			return AllocAddr + 4;
		}
	}
	if(AllocAddr + size > memSize) return -1;
	WriteSize( AllocAddr, size );
	allocatedPtrs.push_back( AllocAddr + 4 );
	return AllocAddr + 4;
}
ADDR Stack::AllocateMemory(const SIZE size, BYTE _Value) {
	ADDR returnValue = AllocateMemory(size);
	memset(memory + returnValue, _Value, size);
	return returnValue;
}
void Stack::Delete(const ADDR addr) {
	for (size_t i = 0; i < allocatedPtrs.size(); i++) {
		if (allocatedPtrs[i] == addr) {
			ClearMemory(addr - 4, ReadSize(addr)+4);
			allocatedPtrs.erase(allocatedPtrs.begin() + i);
		}
	}
}
bool Stack::Edit(const ADDR addr, SIZE size, BYTE*& source, SIZE offset) {
	if ( size + offset > ReadSize( addr ) ) return 0;
	memcpy(memory + addr + offset, source, size);
	return 1;
}
void Stack::Read(BYTE*& dest, const ADDR addr, SIZE from, SIZE to) const {
	if ( from > to || ReadSize(addr) < to ) return;
	if ( dest != NULL ) delete dest;
	dest = new BYTE[to - from];
	memcpy(dest, memory + addr + from, to - from);
}
void Stack::Read(BYTE*& dest, const ADDR addr) const {
	if ( dest != NULL ) delete dest;
	dest = new BYTE[ReadSize(addr)];
	memcpy(dest, memory + addr, ReadSize(addr) );
}
Stack::Stack(SIZE size) {
	memory = new BYTE[size];
	memSize = size;
}
Stack::~Stack() {
	delete[] memory;
}