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

	void loadDiffuse( char *filename )
	{
		// Load file
		Surface image = Surface( filename );

		// Prepare Texture
		diffuse = new Texture();
		diffuse->height = image.GetHeight();
		diffuse->width = image.GetWidth();

		// Load values into Texture
		Pixel *buffer = image.GetBuffer();
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

		hasDiffuseTexture = true;
	}

	// Use this to get the color at the hit coordinates
	vec3 getDiffuse( float u, float v )
	{
		if ( hasDiffuseTexture )
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
	bool hasDiffuseTexture = false;
	Texture *diffuse;
};