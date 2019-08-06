#pragma once

class ray_trace_t
{
	vec3 m_origin;
	vec3 m_dir;
	vec3 m_dir_inv;
public:
	explicit ray_trace_t(const vec3& origin, const vec3& dir)
	{
		m_origin = origin;
		m_dir = dir;
		m_dir_inv = vec3(1.f / m_dir.x, 1.f / m_dir.y, 1.f / m_dir.z);
	}

	bool trace(const vec3& mins, const vec3& maxs, float& distance);
};
