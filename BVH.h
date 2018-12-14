#pragma once

struct BVHNode
{
	aabb bounds;
	bool isLeaf;
	BVHNode *left, *right;
	vector<Primitive *> primitives;

	void subdivide( int currentDepth )
	{
		if ( primitives.size() < 3 || currentDepth >= BVHDEPTH )
		{
			return;
		}

		int longestAxis = bounds.LongestAxis();
		float center = bounds.Center( longestAxis );

		left = new BVHNode();
		left->subdivide( currentDepth + 1 );
		right = new BVHNode();
		right->subdivide( currentDepth + 1 );
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