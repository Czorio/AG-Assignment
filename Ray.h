#pragma once
struct Hit
{
	int hitType;
	float t;
	vec3 coordinates;
	vec3 normal;
	Material mat;
};

enum RayType
{
	PRIMARY_RAY,
	SHADOW_RAY
};

struct Ray
{
	vec3 origin;
	vec3 direction;
	RayType type = PRIMARY_RAY;

	vec3 operator()(const float t) const
	{
		return origin + t * direction;
	}
};
