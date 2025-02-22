#pragma once

struct Primitive
{
	vec3 origin;
	Material mat;

	Primitive()
	{
		mat.albedo = vec3();
		mat.emission = vec3();
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
		Hit h = Hit();
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
		aabb bounds = aabb( origin - vec3( radius + EPSILON, radius + EPSILON, radius + EPSILON ), origin + vec3( radius + EPSILON, radius + EPSILON, radius + EPSILON ) );
		return bounds;
	}
};

// Deprecated since AABB cannot be easily determined for an infinite plane
/*struct Plane : public Primitive
{
	vec3 n;
	Plane( vec3 origin, vec3 normal, Material mat ) : Primitive( origin, mat ), n( normal ) {}

	Hit hit( const Ray &ray ) const override
	{
		Hit h = Hit();
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
};*/

struct Triangle : public Primitive
{
	vec3 v0, v1, v2;
	vec2 uv0, uv1, uv2;

	Triangle( Material mat, vec3 *verteces, vec2 *uv ) : Primitive( vec3(), mat )
	{
		v0 = verteces[0];
		v1 = verteces[1];
		v2 = verteces[2];

		uv0 = uv[0];
		uv1 = uv[1];
		uv2 = uv[2];

		origin = vec3( ( v0.x + v1.x + v2.x ) / 3, ( v0.y + v1.y + v2.y ) / 3, ( v0.z + v1.z + v2.z ) / 3 );
	}

	// Based on ScratchaPixel's implementation
	Hit hit( const Ray &ray ) const override
	{
		Hit h = Hit();

		// Edges
		const vec3 &edge_1 = v1 - v0;
		const vec3 &edge_2 = v2 - v0;

		// Normal
		const vec3 &n = edge_1.cross( edge_2 ).normalized();

		const vec3 &q = ray.direction.cross( edge_2 );
		const float a = edge_1.dot( q );

		// Parallel?
		if ( abs( a ) <= EPSILON )
		{
			return h;
		}

		const vec3 &s = ( ray.origin - v0 ) * ( 1.f / a );
		const vec3 &r = s.cross( edge_1 );

		// Barycentric coordinates
		const float b0 = s.dot( q );
		const float b1 = r.dot( ray.direction );
		const float b2 = 1.f - b0 - b1;

		// Are we within the triangle?
		if ( b0 < 0.f || b1 < 0.f || b2 < 0.f )
		{
			return h;
		}

		// We have a hit!
		const float t = edge_2.dot( r );
		if ( t >= 0.f )
		{
			// From what direction do we hit the triangle? Inside or Outside?
			if ( n.dot( ray.direction ) >= 0.f )
			{
				h.hitType = -1;
			}
			else
			{
				h.hitType = 1;
			}

			h.coordinates = ray( t );
			h.t = t;
			h.mat = mat;
			h.normal = n;

			// Calculate UV
			h.u = b0 * uv0.x + b1 * uv1.x + b2 * uv2.x;
			h.v = b0 * uv0.y + b1 * uv1.y + b2 * uv2.y;
			return h;
		}
		else
		{
			return h;
		}
	}

	aabb volume() const override
	{
		aabb bounds = aabb();
		bounds.Reset();
		bounds.Grow( vec3( v0.x + EPSILON, v0.y + EPSILON, v0.z + EPSILON ) );
		bounds.Grow( vec3( v1.x + EPSILON, v1.y + EPSILON, v1.z + EPSILON ) );
		bounds.Grow( vec3( v2.x + EPSILON, v2.y + EPSILON, v2.z + EPSILON ) );
		return bounds;
	}
};