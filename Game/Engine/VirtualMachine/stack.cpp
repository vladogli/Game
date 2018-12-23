#include "Memory.h"
void Memory::Write(const ADDR addr, BYTE*& src, const SIZE size) {
	if (addr > memSize) {
		throw WRONG_ADDRESS;
		return;
	}
	if (addr + size > memSize) {
		throw TOO_BIG_SIZE;
		return;
	}
	memcpy(memory + addr, src, size);
}
void Memory::Fill(const ADDR addr, const BYTE byte, const SIZE size) {
	if (addr > memSize) {
		throw WRONG_ADDRESS;
		return;
	}
	if (addr + size > memSize) {
		throw TOO_BIG_SIZE;
		return;
	}
	memset(memory + addr, byte, size);
}
void Memory::Clear(const ADDR addr, const SIZE size) {
	Fill(addr, size, 0);
}
void Memory::Read(const ADDR addr, BYTE*& dest, const SIZE size) const {
	if (addr > memSize) {
		throw WRONG_ADDRESS;
		return;
	}
	if (addr + size > memSize) {
		throw TOO_BIG_SIZE;
		return;
	}
	memcpy(dest, memory + addr, size);
}
const SIZE& Memory::MaxMemorySize() const {
	return memSize;
}
Memory::Memory(SIZE size) {
	memory = new BYTE[size];
	memSize = size;
}
Memory::~Memory() {
	delete[] memory;
}