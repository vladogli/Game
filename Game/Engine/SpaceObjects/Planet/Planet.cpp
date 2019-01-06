#include "Planet.h"
Planet::Planet(uint8_t *parent, int64_t x, int64_t y, int64_t z) :
	SpaceObject(parent, x, y, z) {
	LCE lce;
	std::seed_seq seq(seed, seed + SEED_SIZE);
	lce.seed(seq);
	PInfo.MP.SetUnit(drand(0.1,20,lce()));
	PInfo.Radius.SetUnit(drand(1000000, 10000000, lce()));

}