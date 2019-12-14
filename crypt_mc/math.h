#pragma once
#include <xmmintrin.h>
#include <cmath>

enum
{
	PITCH = 0, // up / down
	YAW, // left / right
	ROLL // fall over
};

namespace math
{
	static constexpr float pi = 3.14159265358979323846f;
	static constexpr float radpi = 57.295779513082f;
	static constexpr float pirad = 0.01745329251f;

	// sorry just in case u want to not be retarded
	static constexpr float deg2rad(const float& x)
	{
		return x * pirad;
	}

	static constexpr float rad2deg(const float& x)
	{
		return x * radpi;
	}

	inline void sin_cos(float radians, float* sine, float* cosine)
	{
		*sine = sin(radians);
		*cosine = cos(radians);
	}

	__forceinline float normalize(float angle)
	{
		auto revolutions = angle / 360.f;
		if (angle > 180.f || angle < -180.f)
		{
			revolutions = round(abs(revolutions));
			if (angle < 0.f)
				angle = (angle + 360.f * revolutions);
			else
				angle = (angle - 360.f * revolutions);
			return angle;
		}
		return angle;
	}

	__forceinline float wrap_angle_to_180(float angle)
	{
		angle = fmodf(angle, 360.0f);

		if (angle >= 180.0f)
			angle -= 360.0f;

		if (angle < -180.0f)
			angle += 360.0f;

		return angle;
	}

	template <typename T>
	__forceinline T max(T a, T b)
	{
		return (a < b) ? b : a;
	}

	template <typename T>
	__forceinline T min(T a, T b)
	{
		return (a > b) ? b : a;
	}

	__forceinline vec3 lerp(const vec3& start, const vec3& end, float lerp)
	{
		return start * lerp + end * (1.f - lerp);
	}

	template <typename T>
	__forceinline T lerp(T start, T end, T lerp)
	{
		return start * lerp + end * (1.f - lerp);
	}

	template <typename T>
	__forceinline T clamp(T n, T lower, T upper)
	{
		return max(lower, min(n, upper));
	}

	inline bool is_near(float a, float b, float tolerance)
	{
		return fabsf(a - b) <= tolerance;
	}

	__forceinline void angle_vectors(const vec3& angles, vec3* forward = nullptr, vec3* right = nullptr, vec3* up = nullptr)
	{
		float sr, sp, sy, cr, cp, cy;

		sin_cos(deg2rad(angles[PITCH]), &sp, &cp);
		sin_cos(deg2rad(angles[YAW]),   &sy, &cy);
		sin_cos(deg2rad(angles[ROLL]),  &sr, &cr); 

		if (forward)
		{
			forward->x = -cp * sy;
			forward->y = -sp;
			forward->z = cp * cy;
		}

		if (right)
		{
			right->x = -sr * -sp * sy - -cr * cy;
			right->y = sr * -cp;
			right->z = sr * -sp * cy - -cr * sy;
		}

		if (up)
		{
			up->x = cr * sp * sy + -sr * cy;
			up->y = cr * -cp;
			up->z = -cr * sp * cy - sr * sy;
		}
	}

	__forceinline void rotate2d(vec3& vec, float rot)
	{
		float s = 0.f, c = 0.f, x = 0.f, z = 0.f;
		sin_cos(deg2rad(rot), &s, &c);

		x = vec.x;
		z = vec.z;

		vec.x = (x * c) - (z * s);
		vec.z = (x * s) + (z * c);
	}
}
