#include "CalcSeed.h"

static constexpr uint64_t PrimalCbrt[0x40]
{
	0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538,
	0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe,
	0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
	0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
	0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab,
	0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
	0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed,
	0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
	0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
	0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
	0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373,
	0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
	0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b
};

void CalcSeed(BYTE *&dest, BYTE *value, size_t size) {
	dest = new BYTE[SEED_SIZE];
	LCE lce;
	std::seed_seq seq((uint64_t*)value, (uint64_t*)value + (size / sizeof(uint64_t)));// Создаем seed_seq - это так называемая последовательность, нужна для формирования случайных чисел.
	lce.seed(seq);
	uint64_t *ptr = (uint64_t*)dest;// Превращаем указатель на байты в указатель на uint64_t, чтобы было проще работать.
	int pos = 0;
	while (pos+0x40 <= (SEED_SIZE / sizeof(uint64_t))) { // копируем в ptr набор чисел, а именно - корни первых 64 простых чисел (0x40 = 64)
		if ((SEED_SIZE / sizeof(uint64_t)) >= 0x40 + pos) {
			memcpy(ptr + pos, PrimalCbrt, 0x40);
		}
		else {
			memcpy(ptr + pos, PrimalCbrt, (0x40 + pos) - (SEED_SIZE / sizeof(uint64_t)));
		}
		pos += 0x40;
	}// если размер ptr больше 64 - заполняем полностью
	for (size_t j = 0; j < SEED_SIZE / sizeof(uint64_t); j++) {
		ptr[j] *= value[lce() % size]; // Умножаем каждый корень на случайное число
	}
	for (size_t j = 0; j < SEED_SIZE / sizeof(uint64_t); j++) {
		for (size_t i = 0; i < SEED_SIZE / sizeof(uint64_t); i++) {
			ptr[i] += (ptr[j] << (lce() % 16)); //
			ptr[i] -= (lce() << (lce() % 16));  // а тут просто проводим всякие операции с числами :/
		}
	}
}
void CalculateSeed::Calculate(BYTE *&dest, const std::string str) {
	BYTE *value = new BYTE[str.size()];
	memcpy(value, str.c_str(), str.size());
	CalcSeed(dest, value, str.size());
	delete[] value;
}
void CalculateSeed::Calculate(BYTE *&dest, BYTE *&parent, int64_t x, int64_t y, int64_t z) {
	BYTE *Value = new BYTE[SEED_SIZE + sizeof(int64_t)*SALT_SIZE];
	LCE lce;
	std::seed_seq seq{ x, y, z };
	lce.seed(seq);
	memcpy(Value, parent, SEED_SIZE);
	int itr = 0;
	for (int i = 0; i < sizeof(int64_t) * SALT_SIZE; i++, itr = 0) {
		for (char i = sizeof(int64_t); i >= 0; i--,itr++) {
			Value[i + SEED_SIZE] = (uint64_t)(lce() / std::pow(10, itr)) % 0x100;
		}
	}
	CalcSeed(dest, Value, SEED_SIZE + sizeof(int64_t)*SALT_SIZE);
	delete[] Value;
}
