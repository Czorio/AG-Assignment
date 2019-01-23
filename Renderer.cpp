#include "precomp.h"

Renderer::Renderer( vector<Primitive *> primitives ) : bvh( BVH( primitives ) )
{
	currentIteration = 1;

	prebuffer = new vec3[SCRWIDTH * SCRHEIGHT];

	for ( unsigned i = 0; i < SCRWIDTH * SCRHEIGHT; i++ )
	{
		prebuffer[i] = vec3( 0.f, 0.f, 0.f );
	}

	buffer = new Pixel[SCRWIDTH * SCRHEIGHT];

	for ( unsigned y = 0; y < SCRHEIGHT; y += TILESIZE )
	{
		for ( unsigned x = 0; x < SCRWIDTH; x += TILESIZE )
		{
			tuple<int, int> element = make_pair( x, y );
			tiles.push_back( element );
		}
	}

	this->primitives = primitives;

	// Create a vector with the primitives that are light sources
	// Handy for the Next Event Estimation
	uint count = 0;
	for ( auto prim : primitives )
	{
		if ( prim->mat.type == EMIT )
			lightIndices.push_back( count++ );
	}
}

Renderer::~Renderer()
{
	for ( unsigned i = 0; i < primitives.size(); i++ )
	{
		delete primitives[i];
	}

	delete[] prebuffer;
	prebuffer = nullptr;

	delete[] buffer;
	buffer = nullptr;
}

void Renderer::renderFrame( bool bvh_debug )
{
	if ( currentIteration < ITERATIONS )
	{
#pragma omp parallel for
		for ( int i = 0; i < tiles.size(); i++ )
		{
			int x = get<0>( tiles[i] );
			int y = get<1>( tiles[i] );

			for ( unsigned dy = 0; dy < TILESIZE; dy++ )
			{
				for ( unsigned dx = 0; dx < TILESIZE; dx++ )
				{
					if ( ( x + dx ) < SCRWIDTH && ( y + dy ) < SCRHEIGHT )
					{
						// Firefly surpression
						vec3 color = shootRay( x + dx, y + dy, 0, bvh_debug );

						if ( color.sqrLentgh() > FIREFLY * FIREFLY )
						{
							color *= vec3(1.f / color.length());
						}

						prebuffer[( y + dy ) * SCRWIDTH + ( x + dx )] += color;
					}
				}
			}
		}
		currentIteration++;
	}
	else
	{
		// Prevent stupidly high framerate
		Sleep( ( 1.f / MAX_IDLE_FPS ) * 1000 );
	}
}

void Renderer::invalidatePrebuffer()
{
	for ( size_t i = 0; i < SCRWIDTH * SCRHEIGHT; i++ )
	{
		prebuffer[i] = vec3( 0.f, 0.f, 0.f );
	}

	currentIteration = 1;
}

void Renderer::setCamera( Camera cam )
{
	this->cam = cam;
}

Camera *Renderer::getCamera()
{
	return &cam;
}

// As preparation for iterative rendering
void Renderer::moveCam( vec3 vec )
{
	invalidatePrebuffer();
	cam.move( vec );
}

// As preparation for iterative rendering
void Renderer::rotateCam( vec3 vec )
{
	invalidatePrebuffer();
	cam.rotate( vec );
}

void Renderer::zoomCam( float deltaZoom )
{
	invalidatePrebuffer();
	cam.zoom( deltaZoom, true );
}

void Renderer::changeAperture( float deltaAperture )
{
	invalidatePrebuffer();
	cam.changeAperture( deltaAperture, true );
}

void Renderer::focusCam()
{
	invalidatePrebuffer();
	Hit h = bvh.intersect( cam.focusRay() );

	cam.focusDistance = h.t;
}

Pixel *Renderer::getOutput() const
{
	// currentSample - 1 because it is increased in the renderFrame() function in preparation of the next frame.
	// Unfortunately, we are getting the current frame, so we get currentSample - 1.
	float importance = 1.f / float( currentIteration - 1 );
	for ( unsigned i = 0; i < SCRWIDTH * SCRHEIGHT; i++ )
	{
		buffer[i] = rgb( gammaCorrect( prebuffer[i] * importance ) );
	}

	return buffer;
}

