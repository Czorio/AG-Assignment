#pragma once
class Renderer
{
  public:
	Renderer( unsigned maxDepth );
	~Renderer();

	void renderFrame();

	void setPrimitives( vector<Primitive *> primitives );
	void setLights( vector<Light *> lights );
	void setCamera( Camera cam );

	Camera *getCamera();

	Pixel *getOutput();

  private:
	unsigned maxDepth;

	vector<thread> threads;
	vector<tuple<int, int>> tiles;

	Camera cam;
	vector<Primitive *> primitives;
	vector<Light *> lights;
	Pixel *buffer;

	vec3 shootRay( unsigned x, unsigned y, unsigned depth ) const;
	vec3 shootRay( const Ray &r, unsigned depth ) const;
	vec3 shadowRay( const Hit &h, const Light *l ) const;

	// rgb to Pixel
	Pixel rgb( float r, float g, float b ) const;
	Pixel rgb( vec3 vec ) const;
};
