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

	Camera cam;
	vector<Primitive *> primitives;
	vector<Light *> lights;
	Pixel *buffer;

	Pixel shootRay( unsigned x, unsigned y, unsigned depth ) const;
	Pixel shootRay( const Ray &r, unsigned depth ) const;
	float shadowRay( const Hit &h, const Light *l ) const;

	// rgb to Pixel
	Pixel rgb( float r, float g, float b ) const;
};