vec3 Renderer::shootRay( unsigned x, unsigned y, unsigned depth, bool bvh_debug ) const
{
	Ray r = cam.getRay( x, y );
	return shootRay( r, depth, bvh_debug );
}

__inline void clampFloat( float &val, float lo, float hi )
{
	if ( val > hi )
	{
		val = hi;
	}
	else if ( val < lo )
	{
		val = lo;
	}
}

// FROM Ray-tracer:
//Ray getReflectedRay( const vec3 &incoming, const vec3 &normal, const vec3 &hitLocation )
//{
//	Ray r;
//	vec3 outgoing = incoming - 2.f * incoming.dot( normal ) * normal;
//	r.origin = hitLocation + ( REFLECTIONBIAS * outgoing );
//	r.direction = outgoing;
//
//	return r;
//}

vec3 getPointOnHemi()
{
#ifdef IMPORTANCE_SAMPLING
	vec3 point = Sample::cosineSampleHemisphere( Rand( 1.f ), Rand( 1.f ) );
#else
	vec3 point = Sample::uniformSampleHemisphere( Rand( 1.f ), Rand( 1.f ) );
#endif

	return point;
}

vec3 calculateDiffuseRayDir( const vec3 &N, const vec3 &Nt, const vec3 &Nb )
{
	// Sample the random point on unit hemisphere
	vec3 pointOnHemi = getPointOnHemi();

	// Transform point vector to the local coordinate system of the hit point
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/global-illumination-path-tracing/global-illumination-path-tracing-practical-implementation

	vec3 newdir(
		pointOnHemi.x * Nb.x + pointOnHemi.y * N.x + pointOnHemi.z * Nt.x,
		pointOnHemi.x * Nb.y + pointOnHemi.y * N.y + pointOnHemi.z * Nt.y,
		pointOnHemi.x * Nb.z + pointOnHemi.y * N.z + pointOnHemi.z * Nt.z );

	// Diffused ray with the calculated random direction and origin same as the hit point
	return normalize( newdir );
}

void Renderer::randomPointOnLight( const vec3 &sensorPoint, vec3 &randomPoint, float &randomLightArea, vec3 &lightNormal ) const
{
	int randomLight = (int)Rand( lightIndices.size() );
	randomPoint = primitives[randomLight]->getRandomSurfacePoint( sensorPoint );
	const float &d = ( randomPoint - sensorPoint ).length();
	randomLightArea = primitives[randomLight]->getArea( d );
	lightNormal = primitives[randomLight]->getNormal( randomPoint );
}

