#ifndef _OLOCTTREE_H_
#define _OLOCTTREE_H_

#include "GameObject.h"
#include "OLOctTreeNode.h"

#include <MathGeoLib.h>

class OLOctTree
{
public:
	OLOctTree() = default;
	~OLOctTree();

	void Create(AABB limits);
	void Clear();
	void Insert(GameObject &game_object);
	void CollectIntersect(std::vector<GameObject*> &game_objects, const ComponentCamera &camera);

	static std::vector<float> GetVertices(const AABB &box); // TODO: Move to Utils class

public:
	std::vector<OLOctTreeNode*> flattened_tree;

private:
	void FindLeaves(const AABB &game_object, std::vector<OLOctTreeNode*> &leaves) const;

private:
	OLOctTreeNode *root = nullptr;
	const size_t bucket_size = 3;
	const size_t max_depth = 3;
};

#endif //_OLOCTTREE_H_
