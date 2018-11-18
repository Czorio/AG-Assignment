#include "precomp.h"

Renderer::Renderer()
{
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
	vector<Ray> rays = vector<Ray>( SCRWIDTH * SCRHEIGHT );

	for ( unsigned y = 0; y < SCRHEIGHT; y++ )
	{
		for ( unsigned x = 0; x < SCRWIDTH; x++ )
		{
			rays[y * SCRWIDTH + x] = cam.getRay( x, y );
		}
	}

	Hit h;
	for ( unsigned i = 0; i < SCRWIDTH * SCRHEIGHT; i++ )
	{
		for ( Primitive *p : primitives )
		{
			h = p->hit( rays[i] );

			if ( h.isHit )
			{
				buffer[i] = h.mat.color;
			}
			else
			{
				// Background if no hit
				buffer[i] = 0x0000AA;
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

Pixel *Renderer::getOutput()
{
	return buffer;
}
