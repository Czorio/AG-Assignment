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
		h.isHit = false;
		vec3 C = origin - r.origin;
		float t = dot( C, r.direction );
		vec3 Q = C - t * r.direction;
		float p2 = dot( Q, Q );

		if ( p2 > r2 )
		{
			return h; // r2 = r * r
		}

		t -= sqrt( r2 - p2 );

		h.isHit = true;
		h.t = t;
		h.coordinates = r( t );
		h.mat = mat;

		vec3 normal = h.coordinates - origin;
		normal.normalize();
		h.normal = normal;
		return h;
	}
};