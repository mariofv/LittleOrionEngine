#ifndef _OLQUADTREE_H_
#define _OLQUADTREE_H_

#include "GameObject.h"

#include <MathGeoLib.h>

class OLQuadTree
{
public:
	OLQuadTree();
	~OLQuadTree();

	void Create(AABB limits);
	void Clear();
	void Insert(const GameObject &game_object);
	void CollectIntersect(std::vector<GameObject*> &game_objects, const ComponentCamera &camera);

};

#endif _OLQUADTREE_H_
