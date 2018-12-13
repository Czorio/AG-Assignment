#pragma once

struct BVHNode
{
	aabb bounds;
	bool isLeaf;
	BVHNode *left, *right;
	vector<Primitive *> primitives;

	void subdivide( int currentDepth )
	{
		if ( primitives.size() < 3 || currentDepth >= BVHDEPTH)
		{
			return;
		}

		left = new BVHNode();
		right = new BVHNode();
	}
};

class BVH
{
  public:
	BVH(){};

	void constructBVH( vector<Primitive *> primitives )
	{
		head = new BVHNode();
		head->bounds = aabb();
		head->isLeaf = true;
		head->left = nullptr;
		head->right = nullptr;
		head->primitives = primitives;

		for ( size_t i = 0; i < head->primitives.size(); i++ )
		{
			head->bounds.Grow( head->primitives[i]->volume() );
		}

		head->subdivide( 0 );
	}

	Hit intersect( const Ray &r )
	{
	}

  private:
	BVHNode *head;
};