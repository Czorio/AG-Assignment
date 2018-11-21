#pragma once
class Camera
{
  public:
	vec3 origin;
	vec3 direction;
	vec3 screenCenter;
	vec3 p0, p1, p2;

	Camera() : origin( vec3() ), direction( vec3() ), screenCenter( vec3() ), p0( vec3() ), p1( vec3() ), p2( vec3() ) {}

	Camera( vec3 origin, vec3 direction, float fov ) : origin( origin ), direction( direction )
	{
		screenCenter = origin + fov * direction;
		p0 = screenCenter + vec3( -1, -1, 0 );
		p1 = screenCenter + vec3( 1, -1, 0 );
		p2 = screenCenter + vec3( -1, 1, 0 );
	}

	Ray getRay( unsigned x, unsigned y ) const
	{
		Ray r;
		r.origin = origin;

		// Convert screenspace to UV
		float u = float(x) / SCRWIDTH;
		float v = float(y) / SCRHEIGHT;

		vec3 direction = ( P( u, v ) - origin );
		float length = direction.length();
		direction *= 1 / length;
		r.direction = direction;

		return r;
	}

  private:
	vec3 P( float u, float v ) const
	{
		return p0 + u * ( p1 - p0 ) + v * ( p2 - p0 );
	}
};
