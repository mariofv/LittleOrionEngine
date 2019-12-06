#ifndef _OLQUADTREENODE_H_
#define _OLQUADTREENODE_H_

#include "GameObject.h"
#include <MathGeoLib.h>

class OLQuadTreeNode
{
public:
	OLQuadTreeNode();
	OLQuadTreeNode(const AABB aabb);
	~OLQuadTreeNode();

	void AddChild(OLQuadTreeNode * new_child);
	void RemoveChild(const OLQuadTreeNode * child_to_remove);
	void SetParent(OLQuadTreeNode * parent);
	bool IsLeaf() const;

	void InsertGameObject(GameObject *game_object);
	void Split(std::vector<OLQuadTreeNode*> &generated_nodes);

public:
	AABB box;
	OLQuadTreeNode *parent = nullptr;
	std::vector<OLQuadTreeNode*> children;

	std::vector<GameObject*> objects;


};

#endif //_OLQUADTREENODE_H_
