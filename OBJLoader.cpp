#include "precomp.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

// Based on example on https://github.com/syoyo/tinyobjloader
vector<Primitive *> loadOBJ( const char *filename, const Material &defaultMat )
{
	vector<Primitive *> result = vector<Primitive *>();

	tinyobj::attrib_t attributes;
	vector<tinyobj::shape_t> shapes;
	vector<tinyobj::material_t> materials;

	string warn;
	string err;

	bool ret = tinyobj::LoadObj( &attributes, &shapes, &materials, &warn, &err, filename );

	// If error, print
	if ( !err.empty() )
	{
		cerr << err << endl;
	}

	// Error
	if ( !ret )
	{
		return vector<Primitive *>();
	}

	// Loop over shapes
	for ( size_t s = 0; s < shapes.size(); s++ )
	{
		// Loop over faces
		size_t indexOffset = 0;
		for ( size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++ )
		{
			int fv = shapes[s].mesh.num_face_vertices[f];
			vec3 *verts = new vec3[fv];

			// Loop over verteces
			for ( size_t v = 0; v < fv; v++ )
			{
				// access to vertex
				tinyobj::index_t idx = shapes[s].mesh.indices[indexOffset + v];
				tinyobj::real_t vx = attributes.vertices[3 * idx.vertex_index + 0];
				tinyobj::real_t vy = attributes.vertices[3 * idx.vertex_index + 1];
				tinyobj::real_t vz = attributes.vertices[3 * idx.vertex_index + 2];
				tinyobj::real_t nx = attributes.normals[3 * idx.normal_index + 0];
				tinyobj::real_t ny = attributes.normals[3 * idx.normal_index + 1];
				tinyobj::real_t nz = attributes.normals[3 * idx.normal_index + 2];
				tinyobj::real_t tx = attributes.texcoords[2 * idx.texcoord_index + 0];
				tinyobj::real_t ty = attributes.texcoords[2 * idx.texcoord_index + 1];

				verts[v] = vec3( vx, vy, vz );

				// Optional: vertex colors
				// tinyobj::real_t red = attrib.colors[3*idx.vertex_index+0];
				// tinyobj::real_t green = attrib.colors[3*idx.vertex_index+1];
				// tinyobj::real_t blue = attrib.colors[3*idx.vertex_index+2];
			}


			indexOffset += fv;

			// per-face material
			shapes[s].mesh.material_ids[f];

			Material mat;


			result.push_back( new Triangle( mat, verts ) );
		}
	}

	return result;
}
