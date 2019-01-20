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

	void subdivide( int currentDepth )
	{
		// Conditions warrant a leaf node
		if ( ( primitives.size() < 4 ) || ( currentDepth >= BVHDEPTH ) )
		{
			return;
		}

		vector<Primitive *> leftPrims, rightPrims;

		calculateSAH( leftPrims, rightPrims );

		left = new BVHNode( leftPrims );
		left->subdivide( currentDepth + 1 );

		right = new BVHNode( rightPrims );
		right->subdivide( currentDepth + 1 );

		// We are no longer a leaf
		isLeaf = false;
		primitives.clear();
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

	// Based on PBRT book
	void calculateSAH( vector<Primitive *> &leftPrims, vector<Primitive *> &rightPrims )
	{
		// Not worth calculating over this small size
		if ( primitives.size() <= BVH_MIN_SAH_COUNT )
		{
			int longestAxis = bounds.LongestAxis();
			float center = bounds.Center( longestAxis );

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
		}
		// Else we divide using the SAH with binning
		else
		{
			float *bestSplit = new float[3];
			float *bestCost = new float[3];

			bestCost[0] = FLT_MAX;
			bestCost[1] = FLT_MAX;
			bestCost[2] = FLT_MAX;

			// Calculate best splitplanes across axes
//#pragma omp parallel for
			for ( int axis = 0; axis < 3; axis++ )
			{
				float axisLength = bounds.Extend( axis );

				// Calculate cost for each bin as splitplane
				for ( int i = 0; i < BINCOUNT - 2; i++ )
				{
					// left prim amount, right can be calculated by subtracting from total
					int primsLeft = 0;
					int primsRight = 0;
					aabb boundsLeft = aabb();
					aabb boundsRight = aabb();

					float splitPlane = bounds.Minimum( axis ) + axisLength * ( ( i + 1 ) / float( BINCOUNT ) );

					for ( Primitive *p : primitives )
					{
						if ( p->origin[axis] < splitPlane )
						{
							primsLeft++;
							boundsLeft.Grow( p->volume() );
						}
						else
						{
							primsRight++;
							boundsRight.Grow( p->volume() );
						}
					}

					// Traversal cost omitted because it is the same for all splits anyway
					float cost = ( primsLeft * boundsLeft.Area() + primsRight * boundsRight.Area() ) / bounds.Area();

					if ( cost < bestCost[axis] )
					{
						bestCost[axis] = cost;
						bestSplit[axis] = splitPlane;
					}
				}
			}

			// Determine best axis
			int bestAxis = 0;
			for ( int i = 0; i < 3; i++ )
			{
				cout << bestCost[i] << endl;
				if ( bestCost[i] < bestCost[bestAxis] )
				{
					bestAxis = i;
				}
			}


			// We now have the best splitplane, divide primitives
			for ( Primitive *p : primitives )
			{
				if ( p->origin[bestAxis] < bestSplit[bestAxis] )
				{
					leftPrims.push_back( p );
				}
				else
				{
					rightPrims.push_back( p );
				}
			}
		}
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