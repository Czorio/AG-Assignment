#pragma once

struct Primitive
{
	vec3 origin;
	Material mat;

	Primitive() : origin( vec3() )
	{
		mat.color = vec3();
		mat.spec = 0.f;
	}

	Primitive( vec3 origin, Material mat ) : origin( origin ), mat( mat ) {}

	virtual Hit hit( const Ray &ray ) const = 0;
	virtual aabb volume() const = 0;
};

struct Sphere : public Primitive
{
	float radius;
	float r2;

	Sphere( vec3 origin, float radius, Material mat ) : Primitive( origin, mat ), radius( radius ), r2( radius * radius ) {}

	Hit hit( const Ray &r ) const override
	{
		Hit h;
		h.hitType = 0;

		float a = r.direction.dot( r.direction );
		float b = ( 2.f * r.direction ).dot( r.origin - origin );
		float c = ( r.origin - origin ).dot( r.origin - origin ) - r2;

		float d = ( b * b ) - ( 4 * a * c );
		float t;

		if ( d < 0 ) // No hits
		{
			return h;
		}
		else if ( d == 0 ) // One hit
		{
			// Square root of d is 0, we can leave it out
			t = ( -1 * b ) / ( 2 * a );

			if ( t < 0 )
			{
				return h;
			}

			h.t = t;
			h.coordinates = r( t );
			h.mat = mat;

			vec3 normal = h.coordinates - origin;
			normal.normalize();
			h.normal = normal;

			float dot = normal.dot( r.direction );

			if ( dot > 0 ) // inside out
			{
				h.hitType = -1;
			}
			else if ( dot < 0 ) // outside in
			{
				h.hitType = 1;
			}

			// Calculate UV coordinates for the texture
			h.u = 0.5f + atan2( normal.y, normal.x ) / 2 * PI;
			h.v = 0.5f - asin( normal.y ) / PI;

			return h;
		}
		else // Two hits
		{
			float t1 = ( ( -1 * b ) - sqrt( d ) ) / ( 2 * a ); // Should be lowest possible t; closest t
			float t2 = ( ( -1 * b ) + sqrt( d ) ) / ( 2 * a ); // Should be highest possible t; furthest t

			if ( t1 > 0 )
			{
				// Outside in
				h.hitType = 1;
				h.t = t1;
				h.coordinates = r( t1 );

				h.mat = mat;

				vec3 normal = h.coordinates - origin;
				normal.normalize();
				h.normal = normal;

				// Calculate UV coordinates for the texture
				h.u = 0.5f + atan2( normal.y, normal.x ) / 2 * PI;
				h.v = 0.5f - asin( normal.y ) / PI;

				return h;
			}
			else if ( t1 < 0 && t2 > 0 )
			{
				// This situation happens when you start from within the sphere
				h.hitType = -1;
				h.t = t2;

				h.coordinates = r( t2 );
				h.mat = mat;

				vec3 normal = h.coordinates - origin;
				normal.normalize();
				h.normal = normal;

				// Calculate UV coordinates for the texture
				h.u = 0.5f + atan2( normal.y, normal.x ) / 2 * PI;
				h.v = 0.5f - asin( normal.y ) / PI;

				return h;
			}
			else if ( t1 < 0 && t2 < 0 )
			{
				// No hits
				return h;
			}
			else
			{
				// Some weird situation that I didn't account for
				return h;
			}
		}
	}

	aabb volume() const override
	{
		aabb bounds = aabb( origin + vec3( radius, radius, radius ), origin - vec3( radius, radius, radius ) );
		bounds.Grow( vec3( EPSILON, EPSILON, EPSILON ) );
		return bounds;
	}
};

// Deprecated since AABB cannot be easily determined for an infinite plane
struct Plane : public Primitive
{
	vec3 n;
	Plane( vec3 origin, vec3 normal, Material mat ) : Primitive( origin, mat ), n( normal ) {}