vec3 Renderer::shootRay( const Ray &r, unsigned depth, bool bvh_debug ) const
{
	if ( depth > MAXRAYDEPTH ) return vec3( 0.f, 0.f, 0.f );

	if ( bvh_debug )
	{
		return bvh.debug( r );
	}

	Hit closestHit = bvh.intersect( r );

	// No hit
	if ( closestHit.t == FLT_MAX )
	{
		return vec3( 0.f, 0.f, 0.f );
	}

	// Next Event Estimation
	// Closest hit is light source
	if ( closestHit.mat.type == EMIT )
	{
		switch ( r.type )
		{
		case RayType::INDIRECT_RAY:
			return vec3( 0.f, 0.f, 0.f );
			break;
		case RayType::LIGHT_RAY:
			return closestHit.mat.emission;
			break;
		default:
			return closestHit.mat.albedo;
			break;
		}
	}
	// Closest hit is object/primitive but ray is light ray
	// Does not work for cases where light source is obstracted by another light source
	// Currently: does not work for mirrors!
	if ( closestHit.mat.type != EMIT && r.type == RayType::LIGHT_RAY )
		return vec3( 0.f, 0.f, 0.f );

	// Create the local coordinate system of the hit point
	vec3 Nt, Nb;
	Sample::createLocalCoordinateSystem( closestHit.normal, Nt, Nb );

	// Direct illumination calculations
	// Calculate direct ray (aiming to a random light)
	float randomLightArea;
	vec3 randomPoint;
	vec3 lightNormal;
	Renderer::randomPointOnLight( closestHit.coordinates, randomPoint, randomLightArea, lightNormal );

	// As in slide 18 (lecture "Path Tracing")
	vec3 L = randomPoint - closestHit.coordinates;
	float distance = L.length();
	L = normalize( L );

	// The following are the visibility factor
	float cos_o = dot( -L, lightNormal );	  // --> checks for light-ray intersection
	float cos_i = dot( L, closestHit.normal ); // --> checks for primitive-ray intersection
	//if ( ( cos_o <= 0 ) || ( cos_i <= 0 ) )
	//	return vec3( 0.f, 0.f, 0.f );
	cos_o = std::max( 0.f, cos_o );
	cos_i = std::max( 0.f, cos_i );

	// Slide 26 ("Variance reduction")
	// Shoot the direct ray (light ray)
	Ray directRay;
	directRay.direction = L;
	directRay.origin = closestHit.coordinates + REFLECTIONBIAS * directRay.direction;
	directRay.type = RayType::LIGHT_RAY;
	vec3 Ld = shootRay( directRay, 0, false ); // no splitting for light rays, depth=0

	// Calculate random diffused ray (cosine weighted or uniform depending if "I.S." on)
	Ray diffray;
	diffray.direction = calculateDiffuseRayDir( closestHit.normal, Nt, Nb );
	diffray.origin = closestHit.coordinates + REFLECTIONBIAS * diffray.direction;
	diffray.type = RayType::INDIRECT_RAY;

	// N*R dot product, Slide 47 Lecture "Variance reduction"
	float dpro = dot( closestHit.normal, diffray.direction );

	if ( dpro < 0 )
	{
		// This normally should not happen
		std::cout << "dpro < 0 " << std::endl;
		return vec3( 0.f, 0.f, 0.f );
	}

#ifdef IMPORTANCE_SAMPLING
	// Importance sampling (slide 47- lecture 8,9)
	vec3 PDF = dpro / PI;
#else
	vec3 PDF = 1 / ( 2.0f * PI );
#endif

	// Update light accumulutation
	vec3 BRDF = closestHit.mat.albedo * ( 1 / PI );
	vec3 Ei = shootRay( diffray, depth + 1, false ) * dpro;

	// No / operator !
	// Dont divide with zero ! - Put the following in some function ???
	if ( PDF.x > 0 )
		PDF.x = std::max( EPSILON, PDF.x );
	else
	{
		std::cout << "Negative PDF" << std::endl;
		return vec3( 0.f, 0.f, 0.f );
		//PDF.x = std::min( -EPSILON, PDF.x );
	}

	if ( PDF.y > 0 )
		PDF.y = std::max( EPSILON, PDF.y );
	else
	{
		std::cout << "Negative PDF" << std::endl;
		return vec3( 0.f, 0.f, 0.f );
		//PDF.y = std::min( -EPSILON, PDF.y );
	}

	if ( PDF.z > 0 )
		PDF.z = std::max( EPSILON, PDF.z );
	else
	{
		std::cout << "Negative PDF" << std::endl;
		return vec3( 0.f, 0.f, 0.f );
		//PDF.z = std::min( -EPSILON, PDF.z );
	}

	Ei.x = Ei.x / PDF.x;
	Ei.y = Ei.y / PDF.y;
	Ei.z = Ei.z / PDF.z;

	float solidAngle = ( cos_o * randomLightArea ) / ( distance * distance );
	Ld = Ld * solidAngle * BRDF * cos_i;

	return BRDF * Ei + Ld;
}

Pixel Renderer::rgb( float r, float g, float b ) const
{
	clampFloat( r, 0.f, 1.f );
	clampFloat( g, 0.f, 1.f );
	clampFloat( b, 0.f, 1.f );

	unsigned char cr = r * 255;
	unsigned char cg = g * 255;
	unsigned char cb = b * 255;

	Color c;
	c.c.a = 255; // alpha
	c.c.r = cr;  // red
	c.c.g = cg;  // green
	c.c.b = cb;  // blue

	return c.pixel;
}

Pixel Renderer::rgb( vec3 vec ) const
{
	return rgb( vec.x, vec.y, vec.z );
}

union simdVector {
	__m128 v;   // SSE 4 x float vector
	float a[4]; // scalar array of 4 floats
};

vec3 Renderer::gammaCorrect( vec3 vec ) const
{
	__m128 val = _mm_set_ps( vec.x, vec.y, vec.z, vec.dummy );
	__m128 corrected = _mm_sqrt_ps( val );

	simdVector convert;
	convert.v = corrected;
	vec3 res = vec3( convert.a[3], convert.a[2], convert.a[1] );
	return res;
}