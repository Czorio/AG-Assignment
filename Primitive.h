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

	virtual Hit hit( const Ray &r ) const = 0;
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

				return h;
			}
			else if (t1 < 0 && t2 > 0)
			{
				// This situation happens when you start from within the sphere
				h.hitType = -1;
				h.t = t2;

				h.coordinates = r( t2 );
				h.mat = mat;

				vec3 normal = h.coordinates - origin;
				normal.normalize();
				h.normal = normal;

				return h;
			}
			else if (t1 < 0 && t2 < 0)
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
};