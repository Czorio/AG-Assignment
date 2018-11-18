#include "precomp.h" // include (only) this in every .cpp file

Renderer *renderer;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	int noPrims = 25;
	int noLights = 3;

	Camera cam = Camera( vec3( 2.5, 2.5, -10 ), vec3( 0, 0, 1 ), 1.0 );

	vector<Primitive *> prims = vector<Primitive *>( noPrims );
	vector<Light *> lights = vector<Light *>( noLights );

	Material mat;
	for ( unsigned i = 0; i < noPrims; i++ )
	{
		float spec = float( ( rand() % 255 ) / 255.f );

		mat.color = rand() % 0xFFFFFF;
		mat.spec = spec;
		prims[i] = new Sphere( vec3( rand() % 5, rand() % 5, rand() % 5 ), rand() % 3, mat );
	}

	for ( unsigned i = 0; i < noLights; i++ )
	{
		Light *l = new Light();
		float r = float( ( rand() % 255 ) / 255.f );
		float g = float( ( rand() % 255 ) / 255.f );
		float b = float( ( rand() % 255 ) / 255.f );
		l->color = vec3( r, g, b );
		l->intensity = 2.5f;
		l->origin = vec3( rand() % 2, rand() % 2, rand() % 2 );
		lights[i] = l;
	}

	renderer = new Renderer();
	renderer->setCamera( cam );
	renderer->setLights( lights );
	renderer->setPrimitives( prims );
}

// -----------------------------------------------------------
// Close down application
// -----------------------------------------------------------
void Game::Shutdown()
{
	delete renderer;
}

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	// clear the graphics window
	screen->Clear( 0 );

	// Render the frame
	timer t = timer();
	renderer->renderFrame();
	float elapsed = t.elapsed();
	cout << 1 / (elapsed / 1000) << endl;

	// Display
	screen->SetBuffer( renderer->getOutput() );
	string fps = "FPS: " + int( 1 / elapsed ) + string("\n");
	screen->Print( fps.c_str(), 2, 2, 0xFFFFFF );

	printf( fps.c_str() );
}
