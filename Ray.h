#pragma once
struct Hit
{
	bool isHit;
	vec3 coordinates;
	vec3 normal;
	Material mat;
};

struct Ray
{
	vec3 origin;
	vec3 direction;

	vec3 operator()(float t)
	{
		return origin + t * direction;
	}
};
