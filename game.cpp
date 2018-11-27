#include "precomp.h" // include (only) this in every .cpp file

Renderer *renderer;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	unsigned noPrims = 26;
	unsigned noLights = 1;

	Camera cam = Camera( vec3( 5.f, 5.f, -10.f ), vec3( 0.f, 0.f, 1.f ), 1.f );

	vector<Primitive *> prims = vector<Primitive *>( noPrims );
	vector<Light *> lights = vector<Light *>( noLights );

	Material mat;
	mat.color = vec3( 0.75f, 0.75f, 0.75f );
	mat.spec = 0.f;
	prims[0] = new Sphere( vec3( 0.f, -100.f, 0.f ), 100, mat );

	// Generate random spheres
	for ( unsigned i = 1; i < noPrims; i++ )
	{
		float spec = Rand( 1.f );
		float radius = Rand( 1.f );
		vec3 origin = vec3( Rand( 10.f ), Rand( 10.f ), Rand( 10.f ) );
		float r = Rand( 1.f );
		float g = Rand( 1.f );
		float b = Rand( 1.f );
		mat.color = vec3( r, g, b );
		mat.spec = spec;
		prims[i] = new Sphere( origin, radius, mat );
	}

	// Create "sun"
	Light *l = new Light();
	l->type = LightType::DIRECTIONAL_LIGHT;
	l->color = vec3( 1.f, 1.f, 1.f );
	l->direction = vec3( 0.f, 1.f, 0.f );
	l->origin = vec3();

	lights[0] = l;

	renderer = new Renderer( 50 );
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
	float fps = 1 / ( elapsed / 1000 );

	// Display
	screen->SetBuffer( renderer->getOutput() );
	screen->Print( ( "FPS: " + to_string( fps ) ).c_str(), 2, 2, 0xFFFFFF );
}

void Tmpl8::Game::KeyDown( int key )
{
	switch ( key )
	{
	case SDLK_RIGHT:
		renderer->getCamera()->origin += vec3( 0.1f, 0.f, 0.f );
		break;
	case SDLK_LEFT:
		renderer->getCamera()->origin -= vec3( 0.1f, 0.f, 0.f );
		break;
	case SDLK_UP:
		renderer->getCamera()->origin += vec3( 0.f, 0.1f, 0.f );
		break;
	case SDLK_DOWN:
		renderer->getCamera()->origin -= vec3( 0.f, 0.1f, 0.f );
		break;
	default:
		break;
	}

	printf( "Camera Pos: (%f; %f; %f)\n", renderer->getCamera()->origin.x, renderer->getCamera()->origin.y, renderer->getCamera()->origin.z );
}
