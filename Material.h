#pragma once

enum MaterialType
{
	DIFFUSE_MAT,
	MIRROR_MAT,
	GLASS_MAT
};

struct Material
{
	MaterialType type;
	vec3 color;
	float spec;
	float refractionIndex;
	float attenuation;
};