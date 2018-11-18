#pragma once
class Ray
{
  public:
	vec3 origin;
	vec3 direction;

	vec3 operator()(float t)
	{
		return origin + t * direction;
	}
};
