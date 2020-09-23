#ifndef _OLQUADTREENODE_H_
#define _OLQUADTREENODE_H_

#include "Component/ComponentCamera.h"

#include <MathGeoLib.h>

class GameObject;

class OLQuadTreeNode
{
public:
	OLQuadTreeNode();
	OLQuadTreeNode(const AABB2D aabb);
	~OLQuadTreeNode() = default;

	void AddChild(OLQuadTreeNode * new_child);
	void RemoveChild(const OLQuadTreeNode * child_to_remove);
	void SetParent(OLQuadTreeNode * parent);
	bool IsLeaf() const;

	void InsertGameObject(GameObject *game_object);
	void Split(std::vector<OLQuadTreeNode*> &generated_nodes);
	void DistributeGameObjectsAmongChildren();

	void CollectIntersect(std::vector<GameObject*>& game_objects, const Frustum& camera_frustum);

	std::vector<float> GetVertices() const;

public:
	AABB2D box;
	OLQuadTreeNode *parent = nullptr;
	int depth = 0;
	std::vector<OLQuadTreeNode*> children;

	std::vector<GameObject*> objects;
};

#endif //_OLQUADTREENODE_H_
