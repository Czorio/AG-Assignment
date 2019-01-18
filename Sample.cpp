#include "precomp.h"

// From: http://www.rorydriscoll.com/2009/01/07/better-sampling/
// Actually does not work!
vec3 Sample::cosineSampleHemisphere( const float &u1, const float &u2 )
{
	const float r = sqrt( u1 );
	const float theta = 2 * PI * u2;

	const float x = r * cosf( theta );
	const float y = r * sinf( theta );

	return vec3( x, y, sqrt( std::max( 0.f, 1 - u1 ) ) );
}

// From: Scratchapixel
vec3 Sample::uniformSampleHemisphere( const float &r1, const float &r2 )
{
	// cos(theta) = r1 = y
	// cos^2(theta) + sin^2(theta) = 1 -> sin(theta) = srtf(1 - cos^2(theta))
	float sinTheta = sqrtf( 1 - r1 * r1 );
	float phi = 2 * M_PI * r2;
	float x = sinTheta * cosf( phi );
	float z = sinTheta * sinf( phi );
	return vec3( x, r1, z );
}