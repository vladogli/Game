#pragma once
#include <random>
#include <string>
#define BYTE unsigned char
#define SEED_SIZE 0x400
#define SALT_SIZE 0x10
typedef std::linear_congruential_engine<uint64_t, 6364136223846793005
	, 1442695040888963407, 18446744073709551615> LCE;
namespace CalculateSeed {
	void Calculate(BYTE *&dest, const std::string);
	void Calculate(BYTE *&dest, BYTE *&parent,int64_t x,int64_t y, int64_t z);
}
#include "CalcSeed.cpp"
#undef BYTE
#undef SALT_SIZE