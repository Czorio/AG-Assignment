#include "precomp.h" // include (only) this in every .cpp file

Renderer *renderer;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	unsigned noPrims = 12;
	unsigned noLights = 2;

	Camera cam = Camera( vec3( 0.f, 0.f, -5.f ), vec3( 0.f, 0.f, 1.f ), 1.f, ( (float)SCRWIDTH / (float)SCRHEIGHT ) );

	vector<Primitive *> prims = vector<Primitive *>( noPrims );
	vector<Light *> lights = vector<Light *>( noLights );

  // Base sphere
	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 0.5f, 0.5f, 0.5f );
	prims[0] = new Sphere( vec3( 0.f, 1005.f, 0.f ), 1000.f, mat );

	// Back wall
	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 1.f, 1.f, 1.f );
	prims[1] = new Sphere( vec3( 0.f, 0.f, 1005.f ), 0.f, mat );

	// Glass Sphere
	Material glassMat;
	glassMat.type = MaterialType::GLASS_MAT;
	glassMat.color = vec3( 1.f, 1.f, 1.f );
	glassMat.spec = 1.f;
	glassMat.refractionIndex = 1.1f;
	glassMat.attenuation = 2.5f;
	prims[2] = new Sphere( vec3( 0.f, 0.f, 0.f ), 0.f, glassMat );

	// 3x3 grid of spheres
	// Top row
	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 1.f, 0.f, 0.f );
	prims[3] = new Sphere( vec3( -1.f, -1.f, 4.f ), .5f, mat );

	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 1.f, 0.33f, 0.33f );
	prims[4] = new Sphere( vec3( 0.f, -1.f, 4.f ), .5f, mat );

	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 1.f, 0.67f, 0.67f );
	prims[5] = new Sphere( vec3( 1.f, -1.f, 4.f ), .5f, mat );

	// Mid row
	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 0.f, 1.f, 0.f );
	prims[6] = new Sphere( vec3( -1.f, 0.f, 4.f ), .5f, mat );

	mat.type = MaterialType::MIRROR_MAT;
	mat.spec = 0.5f;
	mat.color = vec3( 0.33f, 1.f, 0.33f );
	prims[7] = new Sphere( vec3( 0.f, 0.f, 4.f ), .5f, mat );

	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 0.67f, 1.f, 0.67f );
	prims[8] = new Sphere( vec3( 1.f, 0.f, 4.f ), .5f, mat );

	// Bot Row
	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 0.f, 0.f, 1.f );
	prims[9] = new Sphere( vec3( -1.f, 1.f, 4.f ), .5f, mat );

	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 0.33f, 0.33f, 1.f );
	prims[10] = new Sphere( vec3( 0.f, 1.f, 4.f ), .5f, mat );

	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 0.67f, 0.67f, 1.f );
	prims[11] = new Sphere( vec3( 1.f, 1.f, 4.f ), .5f, mat );

	// Create "sun"
	Light *l = new Light();
	l->type = LightType::SPOT_LIGHT;
	l->color = vec3( 1.f, 1.f, 1.f );
	l->intensity = 20.f;
	l->fov = 12.5f;
	l->direction = vec3( 1.f, 1.f, 0.f );
	l->direction.normalize();
	l->origin = vec3( 0.f, 3.f, 4.f );
	lights[0] = l;

	// Create point light
	Light *l2 = new Light();
	l2->type = LightType::POINT_LIGHT;
	l2->color = vec3( 1.f, 1.f, 1.f );
	l2->intensity = 0.f;
	l2->direction = vec3( 0.f, 1.f, 1.f );
	l2->direction.normalize();
	l2->origin = vec3( 0.f, -2.f, 1.f );
	lights[1] = l2;

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

bool showHelp = false;

bool moveLeft = false;
bool moveRight = false;
bool moveUp = false;
bool moveDown = false;
bool moveForward = false;
bool moveBackward = false;

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	// Handle input
	if ( moveLeft )
	{
		renderer->moveCam( vec3( -0.01f, 0.f, 0.f ) );
	}

	if ( moveRight )
	{
		renderer->moveCam( vec3( 0.01f, 0.f, 0.f ) );
	}

	if ( moveUp )
	{
		renderer->moveCam( vec3( 0.f, -0.01f, 0.f ) );
	}

	if ( moveDown )
	{
		renderer->moveCam( vec3( 0.f, 0.01f, 0.f ) );
	}

	if ( moveForward )
	{
		renderer->moveCam( vec3( 0.f, 0.f, 0.01f ) );
	}

	if ( moveBackward )
	{
		renderer->moveCam( vec3( 0.f, 0.f, -0.01f ) );
	}

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
	if ( !showHelp )
	{
		screen->Print( "Press \"h\" for controls", 2, 8, 0xFFFFFF );
	}
	else
	{
		screen->Print( "W - Move forward\nS - Move back\nA - Move left\nD - Move right\nSpace - Move up\nLeft Ctrl - Move down", 2, 10, 0xFFFFFF );
	}
}

void Tmpl8::Game::MouseMove( int x, int y )
{
	// TODO: Rotation
}

void Tmpl8::Game::KeyUp( int key )
{
	switch ( key )
	{
	case SDL_SCANCODE_D:
		moveRight = false;
		break;
	case SDL_SCANCODE_A:
		moveLeft = false;
		break;
	case SDL_SCANCODE_W:
		moveForward = false;
		break;
	case SDL_SCANCODE_S:
		moveBackward = false;
		break;
	case SDL_SCANCODE_SPACE:
		moveUp = false;
		break;
	case SDL_SCANCODE_LCTRL:
		moveDown = false;
		break;
	default:
		break;
	}
}

void Tmpl8::Game::KeyDown( int key )
{
	switch ( key )
	{
	case SDL_SCANCODE_D:
		moveRight = true;
		break;
	case SDL_SCANCODE_A:
		moveLeft = true;
		break;
	case SDL_SCANCODE_W:
		moveForward = true;
		break;
	case SDL_SCANCODE_S:
		moveBackward = true;
		break;
	case SDL_SCANCODE_SPACE:
		moveUp = true;
		break;
	case SDL_SCANCODE_LCTRL:
		moveDown = true;
		break;
	case SDL_SCANCODE_H:
		showHelp = !showHelp;
	default:
		break;
	}
}
