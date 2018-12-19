#pragma once

struct BVHNode
{
	aabb bounds;
	bool isLeaf;
	BVHNode *left, *right;
	vector<Primitive *> primitives;

	//DEBUG
	int depth;

	BVHNode( vector<Primitive *> primitives ) : primitives( primitives )
	{
		bounds = aabb();
		isLeaf = true;
		left = nullptr;
		right = nullptr;
		primitives = primitives;

		for ( size_t i = 0; i < primitives.size(); i++ )
		{
			bounds.Grow( primitives[i]->volume() );
		}
	}

	void subdivide( int currentDepth )
	{
		depth = currentDepth;
		if ( ( primitives.size() <= 3 ) || ( currentDepth >= BVHDEPTH ) )
		{
			return;
		}

		int longestAxis = bounds.LongestAxis();
		float center = bounds.Center( longestAxis );

		vec3 leftMin = vec3( bounds.Minimum( 0 ), bounds.Minimum( 1 ), bounds.Minimum( 2 ) );
		vec3 leftMax = vec3( bounds.Minimum( 0 ), bounds.Minimum( 1 ), bounds.Minimum( 2 ) );

		vec3 rightMin = vec3( bounds.Minimum( 0 ), bounds.Minimum( 1 ), bounds.Minimum( 2 ) );
		vec3 rightMax = vec3( bounds.Minimum( 0 ), bounds.Minimum( 1 ), bounds.Minimum( 2 ) );

		leftMax[longestAxis] = center;
		rightMin[longestAxis] = center;

		vector<Primitive *> leftPrims;
		vector<Primitive *> rightPrims;

		for ( Primitive *p : primitives )
		{
			if ( p->origin[longestAxis] < center )
			{
				leftPrims.push_back( p );
			}
			else
			{
				rightPrims.push_back( p );
			}
		}

		left = new BVHNode( leftPrims );
		left->subdivide( currentDepth + 1 );

		right = new BVHNode( rightPrims );
		right->subdivide( currentDepth + 1 );

		isLeaf = false;
	}

	Hit intersect( const Ray &r ) const
	{
		if ( isLeaf )
		{
			// Find closest hit in this leaf
			Hit h = Hit();
			h.hitType = 0;
			h.t = FLT_MAX;

			for ( Primitive *p : primitives )
			{
				Hit tmp = p->hit( r );
				if ( tmp.t < h.t )
				{
					h = tmp;
				}
			}

			h.bvhDepth = depth;
			return h;
		}
		else
		{
			// Determine if we get hits in the child nodes
			// Also determine which hit, if 2 exist, is closest
			Hit leftHit = Hit(), rightHit = Hit();
			leftHit.hitType = 0;
			rightHit.hitType = 0;

			if ( rayIntersectsBounds( left->bounds, r ) )
			{
				leftHit = left->intersect( r );
			}

			if ( rayIntersectsBounds( right->bounds, r ) )
			{
				rightHit = right->intersect( r );
			}

			// Both return a hit
			if ( rightHit.hitType != 0 && leftHit.hitType != 0 )
			{
				// Assumed is that neither hits are on the same t
				if ( rightHit.t < leftHit.t )
				{
					return rightHit;
				}
				else
				{
					return leftHit;
				}
			}
			// Only left
			else if ( rightHit.hitType == 0 && leftHit.hitType != 0 )
			{
				return leftHit;
			}
			// Only right
			else if ( rightHit.hitType != 0 && leftHit.hitType == 0 )
			{
				return rightHit;
			}
			// Neither
			else
			{
				// leftHit should still have hitType 0, this is fine
				return leftHit;
			}
		}
	}

  private:
	// Based on Slab method, as described on https://tavianator.com/fast-branchless-raybounding-box-intersections/
	inline bool rayIntersectsBounds( const aabb &bounds, const Ray &r ) const
	{
		float tmin = -FLT_MAX, tmax = FLT_MAX;

		for ( int i = 0; i < 3; ++i )
		{
			if ( r.direction[i] != 0.0 )
			{
				// Optimization opportunity: precalculate inverse of directions
				float t1 = ( bounds.bmin[i] - r.origin[i] ) / r.direction[i];
				float t2 = ( bounds.bmax[i] - r.origin[i] ) / r.direction[i];

				tmin = max( tmin, min( t1, t2 ) );
				tmax = min( tmax, max( t1, t2 ) );
			}
			else if ( r.origin[i] <= bounds.bmin[i] || r.origin[i] >= bounds.bmax[i] )
			{
				return false;
			}
		}

		return tmax > tmin && tmax > 0.0;
	}
};

class BVH
{
  public:
	BVH( vector<Primitive *> primitives )
	{
		constructBVH( primitives );
	}

	void constructBVH( vector<Primitive *> primitives )
	{
		head = new BVHNode( primitives );
		head->subdivide( 0 );
	}

	Hit intersect( const Ray &r ) const
	{
		return head->intersect( r );
	}

  private:
	BVHNode *head;
};