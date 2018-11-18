#pragma once
class Renderer
{
  public:
	Renderer();
	~Renderer();

	void renderFrame();

	void setPrimitives( vector<Primitive *> primitives );
	void setLights( vector<Light *> lights );
	void setCamera( Camera cam );

	Pixel *getOutput();

  private:
	Camera cam;
	vector<Primitive *> primitives;
	vector<Light *> lights;
	Pixel *buffer;
};
