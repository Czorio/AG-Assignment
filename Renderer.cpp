#include "precomp.h"

Renderer::Renderer( unsigned maxDepth )
{
	this->maxDepth = maxDepth;
	buffer = new Pixel[SCRWIDTH * SCRHEIGHT];
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
	for ( unsigned y = 0; y < SCRHEIGHT; y++ )
	{
		for ( unsigned x = 0; x < SCRWIDTH; x++ )
		{
			buffer[y * SCRWIDTH + x] = shootRay( x, y, maxDepth );
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

Pixel Renderer::shootRay( unsigned x, unsigned y, unsigned depth ) const
{
	Ray r = cam.getRay( x, y );
	return shootRay( r, depth );
}

Pixel Renderer::shootRay( const Ray &r, unsigned depth ) const
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

/*	float intensity = 0.f;

	for ( Light *l : lights )
	{
		intensity += shadowRay( closestHit, l );
	}
*/
	vec3 color = closestHit.mat.color;
	//color *= intensity;
	//printf("(%f; %f; %f) * %f\n", color.x, color.y, color.z, intensity);


	Pixel pix = rgb( color.x, color.y, color.z );

	return pix;
}

float Renderer::shadowRay( const Hit &h, const Light *l ) const
{
	Ray shadowRay;
	shadowRay.origin = h.coordinates;
	float dist;

	vec3 dir = l->origin - h.coordinates;
	dist = dir.length();
	dir.normalize();
	shadowRay.direction = dir;

	// check for obstructions
	for ( Primitive *p : primitives )
	{
		Hit shdw = p->hit( shadowRay );
		if ( shdw.isHit && dist > shdw.t )
		{
			return 0.f;
		}
	}

	float angle = abs(h.normal.dot( dir ));

	// No hits, return intensity at distance
	return l->intensity * angle * ( 1 / ( dist * dist ) );
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
