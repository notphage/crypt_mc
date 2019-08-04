#include "context.h"

bool ray_trace_t::trace(const vec3& mins, const vec3& maxs, float& distance)
{
	// If line is parallel and outsite the box it is not possible to intersect 
	if (m_dir.x == 0.f && (m_origin.x < std::min(mins.x, maxs.x) || m_origin.x > std::max(mins.x, maxs.x)))
		return false;

	if (m_dir.y == 0.f && (m_origin.y < std::min(mins.y, maxs.y) || m_origin.y > std::max(mins.y, maxs.y)))
		return false;

	if (m_dir.z == 0.f && (m_origin.z < std::min(mins.z, maxs.z) || m_origin.z > std::max(mins.z, maxs.z)))
		return false;

	float t1 = (mins.x - m_origin.x) * m_dir_inv.x;
	float t2 = (maxs.x - m_origin.x) * m_dir_inv.x;
	float t3 = (mins.y - m_origin.y) * m_dir_inv.y;
	float t4 = (maxs.y - m_origin.y) * m_dir_inv.y;
	float t5 = (mins.z - m_origin.z) * m_dir_inv.z;
	float t6 = (maxs.z - m_origin.z) * m_dir_inv.z;

	float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
	float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

	if (tmax < 0.f)
	{
		distance = tmax;
		return false;
	}

	if (tmin > tmax)
	{
		distance = tmax;
		return false;
	}

	distance = tmin;
	return true;
}