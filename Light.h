#pragma once

enum LightType
{
	DIRECTIONAL_LIGHT,
	POINT_LIGHT,
	SPOT_LIGHT
};

struct Light
{
	LightType type;
	vec3 origin;
	vec3 direction;
	float intensity;
	float fov;
	vec3 color;
};