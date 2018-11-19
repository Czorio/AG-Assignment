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

Pixel Renderer::shootRay( unsigned x, unsigned y, unsigned depth )
{
	Ray r = cam.getRay( x, y );
	Hit h;
	Pixel pix = 0x000000;

	for ( Primitive *p : primitives )
	{
		h = p->hit( r );
		if ( h.isHit )
		{
			pix = h.mat.color;
		}
	}

	return pix;
}
