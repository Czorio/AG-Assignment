#pragma once

struct BVHNode
{
	aabb bounds;
	bool isLeaf;
	BVHNode *left, *right;
	vector<Primitive *> primitives;

	BVHNode( vector<Primitive *> primitives ) : primitives( primitives )
	{
		bounds = aabb();
		bounds.Reset();
		isLeaf = true;
		left = nullptr;
		right = nullptr;

		for ( size_t i = 0; i < primitives.size(); i++ )
		{
			bounds.Grow( primitives[i]->volume() );
		}
	}

	void calculateSAH( int &bestAxis, float &bestSplit, vector<Primitive *> &bestLeftPrims, vector<Primitive *> &bestRightPrims) 
	{
		float side1 = bounds.Extend( 0 );
		float side2 = bounds.Extend( 1 );
		float side3 = bounds.Extend( 2 );

		// Calculate the cost of the head
		float splitCost = FLT_MAX;
		float minCost = primitives.size() * ( side1 * side2 + side2 * side3 + side3 * side1 );
		float split;
		float surfaceLeft, surfaceRight;
		BVHNode *left, *right;
		vector<Primitive *> primsLeft, primsRight;
		//cout << "Cost of node: " << minCost << endl;

		// Loop over the three axis
		for ( int axis = 0; axis < 3; axis++ )
		{
			// Loop over all possible splits (primitive centroids)
			for ( const auto &p : primitives )
			{
				split = p->origin[axis];
				primsLeft.clear();
				primsRight.clear();
				// Count number of primitives in left and right nodes
				for ( const auto &prim : primitives )
				{
					if ( prim->origin[axis] <= split )
					{
						primsLeft.push_back( prim );
					}
					else
					{
						primsRight.push_back( prim );
					}
				}

				int countLeft = primsLeft.size();
				int countRight = primsRight.size();

				// Avoid useless partionings: (to be tested)
				if ( countLeft <= 1 || countRight <= 1 ) continue;

				left = new BVHNode( primsLeft );
				right = new BVHNode( primsRight );

				// Calculate sides of the left and right bounding boxes
				float lside1 = left->bounds.Extend( 0 );
				float lside2 = left->bounds.Extend( 1 );
				float lside3 = left->bounds.Extend( 2 );

				float rside1 = right->bounds.Extend( 0 );
				float rside2 = right->bounds.Extend( 1 );
				float rside3 = right->bounds.Extend( 2 );

				// Calculate surface area of left and right boxes
				float surfaceLeft = lside1*lside2 + lside2*lside3 + lside3*lside1;
				float surfaceRight = rside1*rside2 + rside2*rside3 + rside3*rside1;

				// Calculate cost of split
				splitCost = surfaceLeft * countLeft + surfaceRight * countRight;

				// Is this split better than the current minimum?
				if (splitCost < minCost) {
					minCost = splitCost;
					bestSplit = split;
					bestAxis = axis;
					bestLeftPrims = primsLeft;
					bestRightPrims = primsRight;
				}
			}
		}
	}

	void subdivide( int currentDepth )
	{
		// Conditions warrant a leaf node
		// For some reason the plain < is buggy with SAH, so <=
		if ( ( primitives.size() <= 3 ) || ( currentDepth >= BVHDEPTH ) )
		{
			return;
		}

	// http://raytracey.blogspot.com/2016/01/ , Tutorial
	// https://github.com/straaljager/GPU-path-tracing-tutorial-3/ , Code
	#ifdef USE_SAH
		int bestAxis = -1;
		float bestSplit = FLT_MAX;
		vector<Primitive *> leftPrims, rightPrims;
		this->calculateSAH( bestAxis, bestSplit, leftPrims, rightPrims ); // currently bestAxis, bestSplit not used
	#else
		// Split aabb on longest axis
		int longestAxis = bounds.LongestAxis();
		float center = bounds.Center( longestAxis );

		// Divide primitives across left and right nodes
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
	#endif // USE_SAH


		left = new BVHNode( leftPrims );
		left->subdivide( currentDepth + 1 );

		right = new BVHNode( rightPrims );
		right->subdivide( currentDepth + 1 );

		// We are no longer a leaf
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
			return h;
		}
		else
		{
			// Determine if we get hits in the child nodes
			// Also determine which hit, if left and right hits exist, is closest
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

	// Debug BVH visualizer
	vec3 debug( const Ray &r ) const
	{
		if ( rayIntersectsBounds( bounds, r ) )
		{

			if ( isLeaf )
			{
				return vec3( 0.f, 1.f / (float)BVHDEPTH, 0.f );
			}
			else
			{
				return vec3( 0.f, 1.f / (float)BVHDEPTH, 0.f ) + left->debug( r ) + right->debug( r );
			}
		}
		else
		{
			return vec3();
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

	vec3 debug( const Ray &r ) const
	{
		return head->debug( r );
	}

  private:
	BVHNode *head;
};