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
void Memory::Write(const ADDR addr, const BYTE*& src, const SIZE size) {
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
void Memory::Write(const ADDR addr, BYTE byte) {
	if (addr > memSize) {
		throw WRONG_ADDRESS;
		return;
	}
	memory[addr] = byte;
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
	if (dest != nullptr) delete dest;
	dest = new BYTE[size];
	memcpy(dest, memory + addr, size);
}
BYTE Memory::Read(const ADDR addr) const {
	if (addr > memSize) {
		throw WRONG_ADDRESS;
		return -1;
	}
	return memory[addr];
}
Memory::Memory(SIZE size) : memSize(size) {
	memory = new BYTE[size];
}
Memory::~Memory() {
	delete[] memory;
}