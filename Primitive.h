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
};

// https://goo.gl/UwMZKg  link to scratchapixel with code and figure
struct Sphere : public Primitive
{
	float radius;
	float r2;

	Sphere( vec3 origin, float radius, Material mat ) : Primitive( origin, mat ), radius( radius ), r2( radius * radius ) { }

	Hit hit( const Ray &ray ) const override
	{
		Hit P;
		P.hitType = 0;
		float t0, t1;
		vec3 L = origin - ray.origin;
		float tca = dot( L, ray.direction );
		if ( tca < 0 ) return P;
		float d2 = dot( L, L ) - tca * tca;
		if ( d2 > r2 ) return P;
		float thc = sqrt( r2 - d2 );
		t0 = tca - thc;
		t1 = tca + thc;

		if ( t0 > t1 ) std::swap( t0, t1 );

		if ( t0 < 0 )
		{
			t0 = t1;				// if t0 is negative, let's use t1 instead
			if ( t0 < 0 ) return P; // both t0 and t1 are negative
		}

		P.hitType = 1;
		P.t = t0;
		P.coordinates = ray( t0 );
		P.mat = mat;

		vec3 normal = P.coordinates - origin;
		normal.normalize();
		P.normal = normal;
		return P;
	}
};

struct Plane : public Primitive
{
	vec3 n;
	Plane( vec3 origin, vec3 normal, Material mat ) : Primitive( origin, mat ), n( normal ) { }

	Hit hit( const Ray &ray ) const override
	{
		Hit P;
		vec3 normal = n; // so that it can be normalized
		normal.normalize();
		P.hitType = 0;

		float denom = dot( normal, ray.direction );

		if ( abs( denom ) > 1e-6 ) // DEFINE AN EPSILON?
		{
			vec3 p0_O = origin - ray.origin; // vector from ray origin to plane origin (point p0)
			float t = dot( p0_O, normal ) / denom;

			if ( t >= 0 )
			{
				P.hitType = 1;
				P.t = t;
				P.coordinates = ray( t );
				P.mat = mat;
				P.normal = normal;
			}
		}
		return P;
	}
};