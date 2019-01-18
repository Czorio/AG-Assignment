#pragma once

// https : //stackoverflow.com/questions/19665818/generate-random-numbers-using-c11-random-library
class Sample
{
  public:

	 // Sample();
	vec3 uniformSampleHemisphere(const float &r1, const float &r2 );
	vec3 cosineSampleHemisphere( const float &r1, const float &r2 );
};