#pragma once
struct Hit
{
	int hitType;
	float t;
	vec3 coordinates;
	vec3 normal;
	Material mat;
};

struct Ray
{
	vec3 origin;
	vec3 direction;

	vec3 operator()(const float t) const
	{
		return origin + t * direction;
	}
};
