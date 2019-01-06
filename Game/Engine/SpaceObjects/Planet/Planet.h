#pragma once
#include "../Star system/StarSystem.h"
#include <vector>
class Planet : public SpaceObject {
public: // structures
	struct TectonicalPlate {
		struct Coord {
			Angle zenith;
			Angle azimuth;
			uint8_t level;
		};
		std::vector<Coord> coords;
	};
	struct {
		Mass::Planet MP;
		Distance::Meters Radius;
		std::vector<TectonicalPlate> Plates;
	} PInfo;
public:
	Planet(uint8_t *parent, int64_t x, int64_t y, int64_t z);
};
#include "Planet.cpp"