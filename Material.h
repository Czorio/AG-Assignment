#pragma once

struct Material
{
	vec3 color;
	float spec;
};

struct GlassMaterial : public Material
{
	float n;
	float attenuation;
};