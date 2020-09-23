#ifndef _OLOCTTREE_H_
#define _OLOCTTREE_H_

#include "OLOctTreeNode.h"

#include <MathGeoLib.h>

class GameObject;

class OLOctTree
{
public:
	OLOctTree() = default;
	~OLOctTree();

	void Create(AABB limits);
	void Clear();
	void Insert(GameObject &game_object);
	void CollectIntersect(std::vector<GameObject*> &game_objects, const Frustum& camera_frustum);


public:
	std::vector<OLOctTreeNode*> flattened_tree;

private:
	void FindLeaves(const AABB &game_object, std::vector<OLOctTreeNode*> &leaves) const;

private:
	OLOctTreeNode* root = nullptr;
	int bucket_size = 3;
	int max_depth = 3;


	friend class PanelConfiguration;

};

#endif //_OLOCTTREE_H_
