#pragma once

struct Light
{
	vec3 origin;
	Pixel color;
	float intensity;
};

struct SpotLight : public Light
{
	vec3 direction;
};

struct DirectionalLight : public Light
{
	vec3 direction;
};