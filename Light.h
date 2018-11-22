#pragma once

enum LightType
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT
};

struct Light
{
	LightType type;
	vec3 origin;
	vec3 direction;
	vec3 color;
};