#ifndef _OLOCTTREENODE_H_
#define _OLOCTTREENODE_H_

#include "Component/ComponentCamera.h"

#include <MathGeoLib.h>

class GameObject;

class OLOctTreeNode
{
public:
	OLOctTreeNode();
	OLOctTreeNode(const AABB aabb);
	~OLOctTreeNode() = default;

	void AddChild(OLOctTreeNode * new_child);
	void RemoveChild(const OLOctTreeNode * child_to_remove);
	void SetParent(OLOctTreeNode * parent);
	bool IsLeaf() const;

	void InsertGameObject(GameObject *game_object);
	void Split(std::vector<OLOctTreeNode*> &generated_nodes);
	void DistributeGameObjectsAmongChildren();

	void CollectIntersect(std::vector<GameObject*> &game_objects, const Frustum &camera_frustum);

	std::vector<float> GetVertices() const;

public:
	AABB box;
	OLOctTreeNode *parent = nullptr;
	int depth = 0;
	std::vector<OLOctTreeNode*> children;

	std::vector<GameObject*> objects;


};

#endif //_OLOCTTREENODE_H_
