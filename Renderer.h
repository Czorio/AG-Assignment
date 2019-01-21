#pragma once
class Renderer
{
  public:
	Renderer( vector<Primitive *> primitives );
	~Renderer();

	void renderFrame( bool bvh_debug );
	void setCamera( Camera cam );

	Camera *getCamera();
	void moveCam( vec3 vec );
	void rotateCam( vec3 vec );
	void zoomCam( float deltaZoom );
	void changeAperture( float deltaAperture );
	void focusCam();

	Pixel *getOutput() const;
	void invalidatePrebuffer();

  private:
	vector<tuple<int, int>> tiles;

	Camera cam;
	vector<Primitive *> primitives;
	const BVH bvh;
	// vector<Light *> lights;

	unsigned currentIteration;
	vec3 *prebuffer;
	float *depthbuffer;
	vec3 *postbuffer;
	Pixel *buffer;
	bool *boolbuffer; // TEST

	float *kernel;

	vec3 shootRay( unsigned x, unsigned y, unsigned depth, bool bvh_debug ) const;
	vec3 shootRay( const unsigned x, const unsigned y, const Ray &r, unsigned depth, bool bvh_debug ) const;

	// rgb to Pixel
	Pixel rgb( float r, float g, float b ) const;
	Pixel rgb( vec3 vec ) const;

	vec3 gammaCorrect( vec3 vec ) const;
};