	Hit hit( const Ray &ray ) const override
	{
		Hit h;
		vec3 normal = n; // so that it can be normalized
		normal.normalize();
		h.hitType = 0;

		float denom = dot( normal, ray.direction );

		if ( abs( denom ) > EPSILON )
		{
			vec3 p0_O = origin - ray.origin; // vector from ray origin to plane origin (point p0)
			float t = dot( p0_O, normal ) / denom;

			if ( t >= 0 )
			{
				h.hitType = 1;
				h.t = t;
				h.coordinates = ray( t );
				h.mat = mat;
				h.normal = normal;

				// Calculate UV coordinates for the texture
				vec3 u = primaryTextureDirection( normal );
				vec3 v = normal.cross( u );

				h.u = u.dot( h.coordinates );
				h.v = v.dot( h.coordinates );
			}
		}
		return h;
	}

	aabb volume() const override
	{
		return aabb();
	}

  private:
	// I asked a question on StackOverflow for this one
	// https://computergraphics.stackexchange.com/questions/8382/how-do-i-convert-a-hit-on-an-infinite-plane-to-uv-coordinates-for-texturing-in-a
	vec3 primaryTextureDirection( vec3 normal ) const
	{
		vec3 a = cross( normal, vec3( 1, 0, 0 ) );
		vec3 b = cross( normal, vec3( 0, 1, 0 ) );

		vec3 max_ab = dot( a, a ) < dot( b, b ) ? b : a;

		vec3 c = cross( normal, vec3( 0, 0, 1 ) );

		return normalize( dot( max_ab, max_ab ) < dot( c, c ) ? c : max_ab );
	}
};

struct Triangle : public Primitive
{
	vec3 v0, v1, v2;

	Triangle( vec3 origin, Material mat, vec3 *verteces ) : Primitive( origin, mat )
	{
		v0 = verteces[0];
		v1 = verteces[1];
		v2 = verteces[2];
	}

	// Based on ScratchaPixel's implementation
	Hit hit( const Ray &r ) const override
	{
		Hit h;
		h.hitType = 0;

		// compute plane's normal
		vec3 v0v1 = v1 - v0;
		vec3 v0v2 = v2 - v0;
		// no need to normalize
		vec3 N = v0v1.cross( v0v2 ); // N
		float area2 = N.length();

		// Step 1: finding P

		// check if ray and plane are parallel ?
		float NdotRayDirection = N.dot( r.direction );
		if ( fabs( NdotRayDirection ) < EPSILON ) // almost 0
		{
			return h; // they are parallel so they don't intersect !
		}

		// compute d parameter
		float d = N.dot( v0 );

		// compute t (equation 3)
		float t = ( N.dot( r.origin ) + d ) / NdotRayDirection;
		// check if the triangle is in behind the ray
		if ( t < 0 )
		{
			return h; // the triangle is behind
		}

		// compute the intersection point using equation 1
		vec3 P = r( t );

		// Step 2: inside-outside test
		vec3 C; // vector perpendicular to triangle's plane

		// edge 0
		vec3 edge0 = v1 - v0;
		vec3 vp0 = P - v0;
		C = edge0.cross( vp0 );
		if ( N.dot( C ) < 0 )
		{
			return h; // P is on the right side
		}

		// edge 1
		vec3 edge1 = v2 - v1;
		vec3 vp1 = P - v1;
		C = edge1.cross( vp1 );
		if ( N.dot( C ) < 0 )
		{
			return h; // P is on the right side
		}

		// edge 2
		vec3 edge2 = v0 - v2;
		vec3 vp2 = P - v2;
		C = edge2.cross( vp2 );
		if ( N.dot( C ) < 0 )
		{
			return h; // P is on the right side;
		}

		h.hitType = 1;
		h.coordinates = P;
		h.normal = N;

		return h; // this ray hits the triangle
	}

	aabb volume() const override
	{
	}
};