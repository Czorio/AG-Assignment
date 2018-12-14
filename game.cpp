#include "precomp.h" // include (only) this in every .cpp file

Renderer *renderer;

// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	Camera cam = Camera( vec3( 0.f, -2.f, -2.f ), vec3( 0.f, -2.f, 0.f ), vec3( 0.f, -1.f, 0.f ), PI / 4, ( (float)SCRWIDTH / (float)SCRHEIGHT ) );

	vector<Primitive *> prims = vector<Primitive *>();
	vector<Light *> lights = vector<Light *>();

	Material mat;
	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 0.75f, 0.75f, 0.75f );

	vector<Primitive *> bunny = loadOBJ( "assets/scene.obj", mat );
	prims.insert( prims.end(), bunny.begin(), bunny.end() );

	Light *l = new Light();
	l->type = LightType::DIRECTIONAL_LIGHT;
	l->color = vec3( 1.f, 1.f, 1.f );
	l->intensity = 1.f;
	l->fov = PI / 16;
	l->direction = vec3( 1.f, 1.f, 1.f );
	l->origin = vec3( -2.f, -5.f, 0.f );
	lights.push_back( l );

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

bool rotLeft = false;
bool rotRight = false;
bool rotUp = false;
bool rotDown = false;
bool rotCW = false;
bool rotCCW = false;

// -----------------------------------------------------------
// Main application tick function
// -----------------------------------------------------------
void Game::Tick( float deltaTime )
{
	// Handle input
	if ( moveLeft )
	{
		renderer->moveCam( vec3( -0.05f, 0.f, 0.f ) );
	}

	if ( moveRight )
	{
		renderer->moveCam( vec3( 0.05f, 0.f, 0.f ) );
	}

	if ( moveUp )
	{
		renderer->moveCam( vec3( 0.f, 0.05f, 0.f ) );
	}

	if ( moveDown )
	{
		renderer->moveCam( vec3( 0.f, -0.05f, 0.f ) );
	}

	if ( moveForward )
	{
		renderer->moveCam( vec3( 0.f, 0.f, 0.05f ) );
	}

	if ( moveBackward )
	{
		renderer->moveCam( vec3( 0.f, 0.f, -0.05f ) );
	}

	if ( rotLeft )
	{
		renderer->rotateCam( vec3( 0.05f, 0.f, 0.f ) );
	}

	if ( rotRight )
	{
		renderer->rotateCam( vec3( -0.05f, 0.f, 0.f ) );
	}

	if ( rotUp )
	{
		renderer->rotateCam( vec3( 0.f, -0.05f, 0.f ) );
	}

	if ( rotDown )
	{
		renderer->rotateCam( vec3( 0.f, 0.05f, 0.f ) );
	}

	if ( rotCW )
	{
		renderer->rotateCam( vec3( 0.f, 0.f, 0.05f ) );
	}

	if ( rotCCW )
	{
		renderer->rotateCam( vec3( 0.f, 0.f, -0.05f ) );
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
		screen->Print( "W - Move forward\n", 2, 10, 0xFFFFFF );
		screen->Print( "S - Move back\n", 2, 18, 0xFFFFFF );
		screen->Print( "A - Move left\n", 2, 26, 0xFFFFFF );
		screen->Print( "D - Move right\n", 2, 34, 0xFFFFFF );
		screen->Print( "Q - Rotate counter clock wise\n", 2, 42, 0xFFFFFF );
		screen->Print( "E - Rotate clock wise\n", 2, 50, 0xFFFFFF );
		screen->Print( "Space - Move up\n", 2, 58, 0xFFFFFF );
		screen->Print( "Left Ctrl - Move down\n", 2, 66, 0xFFFFFF );
		screen->Print( "Move mouse or use the arrow keys to rotate camera\n", 2, 74, 0xFFFFFF );
	}
}

constexpr float rot_speed = 0.005f;

void Tmpl8::Game::MouseMove( int x, int y )
{
	renderer->getCamera()->rotate( vec3( -x * rot_speed, y * rot_speed, 0.f ) );
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
	case SDL_SCANCODE_LEFT:
		rotLeft = false;
		break;
	case SDL_SCANCODE_RIGHT:
		rotRight = false;
		break;
	case SDL_SCANCODE_UP:
		rotUp = false;
		break;
	case SDL_SCANCODE_DOWN:
		rotDown = false;
		break;
	case SDL_SCANCODE_Q:
		rotCCW = false;
		break;
	case SDL_SCANCODE_E:
		rotCW = false;
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
		break;
	case SDL_SCANCODE_LEFT:
		rotLeft = true;
		break;
	case SDL_SCANCODE_RIGHT:
		rotRight = true;
		break;
	case SDL_SCANCODE_UP:
		rotUp = true;
		break;
	case SDL_SCANCODE_DOWN:
		rotDown = true;
		break;
	case SDL_SCANCODE_Q:
		rotCCW = true;
		break;
	case SDL_SCANCODE_E:
		rotCW = true;
		break;
	default:
		break;
	}
}
