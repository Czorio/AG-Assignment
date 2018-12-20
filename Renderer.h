#pragma once
class Renderer
{
  public:
	Renderer( vector<Primitive *> primitives );
	~Renderer();

	void renderFrame();

	void setLights( vector<Light *> lights );
	void setCamera( Camera cam );

	Camera *getCamera();
	void moveCam( vec3 vec );
	void rotateCam( vec3 vec );

	Pixel *getOutput() const;

  private:
	vector<tuple<int, int>> tiles;

	Camera cam;
	vector<Primitive *> primitives;
	const BVH bvh;
	vector<Light *> lights;
	Pixel *buffer;

	vec3 shootRay( unsigned x, unsigned y, unsigned depth ) const;
	vec3 shootRay( const Ray &r, unsigned depth ) const;
	vec3 shadowRay( const Hit &h, const Light *l ) const;

	// rgb to Pixel
	Pixel rgb( float r, float g, float b ) const;
	Pixel rgb( vec3 vec ) const;
};
