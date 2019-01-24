#pragma once
struct Hit
{
	Hit() : hitType( 0 ), t( FLT_MAX ) {}

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

enum RayType
{
	PRIMARY_RAY,
	LIGHT_RAY,
	MIRROR_RAY,
	INDIRECT_RAY
};

struct Ray
{
	vec3 origin;
	vec3 direction;
	RayType type;
	// Refraction index of current medium
	float refractionIndex = 1.f;
	
	// Ray constructor
	Ray() : type(PRIMARY_RAY) {}

	vec3 operator()( const float t ) const
	{
		return origin + t * direction;
	}
};
