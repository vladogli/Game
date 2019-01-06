#pragma once
#include "CalcSeed/CalcSeed.h"
#include "Units/Units.h"
#include <stdint.h>
typedef float Angle;
class SpaceObject {
protected:
	uint8_t *seed;
	long double drand(long double dMin, long double dMax, uint64_t number);
public:
	SpaceObject(uint8_t *, int64_t, int64_t, int64_t);
	~SpaceObject();
};
#include "Object.cpp"