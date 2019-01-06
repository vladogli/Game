#pragma once
template <unsigned long long int _X, unsigned long long int _ToTheGrade>
class Unit {
private:
	long double _Value;
	long double _Mnojitel;
public:
	long double GetUnit() {
		return _Value / _Mnojitel;
	}
	void SetUnit(long double _V) {
		_Value = _V * _Mnojitel;
	}
	Unit() {
		_Mnojitel = std::pow(10, _ToTheGrade) * _X;
	}
	Unit(long double _V) : Unit() {
		_Value = _V;
	}
	operator long double() const {
		return _Value;
	}
};
namespace Mass {
	// in Solar masses
	typedef Unit<1989,27> Star;
	// in Earth masses
	typedef Unit<5972,21> Planet;
	// in kgs
	typedef Unit<1, 0> KGs;
};
namespace Distance {
	// in light seconds
	typedef Unit<299792458, 0> LightSeconds;
	// in light years
	typedef Unit<9454254955488000, 0> LightYears;
	// in meters
	typedef Unit<1, 0> Meters;
};
#include "Units.cpp"