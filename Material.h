#pragma once

struct Material
{
	Pixel color;
	float spec;
};

struct GlassMaterial : public Material
{
	float n;
	float attenuation;
};