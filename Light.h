#pragma once

struct Light
{
	vec3 origin;
	vec3 color;
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