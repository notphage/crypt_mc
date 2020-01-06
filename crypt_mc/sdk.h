#pragma once

#include <cmath>
#include <cfloat>
#include <algorithm>
#include <vector>
#include <memory>
#include <functional>
#include <tuple>

#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#define M_PI_F		((float)(M_PI))	// Shouldn't collide with anything.

// NJS: Inlined to prevent floats from being autopromoted to doubles, as with the old system.
#ifndef RAD2DEG
#define RAD2DEG( x )  ( (float)(x) * (float)(180.f / M_PI_F) )
#endif

#ifndef DEG2RAD
#define DEG2RAD( x )  ( (float)(x) * (float)(M_PI_F / 180.f) )
#endif

class vec2
{
public:
	vec2()
	{
		x = y = 0.0f;
	}

	template <typename t>
	vec2(t X, t Y)
	{
		static_assert(std::is_arithmetic<t>::value, "Arithmetic not possible on this type");
		x = (float)X;
		y = (float)Y;
	}

	template <typename t>
	vec2(const std::initializer_list<t>& list)
	{
		static_assert(std::is_arithmetic<t>::value, "Arithmetic not possible on this type");

		if (list.size() == 2)
		{
			auto it = list.begin();
			x = (float)* it++;
			y = (float)* it;
		}
	}

	vec2(float* v)
	{
		x = v[0];
		y = v[1];
	}

	vec2(const float* v)
	{
		x = v[0];
		y = v[1];
	}

	vec2(const vec2& v)
	{
		x = v.x;
		y = v.y;
	}

	vec2& operator=(const vec2& v)
	{
		x = v.x;
		y = v.y;
		return *this;
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	vec2& operator+=(const vec2& v)
	{
		x += v.x;
		y += v.y;
		return *this;
	}

	vec2& operator-=(const vec2& v)
	{
		x -= v.x;
		y -= v.y;
		return *this;
	}

	vec2& operator*=(const vec2& v)
	{
		x *= v.x;
		y *= v.y;
		return *this;
	}

	vec2& operator/=(const vec2& v)
	{
		x /= v.x;
		y /= v.y;
		return *this;
	}

	vec2& operator+=(float v)
	{
		x += v;
		y += v;
		return *this;
	}

	vec2& operator-=(float v)
	{
		x -= v;
		y -= v;
		return *this;
	}

	vec2& operator*=(float v)
	{
		x *= v;
		y *= v;
		return *this;
	}

	vec2& operator/=(float v)
	{
		x /= v;
		y /= v;
		return *this;
	}

	vec2 operator+(const vec2& v) const
	{
		return vec2(x + v.x, y + v.y);
	}

	vec2 operator-(const vec2& v) const
	{
		return vec2(x - v.x, y - v.y);
	}

	vec2 operator*(const vec2& v) const
	{
		return vec2(x * v.x, y * v.y);
	}

	vec2 operator/(const vec2& v) const
	{
		return vec2(x / v.x, y / v.y);
	}

	vec2 operator+(float v) const
	{
		return vec2(x + v, y + v);
	}

	vec2 operator-(float v) const
	{
		return vec2(x - v, y - v);
	}

	vec2 operator*(float v) const
	{
		return vec2(x * v, y * v);
	}

	vec2 operator/(float v) const
	{
		return vec2(x / v, y / v);
	}

	void set(float X = 0.0f, float Y = 0.0f)
	{
		x = X;
		y = Y;
	}

	float length(void) const
	{
		return sqrtf(x * x + y * y);
	}

	float lengthsqr(void) const
	{
		return (x * x + y * y);
	}

	float dist_to(const vec2& v) const
	{
		return (*this - v).length();
	}

	float dist_to_sqr(const vec2& v) const
	{
		return (*this - v).lengthsqr();
	}

	float dot(const vec2& v) const
	{
		return (x * v.x + y * v.y);
	}

	bool is_zero(void) const
	{
		return (x > -FLT_EPSILON && x < FLT_EPSILON &&
			y > -FLT_EPSILON && y < FLT_EPSILON);
	}

	vec2 normalized()
	{
		return (*this) /= length();
	}

	operator bool() const noexcept
	{
		return !is_zero();
	}

	float x, y;
};

class vec3
{
public:
	vec3()
	{
		x = y = z = 0.0f;
	}

