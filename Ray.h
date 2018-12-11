#pragma once
struct Hit
{
	// World
	int hitType; // -1 hit from inside; 0 no hit; 1 hit
	float t;
	vec3 coordinates;
	vec3 normal;

	// Texture mapping
	float u;
	float v;
	Material mat;
};

struct Ray
{
	vec3 origin;
	vec3 direction;
	// Refraction index of current medium
	float refractionIndex = 1.f;

	vec3 operator()( const float t ) const
	{
		return origin + t * direction;
	}
};
