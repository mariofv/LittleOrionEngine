#ifndef _OLOCTTREENODE_H_
#define _OLOCTTREENODE_H_

#include "GameObject.h"
#include <MathGeoLib.h>

class OLOctTreeNode
{
public:
	OLOctTreeNode();
	OLOctTreeNode(const AABB aabb);
	~OLOctTreeNode();

	void AddChild(OLOctTreeNode * new_child);
	void RemoveChild(const OLOctTreeNode * child_to_remove);
	void SetParent(OLOctTreeNode * parent);
	bool IsLeaf() const;

	void InsertGameObject(GameObject *game_object);
	void Split(std::vector<OLOctTreeNode*> &generated_nodes);

public:
	AABB box;
	OLOctTreeNode *parent = nullptr;
	std::vector<OLOctTreeNode*> children;

	std::vector<GameObject*> objects;


};

#endif //_OLOCTTREENODE_H_