	template <typename t>
	vec3(t X, t Y, t Z)
	{
		static_assert(std::is_arithmetic<t>::value, "Arithmetic not possible on this type");
		x = (float)X;
		y = (float)Y;
		z = (float)Z;
	}

	template <typename t>
	vec3(const std::initializer_list<t>& list)
	{
		static_assert(std::is_arithmetic<t>::value, "Arithmetic not possible on this type");

		if (list.size() == 3)
		{
			auto it = list.begin();
			x = (float)* it++;
			y = (float)* it++;
			z = (float)* it;
		}
	}

	vec3(float* v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

	vec3(const float* v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
	}

	vec3(const vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
	}

	vec3(const vec2& v)
	{
		x = v.x;
		y = v.y;
		z = 0.0f;
	}

	vec3& operator=(const vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		return *this;
	}

	vec3& operator=(const vec2& v)
	{
		x = v.x;
		y = v.y;
		z = 0.0f;
		return *this;
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	vec3& operator+=(const vec3& v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	vec3& operator-=(const vec3& v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	vec3& operator*=(const vec3& v)
	{
		x *= v.x;
		y *= v.y;
		z *= v.z;
		return *this;
	}

	vec3& operator/=(const vec3& v)
	{
		x /= v.x;
		y /= v.y;
		z /= v.z;
		return *this;
	}

	vec3& operator+=(float v)
	{
		x += v;
		y += v;
		z += v;
		return *this;
	}

	vec3& operator-=(float v)
	{
		x -= v;
		y -= v;
		z -= v;
		return *this;
	}

	vec3& operator*=(float v)
	{
		x *= v;
		y *= v;
		z *= v;
		return *this;
	}

	vec3& operator/=(float v)
	{
		x /= v;
		y /= v;
		z /= v;
		return *this;
	}

	vec3 operator+(const vec3& v) const
	{
		return vec3(x + v.x, y + v.y, z + v.z);
	}

	vec3 operator-(const vec3& v) const
	{
		return vec3(x - v.x, y - v.y, z - v.z);
	}

	vec3 operator*(const vec3& v) const
	{
		return vec3(x * v.x, y * v.y, z * v.z);
	}

	vec3 operator/(const vec3& v) const
	{
		return vec3(x / v.x, y / v.y, z / v.z);
	}

	vec3 operator+(float v) const
	{
		return vec3(x + v, y + v, z + v);
	}

	vec3 operator-(float v) const
	{
		return vec3(x - v, y - v, z - v);
	}

	vec3 operator*(float v) const
	{
		return vec3(x * v, y * v, z * v);
	}

	vec3 operator/(float v) const
	{
		return vec3(x / v, y / v, z / v);
	}

	bool operator==(const vec3& other) const
	{
		return x == other.x && y == other.y && other.z == other.z;
	}

	bool operator!=(const vec3& other) const
	{
		return !(*this == other);
	}

	bool operator!() const
	{
		return this->is_zero();
	}

	void set(float X = 0.0f, float Y = 0.0f, float Z = 0.0f)
	{
		x = X;
		y = Y;
		z = Z;
	}

	vec3 center() const
	{
		return *this * 0.5f;
	}

	vec3 to_angles() const
	{
		const vec3 forward = *this;
		return vec3(RAD2DEG(std::atan2(-forward.z, std::sqrt(forward.x * forward.x + forward.y * forward.y))),
			RAD2DEG(std::atan2(forward.y, forward.x)),
			0.0f);
	}

	float length() const
	{
		return std::sqrt(x * x + y * y + z * z);
	}

	float length_sqr() const
	{
		return (x * x + y * y + z * z);
	}

	float length2d() const
	{
		return std::sqrt(x * x + y * y);
	}

	float length2d_sqr() const
	{
		return (x * x + y * y);
	}

	float dist(const vec3& v) const
	{
		return (*this - v).length();
	}

	float dist_sqr(const vec3& v) const
	{
		return (*this - v).length_sqr();
	}

	float dot(const vec3& v) const
	{
		return (x * v.x + y * v.y + z * v.z);
	}

	float dot_2d(const vec3& v) const
	{
		return (x * v.x + y * v.y);
	}

	vec3 cross_product(const vec3& v) const
	{
		return vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
	}

	bool is_zero() const
	{
		return (x > -0.01f && x < 0.01f &&
			y > -0.01f && y < 0.01f &&
			z > -0.01f && z < 0.01f);
	}

	bool is_nan() const
	{
		return isnan(x) || isnan(y) || isnan(z);
	}

	void normalize()
	{
		auto& vec = *this;

		// Proper large epenis normalization, none of that gay while loop shit
		for (auto i = 0; i <= 2; i++)
			if (vec[i] > 180.f || vec[i] < -180.f)
				vec[i] = (vec[i] < 0.f) ? vec[i] + 360.f * round(abs(vec[i] / 360.f)) : vec[i] - 360.f * round(abs(vec[i] / 360.f));
	}

	vec3 normalized() const
	{
		auto vec = *this;
		vec.normalize();
		return vec;
	}

	//normalized
	float yeetus_that_feetus() const
	{
		vec3 res = *this;
		float l = res.length();
		if (l != 0.0f)
		{
			res /= l;
		}
		else
		{
			res.x = res.y = res.z = 0.0f;
		}
		return l;
	}

	void clamp()
	{
		x = std::clamp<float>(x, -89.f, 89.f);
		y = std::clamp<float>(y, -180.0f, 180.0f);
		z = 0.0f;
	}

	vec3 clamped()
	{
		return vec3(
			std::clamp<float>(x, -89.f, 89.f),
			std::clamp<float>(y, -180.0f, 180.0f),
			0.0f
		);
	}

	void sanitize()
	{
		//safety
		if (!isfinite(x) || isnan(x) || isinf(x))
			x = 0.0f;

		if (!isfinite(y) || isnan(y) || isinf(y))
			y = 0.0f;

		//normalize
		normalize();

		//clamp
		clamp();
	}

	void normalize_in_place()
	{
		*this *= 1.f / (this->length() + FLT_EPSILON);
	}

	bool is_zero()
	{
		return x == 0.0f && y == 0.0f && z == 0.0f;
	}

	float x, y, z;
};

class vec4
{
public:
	vec4()
	{
		x = y = z = w = 0.f;
	}

	template <typename t>
	vec4(t X, t Y, t Z, t W)
	{
		static_assert(std::is_arithmetic<t>::value, "Arithmetic not possible on this type");
		x = (float)X;
		y = (float)Y;
		z = (float)Z;
		w = (float)W;
	}

	template <typename t>
	vec4(const std::initializer_list<t>& list)
	{
		static_assert(std::is_arithmetic<t>::value, "Arithmetic not possible on this type");

		if (list.size() == 4)
		{
			auto it = list.begin();
			x = (float)* it++;
			y = (float)* it++;
			z = (float)* it++;
			w = (float)* it;
		}
	}

	vec4(float* v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
		w = v[3];
	}

	vec4(const float* v)
	{
		x = v[0];
		y = v[1];
		z = v[2];
		w = v[3];
	}

	vec4(const vec4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
	}

	vec4(const vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = 0.f;
	}

	vec4(const vec2& v)
	{
		x = v.x;
		y = v.y;
		z = 0.f;
		w = 0.f;
	}

	vec4& operator=(const vec4& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = v.w;
		return *this;
	}

	vec4& operator=(const vec3& v)
	{
		x = v.x;
		y = v.y;
		z = v.z;
		w = 0.f;
		return *this;
	}

	vec4& operator=(const vec2& v)
	{
		x = v.x;
		y = v.y;
		z = 0.f;
		w = 0.f;
		return *this;
	}

	bool operator==(const vec4& other) const
	{
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool operator!=(const vec4& other) const
	{
		return !(*this == other);
	}

	float& operator[](int i)
	{
		return ((float*)this)[i];
	}

	float operator[](int i) const
	{
		return ((float*)this)[i];
	}

	float x, y, z, w;
};

class ColorF
{
	float R, G, B, A;
public:
	constexpr ColorF()
		: R(0.f), G(0.f), B(0.f), A(0.f) { }

	constexpr ColorF(float r, float g, float b, float a)
		: R(r), G(g), B(b), A(a) { }

	constexpr ColorF(float r, float g, float b)
		: R(r), G(g), B(b), A(1.f) { }

	float r() const
	{
		return R;
	}

	float g() const
	{
		return G;
	}

	float b() const
	{
		return B;
	}

	float a() const
	{
		return A;
	}

	void r(float _r)
	{
		R = _r;
	}

	void g(float _g)
	{
		G = _g;
	}

	void b(float _b)
	{
		B = _b;
	}

	void a(float _a)
	{
		A = _a;
	}

	const float* raw() const
	{
		return &R;
	}
};

class color_t
{
	uint8_t R, G, B, A;
public:
	constexpr color_t()
		: R(0), G(0), B(0), A(0) { }

	constexpr color_t(int r, int g, int b, int a)
		: R(r), G(g), B(b), A(a) { }

	constexpr color_t(int r, int g, int b)
		: R(r), G(g), B(b), A(255) { }

	int r() const
	{
		return R;
	}

	int g() const
	{
		return G;
	}

	int b() const
	{
		return B;
	}

	int a() const
	{
		return A;
	}

	void r(int _r)
	{
		R = _r;
	}

	void g(int _g)
	{
		G = _g;
	}

	void b(int _b)
	{
		B = _b;
	}

	void a(int _a)
	{
		A = _a;
	}

	uint8_t* r_ptr()
	{
		return &R;
	}

	uint8_t* g_ptr()
	{
		return &G;
	}

	uint8_t* b_ptr()
	{
		return &B;
	}

	uint8_t* a_ptr()
	{
		return &A;
	}

	color_t& operator =(const color_t& c)
	{
		R = c.r();
		G = c.g();
		B = c.b();
		A = c.a();
		return *this;
	}

	color_t operator+(const color_t& v) const
	{
		return color_t(R + v.R, G + v.G, B + v.B, A + v.A);
	}

	explicit operator bool() const noexcept
	{
		return R > 0 || G > 0 || B > 0 || A > 0;
	}

	bool operator==(const color_t& c) const
	{
		return R == c.r() && G == c.g() && B == c.b();
	}

	static color_t from_hsb(float hue, float saturation, float brightness)
	{
		float h = hue == 1.0f ? 0 : hue * 6.0f;
		float f = h - (int)h;
		float p = brightness * (1.0f - saturation);
		float q = brightness * (1.0f - saturation * f);
		float t = brightness * (1.0f - saturation * (1.0f - f));

		if (h < 1)
		{
			return color_t(
			               (unsigned char)(brightness * 255),
			               (unsigned char)(t * 255),
			               (unsigned char)(p * 255)
			              );
		}
		if (h < 2)
		{
			return color_t(
			               (unsigned char)(q * 255),
			               (unsigned char)(brightness * 255),
			               (unsigned char)(p * 255)
			              );
		}
		if (h < 3)
		{
			return color_t(
			               (unsigned char)(p * 255),
			               (unsigned char)(brightness * 255),
			               (unsigned char)(t * 255)
			              );
		}
		if (h < 4)
		{
			return color_t(
			               (unsigned char)(p * 255),
			               (unsigned char)(q * 255),
			               (unsigned char)(brightness * 255)
			              );
		}
		if (h < 5)
		{
			return color_t(
			               (unsigned char)(t * 255),
			               (unsigned char)(p * 255),
			               (unsigned char)(brightness * 255)
			              );
		}
		return color_t(
		               (unsigned char)(brightness * 255),
		               (unsigned char)(p * 255),
		               (unsigned char)(q * 255)
		              );
	}

	constexpr color_t& from_hsv(float h, float s, float v)
	{
		float colOut[3]{ };
		if (s == 0.0f)
		{
			R = G = B = static_cast<int>(v * 255);
			return *this;
		}

		h = fmodf(h, 1.0f) / (60.0f / 360.0f);
		int   i = static_cast<int>(h);
		float f = h - static_cast<float>(i);
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));

		switch (i)
		{
		case 0:
			colOut[0] = v;
			colOut[1] = t;
			colOut[2] = p;
			break;
		case 1:
			colOut[0] = q;
			colOut[1] = v;
			colOut[2] = p;
			break;
		case 2:
			colOut[0] = p;
			colOut[1] = v;
			colOut[2] = t;
			break;
		case 3:
			colOut[0] = p;
			colOut[1] = q;
			colOut[2] = v;
			break;
		case 4:
			colOut[0] = t;
			colOut[1] = p;
			colOut[2] = v;
			break;
		case 5: default:
			colOut[0] = v;
			colOut[1] = p;
			colOut[2] = q;
			break;
		}

		R = static_cast<int>(colOut[0] * 255);
		G = static_cast<int>(colOut[1] * 255);
		B = static_cast<int>(colOut[2] * 255);
		return *this;
	}

	/*fColor ToFColor( ) const
	{
	return fColor( R / 255.0f, G / 255.0f, B / 255.0f, A / 255.0f );
	}*/

	ColorF base() const
	{
		//returned address of temp value before :>
		return ColorF(R / 255.0f, G / 255.0f, B / 255.0f, A / 255.f);
	}

	color_t __base()
	{
		return color_t(R / 255.0f, G / 255.0f, B / 255.0f);
	}

	static color_t blend(const color_t& first, const color_t& second, float t)
	{
		return color_t(
		               first.r() + static_cast<int>(t * (second.r() - first.r())),
		               first.g() + static_cast<int>(t * (second.g() - first.g())),
		               first.b() + static_cast<int>(t * (second.b() - first.b())),
		               first.a() + static_cast<int>(t * (second.a() - first.a()))
		              );
	}

	float to_hue()
	{
		typedef struct
		{
			float h, s, v;
		} hsv;
		hsv out;

		float min = static_cast<float>(R < G ? R : G);
		min = static_cast<float>(min < B ? min : B);

		float max = static_cast<float>(R > G ? R : G);
		max = static_cast<float>(max > B ? max : B);

		out.v = max;
		float delta = max - min;
		if (delta < 0.0010f)
		{
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if (max > 0.0f)
		{
			out.s = delta / max;
		}
		else
		{
			out.s = 0.0f;
			out.h = NAN;
			return out.h;
		}
		if (R >= max)
			out.h = static_cast<float>(G - B) / delta;
		else if (G >= max)
			out.h = 2.0f + static_cast<float>(B - R) / delta;
		else
			out.h = 4.0f + static_cast<float>(R - G) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if (out.h < 0.0f)
			out.h += 360.0f;

		return out.h;
	}

	float brightness()
	{
		typedef struct
		{
			float h, s, v;
		} hsv;
		hsv out;

		float min = static_cast<float>(R < G ? R : G);
		min = static_cast<float>(min < B ? min : B);

		float max = static_cast<float>(R > G ? R : G);
		max = static_cast<float>(max > B ? max : B);

		out.v = max;
		float delta = max - min;
		if (delta < 0.0010f)
		{
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if (max > 0.0f)
		{
			out.s = delta / max;
		}
		else
		{
			out.s = 0.0f;
			out.h = NAN;
			return out.h;
		}
		if (R >= max)
			out.h = static_cast<float>(G - B) / delta;
		else if (G >= max)
			out.h = 2.0f + static_cast<float>(B - R) / delta;
		else
			out.h = 4.0f + static_cast<float>(R - G) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if (out.h < 0.0f)
			out.h += 360.0f;

		return out.v;
	}

	float saturation()
	{
		typedef struct
		{
			float h, s, v;
		} hsv;
		hsv out;

		float min = static_cast<float>(R < G ? R : G);
		min = static_cast<float>(min < B ? min : B);

		float max = static_cast<float>(R > G ? R : G);
		max = static_cast<float>(max > B ? max : B);

		out.v = max;
		float delta = max - min;
		if (delta < 0.0010f)
		{
			out.s = 0.f;
			out.h = 0.f;
			return out.h;
		}
		if (max > 0.0f)
		{
			out.s = delta / max;
		}
		else
		{
			out.s = 0.0f;
			out.h = NAN;
			return out.h;
		}
		if (R >= max)
			out.h = static_cast<float>(G - B) / delta;
		else if (G >= max)
			out.h = 2.0f + static_cast<float>(B - R) / delta;
		else
			out.h = 4.0f + static_cast<float>(R - G) / delta;

		out.h *= 60.0f;
		out.h /= 360.f;

		if (out.h < 0.0f)
			out.h += 360.0f;

		return out.s;
	}

	void ScaleColor(float scaleR, float scaleG, float scaleB)
	{
		R = static_cast<int>(scaleR * R);
		G = static_cast<int>(scaleG * G);
		B = static_cast<int>(scaleB * B);
	}

	void scale_alpha(float scaleA)
	{
		A = static_cast<int>(scaleA * A);
	}

	void from_abgr(uint32_t abgr)
	{
		uint8_t* vp = (uint8_t*)& abgr;

		A = vp[0];
		B = vp[1];
		G = vp[2];
		R = vp[3];
	}

	uint32_t abgr() const
	{
		uint32_t abgr = (A << 24) | (B << 16) | (G << 8) | (R);
		return abgr;
	}

	color_t from_rgba(uint32_t rgba)
	{
		uint8_t* vp = (uint8_t*)&rgba;

		return color_t{ vp[0], vp[1], vp[2], vp[3] };
	}			
		
	uint32_t rgba() const
	{
		uint32_t rgba = (R << 24) | (G << 16) | (B << 8) | (A);
		return rgba;
	}

	uint32_t argb() const
	{
		uint32_t argb = (A << 24) | (R << 16) | (G << 8) | (B);
		return argb;
	}

#define def_color(name, r, g, b, a) \
	static color_t name() { return color_t(r, g, b, a); }

	def_color(black, 0, 0, 0, 255);
	def_color(navy, 0, 0, 128, 255);
	def_color(blue, 0, 0, 255, 255);
	def_color(green, 0, 128, 0, 255);
	def_color(teal, 0, 128, 128, 255);
	def_color(lime, 0, 255, 0, 255);
	def_color(aqua, 0, 255, 255, 255);
	def_color(maroon, 128, 0, 0, 255);
	def_color(purple, 128, 0, 128, 255);
	def_color(olive, 128, 128, 0, 255);
	def_color(gray, 128, 128, 128, 255);
	def_color(silver, 192, 192, 192, 255);
	def_color(red, 255, 0, 0, 255);
	def_color(fuchsia, 255, 0, 255, 255);
	def_color(yellow, 255, 255, 0, 255);
	def_color(white, 255, 255, 255, 255);

#undef def_color
};

struct matrix4x4_t
{
	float m[4][4];

	inline float* operator[](int i)
	{
		return m[i];
	}

	inline const float* operator[](int i) const
	{
		return m[i];
	}
};

struct matrix3x4_t
{
	float m[3][4];

	inline float* operator[](int i)
	{
		return m[i];
	}

	inline const float* operator[](int i) const
	{
		return m[i];
	}
};

enum client_flavor
{
	VANILLA,
	FORGE,
	BADLION,
	HYPERIUM
};

enum find_item_type
{
	POTION,
	SOUP,
	PEARL,
	ROD
};

class c_block
{
protected:
	JNIEnv* jni;

	jint id = 0;
	std::string name{};
public:
	jobject block_instance = nullptr;

	virtual void instantiate(jobject, JNIEnv*) = 0;

	__forceinline jint get_id()
	{
		return id;
	}

	__forceinline std::string_view get_name()
	{
		return name;
	}
};

class c_class_loader
{
protected:
	JNIEnv* jni;

public:
	virtual void instantiate(JNIEnv*) = 0;

	virtual jclass find_class(const char*) = 0;
};

class c_player
{
protected:
	JNIEnv* jni;

public:
	~c_player()
	{
		jni->DeleteLocalRef(player_instance);
	}

	jobject player_instance = nullptr;

	virtual void instantiate(jobject, JNIEnv*) = 0;

	virtual bool is_same(const std::shared_ptr<c_player>&) = 0;
	virtual jboolean is_invisible() = 0;
	virtual jboolean is_sneaking() = 0;
    virtual jboolean is_potion_active(jint) = 0;

	virtual jfloat get_pitch() = 0;
	virtual jfloat get_prev_pitch() = 0;
	virtual jfloat get_yaw() = 0;
	virtual jfloat get_prev_yaw() = 0;

	virtual jfloat get_camera_pitch() = 0;
	virtual jfloat get_prev_camera_pitch() = 0;
	virtual jfloat get_camera_yaw() = 0;
	virtual jfloat get_prev_camera_yaw() = 0;

	virtual jfloat get_strafing() = 0;
	virtual jfloat get_forward() = 0;
	virtual jobject get_held_item() = 0;
	virtual jobject get_held_item_stack() = 0;
	virtual jstring get_item_name(jobject) = 0;
	virtual jboolean holding_item() = 0;
	virtual jboolean holding_weapon() = 0;
	virtual jboolean holding_sword() = 0;
	virtual jboolean holding_axe() = 0;
	virtual jboolean holding_pick_axe() = 0;
	virtual jboolean holding_hoe() = 0;
	virtual jboolean holding_shovel() = 0;
	virtual jboolean holding_projectile() = 0;
	virtual jboolean holding_block() = 0;
	virtual jboolean holding_fishing_rod() = 0;

	virtual jdouble origin_x() = 0;
	virtual jdouble origin_y() = 0;
	virtual jdouble origin_z() = 0;

	virtual jdouble motion_x() = 0;
	virtual jdouble motion_y() = 0;
	virtual jdouble motion_z() = 0;

	virtual jdouble aabb_min_x() = 0;
	virtual jdouble aabb_min_y() = 0;
	virtual jdouble aabb_min_z() = 0;
	virtual jdouble aabb_max_x() = 0;
	virtual jdouble aabb_max_y() = 0;
	virtual jdouble aabb_max_z() = 0;

	virtual jdouble old_origin_x() = 0;
	virtual jdouble old_origin_y() = 0;
	virtual jdouble old_origin_z() = 0;

	virtual jfloat get_distance_walked() = 0;
	virtual jfloat get_prev_distance_walked() = 0;

	virtual jboolean is_dead() = 0;
    virtual jboolean is_collided_vertically() = 0;
    virtual jboolean is_collided_horizontally() = 0;
	virtual jboolean is_on_ground() = 0;
    virtual jboolean is_sprinting() = 0;
	virtual jboolean is_visible(jobject) = 0;
	virtual jboolean in_water() = 0;

	virtual jfloat get_width() = 0;
	virtual jfloat get_height() = 0;
    virtual jfloat get_eye_height() = 0;
    
    virtual jfloat get_health() = 0;
	virtual jfloat get_max_health() = 0;
	virtual jboolean has_armor() = 0;

	virtual jstring get_name() = 0;

	virtual jint get_hurt_time() = 0;
    virtual jint get_speed_amplifier() = 0;
	virtual jint get_max_hurt_time() = 0;

	virtual jobject get_stack(jint) = 0;
	virtual jobject get_item(jobject) = 0;
	virtual jobject get_effects(jobject, jobject) = 0;
	virtual jint get_effects_id(jobject) = 0;
	virtual jint get_potion_id(jobject) = 0;
	virtual jint get_current_slot() = 0;
	virtual jint get_ticks_existed() = 0;
	virtual std::vector<int> find_item(int min, int max, find_item_type item_type) = 0;

	virtual jboolean send_use_item(jobject) = 0;

	virtual void set_current_slot(jint) = 0;
	virtual void set_motion_x(jdouble) = 0;
	virtual void set_motion_y(jdouble) = 0;
    virtual void set_step_height(jfloat) = 0;
	virtual void set_motion_z(jdouble) = 0;
	virtual void set_pitch(jfloat) = 0;
	virtual void set_yaw(jfloat) = 0;
	virtual void set_velocity(jdouble, jdouble, jdouble) = 0;
	virtual void set_position(jdouble, jdouble, jdouble) = 0;
	virtual void set_angles(jfloat, jfloat) = 0;
	virtual void set_sneaking(jboolean) = 0;
    virtual void set_sprinting(jboolean) = 0;
};

class c_world
{
protected:
	JNIEnv* jni;

	jobject world_instance = nullptr;
public:
	~c_world()
	{
		jni->DeleteLocalRef(world_instance);
	}

	virtual void instantiate(jobject, JNIEnv*) = 0;
	virtual std::vector<std::shared_ptr<c_player>> get_players() = 0;
	virtual std::shared_ptr<c_block> get_block(jfloat, jfloat, jfloat) = 0;
};

class c_game
{
protected:
	JNIEnv* jni;
public:
	virtual void instantiate(JNIEnv*) = 0;

	virtual jboolean in_game_has_focus() = 0;
	virtual jboolean is_in_inventory() = 0;
	virtual jboolean is_view_bobbing() = 0;
	virtual jboolean is_in_chat() = 0;
	virtual jboolean is_hovering_block() = 0;

	virtual void set_view_bobbing(jboolean) = 0;
	virtual void set_timer_speed(jfloat) = 0;
	virtual void set_right_click_delay(jint) = 0;
	virtual void set_pointed_entity(jobject ent) = 0;
	virtual void set_object_mouse_over(jobject moving, const vec3& hit_poisition) = 0;
	virtual void set_in_focus() = 0;
	virtual void set_mouse_grabbed(jboolean grabbed) = 0;
	virtual void set_not_in_focus() = 0;
	virtual void set_key_bind_state(jint key_code, jboolean state) = 0;

	virtual std::shared_ptr<c_player> get_player() = 0;
	virtual std::shared_ptr<c_world> get_world() = 0;

	virtual jfloat get_render_partial_ticks() = 0;
	virtual jfloat get_mouse_sensitivity() = 0;
	virtual jfloat get_timer_speed() = 0;
	virtual jfloat get_gamma() = 0;
	virtual jfloat get_fov(jfloat) = 0;

	virtual jint get_right_click_delay() = 0;
	virtual jint get_string_width(jstring) = 0;
	virtual jint draw_string_with_shadow(jstring, jint, jint, jint) = 0;
	virtual jint get_sneak_key_code() = 0;

	virtual jobject get_net_handler() = 0;
	virtual jobject get_player_controller() = 0;
	virtual jobject get_pointed_entity() = 0;
	virtual jobject get_mouse_over() = 0;
	virtual jobject get_screen() = 0;

	virtual void set_gamma(jfloat) = 0;
	virtual void enable_light_map() = 0;
	virtual void disable_light_map() = 0;
	virtual void setup_camera_transform(jfloat, int) = 0;

	virtual jlong get_window_handle() = 0;
	virtual jint get_screen_w() = 0;
	virtual jint get_screen_h() = 0;
	virtual void set_cursor_pos(jint, jint) = 0;
};

struct stack_trace
{
	bool valid = false;
	uint64_t method_name = 0;
	jint line_number = 0;
	std::string method_str{};
};

class c_stack_trace
{
	JNIEnv* jni;
public:
	void instantiate(JNIEnv*);
	void get_stack_trace(int, stack_trace&);
};
