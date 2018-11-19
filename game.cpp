#include "precomp.h" // include (only) this in every .cpp file

Renderer *renderer;

// rgb to Pixel
Pixel rgb( float r, float g, float b )
{
	clamp( r, 0.f, 1.f );
	clamp( g, 0.f, 1.f );
	clamp( b, 0.f, 1.f );

	Pixel pix = 0x000000;

	unsigned char cr = r * 255;
	unsigned char cg = g * 255;
	unsigned char cb = b * 255;

	pix = ( (int)cr << 16 ) | ( (int)cg << 8 ) | ( (int)cb );

	return pix;
}

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	int noPrims = 25;
	int noLights = 3;

	Camera cam = Camera( vec3( 5.f, 5.f, -10.f ), vec3( 0.f, 0.f, 1.f ), 1.f );

	vector<Primitive *> prims = vector<Primitive *>( noPrims );
	vector<Light *> lights = vector<Light *>( noLights );

	Material mat;
	for ( unsigned i = 0; i < noPrims; i++ )
	{
		float spec = Rand( 1.f );
		float radius = Rand( 1.f );
		vec3 origin = vec3( Rand( 10.f ), Rand( 10.f ), Rand( 10.f ) );
		float r = Rand( 1.f );
		float g = Rand( 1.f );
		float b = Rand( 1.f );
		mat.color = rgb( r, g, b );
		mat.spec = spec;
		prims[i] = new Sphere( origin, radius, mat );

		printf( "Sphere %d\n", i );
		printf( "\tpos: (%f; %f; %f)\n", origin.x, origin.y, origin.z );
		printf( "\trad: %f\n", radius );
		printf( "\trgb: (%f, %f, %f)\n", r, g, b );
		printf( "\tmat.color: %x\n", mat.color );
		printf( "\tmat.spec: %f\n", mat.spec );
	}

	for ( unsigned i = 0; i < noLights; i++ )
	{
		Light *l = new Light();
		float r = Rand( 1.f );
		float g = Rand( 1.f );
		float b = Rand( 1.f );

		l->color = rgb( r, g, b );
		l->intensity = 2.5f;
		l->origin = vec3( Rand( 5.f ), Rand( 5.f ), Rand( 5.f ) );
		lights[i] = l;
	}

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
