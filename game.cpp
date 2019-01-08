#include "precomp.h" // include (only) this in every .cpp file

Renderer *renderer;
int noPrim;
int noLight;

	// -----------------------------------------------------------
// Initialize the application
// -----------------------------------------------------------
void Game::Init()
{
	Camera cam = Camera( vec3( 0.f, -1.5f, -2.f ), vec3( 0.f, -1.5f, 0.f ), vec3( 0.f, -1.f, 0.f ), PI / 4, ( (float)SCRWIDTH / (float)SCRHEIGHT ) );

	Material mat;
	mat.type = MaterialType::DIFFUSE_MAT;
	mat.spec = 0.5f;
	mat.attenuation = 0.4f;
	mat.refractionIndex = 1.41f;
	mat.color = vec3( 0.75f, 0.25f, 0.25f );

	vector<Primitive *> scene = loadOBJ( "assets/Monkey.obj", mat );

	// Base plane
	mat.type = MaterialType::MIRROR_MAT;
	mat.spec = 0.5f;
	mat.color = vec3( 0.125f, 0.125f, 0.125f );
	scene.push_back( new Sphere( vec3( 0.f, 101.5f, 0.f ), 100.f, mat ) );

	// Back wall
	mat.type = MaterialType::DIFFUSE_MAT;
	mat.color = vec3( 0.25f, 0.25f, 0.75f );
	scene.push_back( new Sphere( vec3( 0.f, 0.f, 105.f ), 100.f, mat ) );

	vector<Light *> lights = vector<Light *>();

	// Key light
	Light *key = new Light();
	key->type = LightType::SPOT_LIGHT;
	key->color = vec3( 1.f, 1.f, 1.f );
	key->intensity = 15.f;
	key->fov = PI / 8;
	key->direction = vec3( -1.f, 0.f, 1.f );
	key->origin = vec3( 3.5f, -1.5f, -3.5f );
	lights.push_back( key );

	// fill light
	Light *fill = new Light();
	fill->type = LightType::SPOT_LIGHT;
	fill->color = vec3( 1.f, 1.f, 1.f );
	fill->intensity = 7.5f;
	fill->fov = PI / 4;
	fill->direction = vec3( 1.f, 0.f, 1.f );
	fill->origin = vec3( -3.5f, -1.5f, -3.5f );
	lights.push_back( fill );

	// Back light
	Light *back = new Light();
	back->type = LightType::SPOT_LIGHT;
	back->color = vec3( 1.f, 1.f, 1.f );
	back->intensity = 7.5f;
	back->fov = PI / 4;
	back->direction = vec3( -1.f, 0.f, -1.f );
	back->origin = vec3( 3.5f, -1.5f, 3.5f );
	lights.push_back( back );

	renderer = new Renderer( scene );
	noPrim = scene.size();
	noLight = lights.size();
	renderer->setCamera( cam );
	renderer->setLights( lights );
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
	screen->Print( ( "FPS: " + to_string( fps ) + " " + to_string( noPrim ) + " Primitives, " + to_string( noLight ) + " Lights" ).c_str(), 2, 2, 0xFFFFFF );
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
