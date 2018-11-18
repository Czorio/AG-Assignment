#include "precomp.h"

Ray::Ray()
{
}

Ray::~Ray()
{
}

vec3 Ray::operator()( float t )
{
	return origin + t * direction;
}
