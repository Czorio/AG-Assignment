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
	virtual vec3 getRandomSurfacePoint( const vec3 &sensorPoint ) const = 0; // NEE, to sample point on light
	virtual float getArea( const float &d ) const = 0;						 // NEE, to sample point on light
	virtual vec3 getNormal( const vec3 &pointOnSurface ) const = 0;
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

	vec3 getRandomSurfacePoint( const vec3 &sensorPoint ) const override
	{
		// This function is only used for the sampling a random point on the light
		// so no cosine weighted choice even when Importance Sampling on (unlike diffuse rays)

		// Get a random point on hemisphere with unit radius
		vec3 unitPoint = Sample::uniformSampleHemisphere( Rand( 1 ), Rand( 1 ) ); // care with the randoms here

		// Transform the point to be on the surface part that is visible by the sensor point
		// N connects the origin of the sphere and the point we do the calculations for (sensor)
		vec3 N = normalize( sensorPoint - this->origin );
		vec3 Nt, Nb;
		// Create coordinate system
		Sample::createLocalCoordinateSystem( N, Nt, Nb );

		vec3 surfacePoint(
			unitPoint.x * Nb.x + unitPoint.y * N.x + unitPoint.z * Nt.x,
			unitPoint.x * Nb.y + unitPoint.y * N.y + unitPoint.z * Nt.y,
			unitPoint.x * Nb.z + unitPoint.y * N.z + unitPoint.z * Nt.z );

		// Normalize and the multiply by radius so that it is on the sphere surface
		surfacePoint = this->origin + normalize( surfacePoint ) * this->radius;

		return surfacePoint;
	}

	float getArea( const float &d ) const override
	{
		// Visible area of sphere from any point (r2 = radius*radius)
		// Source: https://math.stackexchange.com/questions/1329130/what-fraction-of-a-sphere-can-an-external-observer-see
		return 2 * PI * r2 * d / (radius + d);
	}

	vec3 getNormal(const vec3 &pointOnSurface) const override
	{
		return normalize( pointOnSurface - origin );
	}
};

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

	// Need to implement this? - Lights made of triangles?
	vec3 getRandomSurfacePoint( const vec3 &sensorPoint ) const override
	{
		return vec3( 0.f, 0.f, 0.f );
	}

	float getArea( const float &d ) const override // Next event estimation
	{
		return 0;
	}

	vec3 getNormal(const vec3 &pointOnSurface) const override // Next event estimation
	{
		return vec3(0.f, 0.f, 0.f);
	}
};