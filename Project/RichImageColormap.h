#pragma once
#include <cassert>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <memory>

// вектор, содержащий цветовые каналы
struct vec4
{
	vec4(double a0, double a1, double a2, double a3)
		: x(a0)
		, y(a1)
		, z(a2)
		, w(a3)
	{
	}

	double operator[](size_t index) const
	{
		assert(index < 4);
		if (index < 2) {
			if (index < 1) {
				return x;
			} else {
				return y;
			}
		} else {
			if (index < 3) {
				return z;
			} else {
				return w;
			}
		}
	}

	double& operator[](size_t index)
	{
		assert(index < 4);
		if (index < 2) {
			if (index < 1) {
				return x;
			} else {
				return y;
			}
		} else {
			if (index < 3) {
				return z;
			} else {
				return w;
			}
		}
	}

	bool operator==(vec4 const& o) const
	{
		return x == o.x && y == o.y && z == o.z && w == o.w;
	}

	vec4 operator*(double v) const
	{
		return vec4(r * v, g * v, b * v, a * v);
	}

	vec4 operator+(vec4 const& v) const
	{
		return vec4(r + v.r, g + v.g, b + v.b, a + v.a);
	}

	std::string to_string() const
	{
		return
			std::string("{") +
			std::to_string(x) + std::string(",") +
			std::to_string(y) + std::string(",") +
			std::to_string(z) + std::string(",") +
			std::to_string(w) +
			std::string("}");
	}

	union {
		double r;
		double x;
	};
	union {
		double g;
		double y;
	};
	union {
		double b;
		double z;
	};
	union {
		double a;
		double w;
	};
};


// доступ к цветовым палитрам
class CRichImageColormap : public CObject
{
private:
		float clamp(float _X, float _Min, float _Max);

public:
	CRichImageColormap();
	virtual ~CRichImageColormap();

	vec4 GetColor_Autumn(float x);
	vec4 GetColor_Bone(float x);
	vec4 GetColor_Cool(float x);
	vec4 GetColor_Copper(float x);
	vec4 GetColor_Hot(float x);
	vec4 GetColor_HSV(float x);
	vec4 GetColor_Jet(float x);
	vec4 GetColor_Parula(float x);
	vec4 GetColor_Pink(float x);
	vec4 GetColor_Spring(float x);
	vec4 GetColor_Summer(float x);
	vec4 GetColor_Winter(float x);
};


