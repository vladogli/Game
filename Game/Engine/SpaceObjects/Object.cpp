#include "Object.h"
long double SpaceObject::drand(long double dMin, long double dMax, uint64_t number) {
	long double saveValue = dMin;
	if (dMin < 0)
	{
		dMax -= dMin;
		dMin -= dMin;
	}
	uint64_t from = uint64_t(10000000000000000 * dMin);
	uint64_t to = uint64_t(100000000000000000 * dMax);
	uint64_t returnValue = from + (number % ((to == 0) ? 1 : to));
	long double returnValue1 = (long double)(returnValue) / 100000000000000000;
	if (saveValue < 0)
		returnValue1 += saveValue;
	return returnValue1;
}
SpaceObject::SpaceObject(uint8_t *parent, int64_t x, int64_t y, int64_t z) {
	CalculateSeed::Calculate(seed, parent, x, y, z);
}
SpaceObject::~SpaceObject() {
	delete seed;
}