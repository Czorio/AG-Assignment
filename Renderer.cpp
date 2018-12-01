#include "precomp.h"

// Makes converting from RGB to a Pixel easy
union Color {
	Pixel pixel;
	struct
	{
		Pixel b : 8;
		Pixel g : 8;
		Pixel r : 8;
		Pixel a : 8;
	} c;
};

Renderer::Renderer()
{
	threads = vector<thread>( thread::hardware_concurrency() );

	buffer = new Pixel[SCRWIDTH * SCRHEIGHT];

	for ( unsigned y = 0; y < SCRHEIGHT; y += TILESIZE )
	{
		for ( unsigned x = 0; x < SCRWIDTH; x += TILESIZE )
		{
			tuple<int, int> element = make_pair( x, y );
			tiles.push_back( element );
		}
	}
}

Renderer::~Renderer()
{
	for ( unsigned i = 0; i < primitives.size(); i++ )
	{
		delete primitives[i];
	}

	for ( unsigned i = 0; i < lights.size(); i++ )
	{
		delete lights[i];
	}

	delete[] buffer;
	buffer = nullptr;
}

void Renderer::renderFrame()
{
#pragma omp parallel for
	for ( int i = 0; i < tiles.size(); i++ )
	{
		int x = get<0>( tiles[i] );
		int y = get<1>( tiles[i] );

		for ( unsigned dy = 0; dy < TILESIZE; dy++ )
		{
			for ( unsigned dx = 0; dx < TILESIZE; dx++ )
			{
				if ( ( x + dx ) < SCRWIDTH && ( y + dy ) < SCRHEIGHT )
				{
					vec3 color = shootRay( x + dx, y + dy, MAXDEPTH );
					buffer[( y + dy ) * SCRWIDTH + ( x + dx )] = rgb( color );
				}
			}
		}
	}
}

void Renderer::setPrimitives( vector<Primitive *> primitives )
{
	this->primitives = primitives;
}

void Renderer::setLights( vector<Light *> lights )
{
	this->lights = lights;
}

void Renderer::setCamera( Camera cam )
{
	this->cam = cam;
}

Camera *Renderer::getCamera()
{
	return &cam;
}

Pixel *Renderer::getOutput()
{
	return buffer;
}

vec3 Renderer::shootRay( unsigned x, unsigned y, unsigned depth ) const
{
	Ray r = cam.getRay( x, y );
	return shootRay( r, depth );
}

vec3 Renderer::shootRay( const Ray &r, unsigned depth ) const
{
	Hit closestHit;
	closestHit.t = FLT_MAX;

	// Find nearest hit
	for ( Primitive *p : primitives )
	{
		Hit tmp = p->hit( r );
		if ( tmp.hitType != 0 )
		{
			if ( tmp.t < closestHit.t )
			{
				closestHit = tmp;
			}
		}
	}

	// No hit
	if ( closestHit.t == FLT_MAX )
	{
		return vec3();
	}

	vec3 lightIntensity = vec3( AMBIENTLIGHT, AMBIENTLIGHT, AMBIENTLIGHT );

	// Shadows
	for ( Light *l : lights )
	{
		lightIntensity += shadowRay( closestHit, l );
	}

	vec3 color = vec3();

	// Reflection
	if ( depth > 0 && closestHit.mat.type == MaterialType::MIRROR_MAT )
	{
		Ray refl;
		vec3 reflDir = r.direction - 2.f * r.direction.dot( closestHit.normal ) * closestHit.normal;
		refl.origin = closestHit.coordinates + ( REFLECTIONBIAS * reflDir );
		refl.direction = reflDir;

		vec3 specular = shootRay( refl, depth - 1 );
		color = ( closestHit.mat.color * lightIntensity ) * ( 1.f - closestHit.mat.spec );
		specular *= closestHit.mat.spec;
		color += specular;
	}
	// Refraction
	else if ( depth > 0 && closestHit.mat.type == MaterialType::GLASS_MAT )
	{
		float n = r.refractionIndex / closestHit.mat.refractionIndex;
		// If we hit from inside, flip the normal
		vec3 normal = closestHit.normal * closestHit.hitType;
		float cosI = -1 * normal.dot( r.direction );
		float cosT2 = 1.f - ( n * n ) * ( 1.f - ( cosI * cosI ) );

		if ( cosT2 > 0.f )
		{
			// Refraction occurs, send out rays
			Ray refr;
			vec3 refractedDirection = ( n * r.direction ) + ( n * cosI - sqrtf( cosT2 ) ) * normal;
			refractedDirection.normalize();
			refr.origin = closestHit.coordinates + ( REFRACTIONBIAS * refractedDirection );
			refr.direction = refractedDirection;

			vec3 attenuation = vec3( 1.f, 1.f, 1.f );

			if ( closestHit.hitType == -1 )
			{
				// If we hit from inside the object, add in Attenuation and set the refraction index on the Ray
				refr.refractionIndex = closestHit.mat.refractionIndex;

				// Calculate attenuation of the light through Beer's Law
				vec3 absorbance = ( vec3( 1.f, 1.f, 1.f ) - closestHit.mat.color ) * closestHit.mat.attenuation * -1.f * closestHit.t;
				attenuation = vec3( expf( absorbance.x ), expf( absorbance.y ), expf( absorbance.z ) );
			}

			vec3 refracted = shootRay( refr, depth - 1 );

			color += refracted * attenuation;
		}
	}
	// Not refractive or reflective, or we've reached the end of the allowed depth
	else
	{
		color = closestHit.mat.color * lightIntensity;
	}

	return color;
}

vec3 Renderer::shadowRay( const Hit &h, const Light *l ) const
{
	Ray shadowRay;
	float dist;
	float intensity;
	float inverseSquare;
	vec3 dir;

	if ( l->type == LightType::DIRECTIONAL_LIGHT )
	{
		dist = FLT_MAX;
		dir = -1 * l->direction;
		inverseSquare = 1.f;
		intensity = l->intensity;
	}
	else if ( l->type == LightType::POINT_LIGHT )
	{
		dir = l->origin - h.coordinates;
		dist = dir.length();
		dir.normalize();
		inverseSquare = 1 / ( dist * dist );
		intensity = l->intensity;
	}

	shadowRay.direction = dir;
	// Shadow bias
	shadowRay.origin = h.coordinates + ( SHADOWBIAS * dir );

	for ( Primitive *prim : primitives )
	{
		if ( prim->hit( shadowRay ).hitType != 0 )
		{
			return vec3();
		}
	}

	float dot = h.normal.dot( dir );
	if ( dot > 0 )
	{
		return l->color * intensity * dot * inverseSquare;
	}
	else
	{
		return vec3();
	}
}

inline void clampFloat( float &val, float lo, float hi )
{
	if ( val > hi )
	{
		val = hi;
	}
	else if ( val < lo )
	{
		val = lo;
	}
}

Pixel Renderer::rgb( float r, float g, float b ) const
{
	clampFloat( r, 0.f, 1.f );
	clampFloat( g, 0.f, 1.f );
	clampFloat( b, 0.f, 1.f );

	unsigned char cr = r * 255;
	unsigned char cg = g * 255;
	unsigned char cb = b * 255;

	Color c;
	c.c.a = 255; // alpha
	c.c.r = cr;  // red
	c.c.g = cg;  // green
	c.c.b = cb;  // blue

	return c.pixel;
}

Pixel Renderer::rgb( vec3 vec ) const
{
	return rgb( vec.x, vec.y, vec.z );
}
