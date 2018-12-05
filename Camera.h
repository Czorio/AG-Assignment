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

	Camera() : origin( vec3() ), forward( vec3() ), up( vec3() ), right( vec3() ) {}

	Camera( vec3 origin, vec3 target, vec3 upGuide, float fov, float aspect ) : origin( origin )
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
		r.origin = origin;

		float norm_x = ( 2.0f * x ) / SCRWIDTH - 1.0f;
		float norm_y = ( -2.0f * y ) / SCRHEIGHT + 1.0f;

		r.direction = ( forward + norm_x * width * right + norm_y * height * up ).normalized();

		return r;
	}

	// Own code
	void move( vec3 v )
	{
		origin += ( right * v.x + up * v.y + forward * v.z );
	}

	// Stack overflow: https://stackoverflow.com/questions/42421611/3d-vector-rotation-in-c
	vec3 rotateVec( const vec3 &v, const vec3 &axis, float angle )
	{
		float cos_angle = cos( angle );
		float sin_angle = sin( angle );

		vec3 rotated = ( v * cos_angle ) + ( axis.cross( v ) * sin_angle ) + ( axis * axis.dot( v ) ) * ( 1 - cos_angle );

		return rotated;
	}

	// Own code
	void rotate( vec3 v )
	{
		vec3 new_forward = rotateVec( forward, up, v.x ); // Left/Right
		new_forward = rotateVec( new_forward, right, -v.y ); // Up/Down

		forward = new_forward.normalized();
		right = forward.cross( rotateVec( up, forward, v.z ) ).normalized(); // Roll
		up = right.cross( forward );
	}
};
