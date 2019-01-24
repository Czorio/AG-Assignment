#pragma once

struct Texture
{
	unsigned int width;
	unsigned int height;
	vec3 *values;
};

enum MaterialType
{
	LAMBERTIAN_MAT,
	DIELECTRIC_MAT,
	SPECULAR_MAT,
	EMIT_MAT // for lights
};

struct Material
{
	MaterialType type;
	vec3 albedo;
	vec3 emission;
	float roughness;

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
			vec3 albedo = vec3();

			converter.pixel = buffer[i];

			albedo.x = (float)converter.c.r / 255.f;
			albedo.y = (float)converter.c.g / 255.f;
			albedo.z = (float)converter.c.b / 255.f;

			diffuse->values[i] = albedo;
		}

		hasDiffuseTexture = true;
	}

	// Use this to get the albedo at the hit coordinates
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
			return albedo;
		}
	}

  private:
	bool hasDiffuseTexture = false;
	Texture *diffuse;
};