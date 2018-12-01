#include "precomp.h" // include (only) this in every .cpp file

Renderer *renderer;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	unsigned noPrims = 27;
	unsigned noLights = 1;

	Camera cam = Camera( vec3( 0.f, 0.f, -30.f ), vec3( 0.f, 0.f, 1.f ), 1.f, (SCRWIDTH / SCRHEIGHT) );

	vector<Primitive *> prims = vector<Primitive *>( noPrims );
	vector<Light *> lights = vector<Light *>( noLights );

	// Base
	Material mat;
	mat.color = vec3( 0.75f, 0.75f, 0.75f );
	mat.spec = 0.25f;
	prims[0] = new Plane( vec3( 0.f, 10.f, -5.f ), vec3( 0.f, -1.f, -0.1f ), mat );
	
	// prims[0] = new Sphere( vec3( 0.f, 1000010.f, 0.f ), 1000000.f, mat );

	// Glass Sphere
	GlassMaterial glassMat;
	glassMat.color = vec3( 1.f, 1.f, 1.f );
	glassMat.spec = 1.f;
	glassMat.refraction = 0.5f;
	glassMat.n = 1.41;
	glassMat.attenuation = 0.f;
	prims[1] = new Sphere( vec3( 0.f, 0.f, -25.f ), 2.f, glassMat );

	// Generate random spheres
	for ( unsigned i = 2; i < noPrims; i++ )
	{
		float spec = Rand( 0.5f );
		float radius = 0.5f + Rand( 1.5f );
		vec3 origin = vec3( -12.5f + Rand( 25.f ), -12.5f + Rand( 25.f ), -12.5f + Rand( 25.f ) );
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
	l->intensity = 1.f;
	l->direction = vec3( 0.f, 1.f, 0.f );
	l->origin = vec3();

	lights[0] = l;

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
	float fps = 1 / ( elapsed / 1000 );

	// Display
	screen->SetBuffer( renderer->getOutput() );
	screen->Print( ( "FPS: " + to_string( fps ) ).c_str(), 2, 2, 0xFFFFFF );
}

void Tmpl8::Game::KeyDown( int key )
{
	switch ( key )
	{
	case SDL_SCANCODE_D:
		renderer->getCamera()->move( vec3( -0.1f, 0.f, 0.f ) );
		break;
	case SDL_SCANCODE_A:
		renderer->getCamera()->move( vec3( 0.1f, 0.f, 0.f ) );
		break;
	case SDL_SCANCODE_W:
		renderer->getCamera()->move( vec3( 0.f, 0.f, 0.1f ) );
		break;
	case SDL_SCANCODE_S:
		renderer->getCamera()->move( vec3( 0.f, 0.f, -0.1f ) );
		break;
	case SDL_SCANCODE_SPACE:
		renderer->getCamera()->move( vec3( 0.f, -0.1f, 0.f ) );
		break;
	case SDL_SCANCODE_LCTRL:
		renderer->getCamera()->move( vec3( 0.f, 0.1f, 0.f ) );
		break;
	default:
		break;
	}
}
