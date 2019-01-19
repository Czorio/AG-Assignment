#pragma once

// https : //stackoverflow.com/questions/19665818/generate-random-numbers-using-c11-random-library
class Sample
{
  public:

	 // Sample();
	static vec3 uniformSampleHemisphere(const float &r1, const float &r2 );
	static vec3 cosineSampleHemisphere( const float &r1, const float &r2 );

	static void createLocalCoordinateSystem( const vec3 &N, vec3 &Nt, vec3 &Nb );
};