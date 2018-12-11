#pragma once

struct BVHNode
{
	aabb bounds;
	bool isLeaf;
	BVHNode *left, *right;
	int first, count;

	void subdivide()
	{
		if ( count < 3 )
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
	BVH() {};

	void constructBVH( Primitive *primitives )
	{
	}

  private:
	BVHNode *head;
};