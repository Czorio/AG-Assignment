#pragma once

struct Texture
{
	unsigned int width;
	unsigned int height;
	vec3 *values;
};

enum MaterialType
{
	DIFFUSE_MAT,
	MIRROR_MAT,
	GLASS_MAT
};

struct Material
{
	MaterialType type;
	vec3 color;
	float spec;
	float refractionIndex;
	float attenuation;

	// From LodePNG examples
	void loadDiffuse( char *filename )
	{
		Surface *image = new Surface( filename );

		diffuse = new Texture();
		diffuse->height = image->GetHeight();
		diffuse->width = image->GetWidth();

		Pixel *buffer = image->GetBuffer();
		diffuse->values = new vec3[diffuse->width * diffuse->height];

		for ( unsigned i = 0; i < diffuse->height * diffuse->width; i++ )
		{
			Color converter;
			vec3 color = vec3();

			converter.pixel = buffer[i];

			color.x = (float)converter.c.r / 255.f;
			color.y = (float)converter.c.g / 255.f;
			color.z = (float)converter.c.b / 255.f;

			diffuse->values[i] = color;
		}

		delete image;

		hasTexture = true;
	}

	vec3 getDiffuse( float u, float v )
	{
		if ( hasTexture )
		{
			int x = int( u * diffuse->width ) % diffuse->width;
			int y = int( v * diffuse->height ) % diffuse->height;
			return diffuse->values[y * diffuse->width + x];
		}
		else
		{
			return color;
		}
	}

  private:
	bool hasTexture = false;
	Texture *diffuse;
};