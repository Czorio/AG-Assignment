#include "precomp.h"

Renderer::Renderer( unsigned maxDepth )
{
	threads = vector<thread>( thread::hardware_concurrency() );

	this->maxDepth = maxDepth;
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
				vec3 color = shootRay( x + dx, y + dy, maxDepth );
				buffer[( y + dy ) * SCRWIDTH + ( x + dx )] = rgb( color );
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

	// Initialize color to black
	closestHit.mat.color = vec3();

	// Find nearest hit
	for ( Primitive *p : primitives )
	{
		Hit tmp = p->hit( r );
		if ( tmp.isHit )
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
		return rgb( 0.f, 0.f, 0.f );
	}

	vec3 lightIntensity = vec3();

	for ( Light *l : lights )
	{
		lightIntensity += shadowRay( closestHit, l );
	}

	vec3 color = closestHit.mat.color * lightIntensity;

	return color;
}

vec3 Renderer::shadowRay( const Hit &h, const Light *l ) const
{
	Ray shadowRay;
	// Shadow bias
	shadowRay.origin = h.coordinates + ( 0.0004f * h.normal );
	float dist;
	vec3 dir;

	if ( l->type == LightType::DIRECTIONAL_LIGHT )
	{
		dist = FLT_MAX;
		dir = -1 * l->direction;
	}
	else if ( l->type == LightType::POINT_LIGHT )
	{
		dir = l->origin - h.coordinates;
		dist = dir.length();
		dir.normalize();
	}

	shadowRay.direction = dir;

	// TODO: Cast ray from hit to light

	float dot = h.normal.dot( dir );
	if ( dot > 0 )
	{
		return l->color * dot;
	}
	else
	{
		return vec3();
	}
}

Pixel Renderer::rgb( float r, float g, float b ) const
{
	clamp( r, 0.f, 1.f );
	clamp( g, 0.f, 1.f );
	clamp( b, 0.f, 1.f );

	Pixel pix = 0x000000;

	unsigned char cr = r * 255;
	unsigned char cg = g * 255;
	unsigned char cb = b * 255;

	pix = ( (int)cr << 16 ) | ( (int)cg << 8 ) | ( (int)cb );

	return pix;
}

Pixel Renderer::rgb( vec3 vec ) const
{
	return rgb( vec.x, vec.y, vec.z );
}
