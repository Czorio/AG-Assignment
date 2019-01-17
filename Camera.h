#pragma once

// Based on https://github.com/Hillsoft/Raytracer-Tutorial
class Camera
{
  public:
	vec3 origin;

	vec3 forward;
	vec3 up;
	vec3 right;

	float width;
	float height;

	float aperture;
	float focalLength;
	float focusDistance;

	Camera() : origin( vec3() ), forward( vec3() ), up( vec3() ), right( vec3() ) {}

	Camera( vec3 origin, vec3 target, vec3 upGuide, float fov, float aspect, float aperture, float focalLength, float focusDistance ) : origin( origin ), aperture( aperture ), focalLength( focalLength ), focusDistance( focusDistance )
	{
		forward = ( target - origin ).normalized();
		right = forward.cross( upGuide ).normalized();
		up = right.cross( forward );

		height = tan( fov );
		width = height * aspect;
	}

	Ray getRay( unsigned x, unsigned y ) const
	{
		Ray r;

		// Randomize origin for DoF
		vec3 randVec = rotateVec( up, forward, Rand( 2 * PI ) );
		r.origin = origin + randVec * Rand( aperture );

		// Add some AA
		float norm_x = ( ( float( x ) + ( -1.f + Rand( 1.f ) ) ) / float( SCRWIDTH ) ) - 0.5f;
		float norm_y = ( ( float( y ) + ( -1.f + Rand( 1.f ) ) ) / float( SCRHEIGHT ) ) - 0.5f;

		vec3 imagePoint = norm_x * right * ( focusDistance * 0.5f ) * ( 1 / focalLength ) + norm_y * up * ( focusDistance * 0.5f ) * ( 1 / focalLength ) + origin + forward * focusDistance;

		r.direction = imagePoint - r.origin;

		return r;
	}

	// Relative zoom is true when you simply want to zoom in or out
	// Relative zoom is false when you want to jump to a specific value
	void zoom( float value, bool relativeZoom )
	{
		if ( relativeZoom )
		{
			if ( focalLength + value > 0.f )
			{
				focalLength += value;
			}
			else
			{
				focalLength = 0.01f;
			}
		}
		else
		{
			focalLength = value;
		}
	}

	void changeAperture( float value, bool relativeChange )
	{
		if ( relativeChange )
		{
			if ( aperture + value > 0.f )
			{
				aperture += value;
			}
			else
			{
				aperture = 0.f;
			}
		}
		else
		{
			aperture = value;
		}
	}

	Ray focusRay()
	{
		Ray r;
		r.origin = origin;
		r.direction = forward;

		return r;
	}

	// Own code
	void move( vec3 v )
	{
		origin += ( right * v.x + up * v.y + forward * v.z );
	}

	// Stack overflow: https://stackoverflow.com/questions/42421611/3d-vector-rotation-in-c
	vec3 rotateVec( const vec3 &v, const vec3 &axis, float angle ) const
	{
		float cos_angle = cos( angle );
		float sin_angle = sin( angle );

		vec3 rotated = ( v * cos_angle ) + ( axis.cross( v ) * sin_angle ) + ( axis * axis.dot( v ) ) * ( 1 - cos_angle );

		return rotated;
	}

	// Own code
	void rotate( vec3 v )
	{
		vec3 new_forward = rotateVec( forward, up, v.x );	// Left/Right
		new_forward = rotateVec( new_forward, right, -v.y ); // Up/Down

		forward = new_forward.normalized();
		right = forward.cross( rotateVec( up, forward, v.z ) ).normalized(); // Roll
		up = right.cross( forward );
	}
};
