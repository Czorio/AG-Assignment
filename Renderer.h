#pragma once
class Renderer
{
  public:
	Renderer( vector<Primitive *> primitives );
	~Renderer();

	void renderFrame();
	void setCamera( Camera cam );

	Camera *getCamera();
	void moveCam( vec3 vec );
	void rotateCam( vec3 vec );
	void zoomCam( float deltaZoom );
	void changeAperture( float deltaAperture );
	void focusCam();

	Pixel *getOutput() const;

  private:
	vector<tuple<int, int>> tiles;

	Camera cam;
	vector<Primitive *> primitives;
	const BVH bvh;
	// vector<Light *> lights;

	unsigned currentIteration;
	vec3 *prebuffer;
	Pixel *buffer;
	bool *boolbuffer; // TEST

	vec3 shootRay( unsigned x, unsigned y, unsigned depth ) const;
	vec3 shootRay( const Ray &r, unsigned depth ) const;

	void invalidatePrebuffer();

	// rgb to Pixel
	Pixel rgb( float r, float g, float b ) const;
	Pixel rgb( vec3 vec ) const;

	vec3 gammaCorrect( vec3 vec ) const;
};
