#ifndef _OLQUADTREENODE_H_
#define _OLQUADTREENODE_H_

#include <MathGeoLib.h>

class OLQuadTreeNode
{
public:
	OLQuadTreeNode();
	~OLQuadTreeNode();

	void AddChild(OLQuadTreeNode * new_child);
	void RemoveChild(const OLQuadTreeNode * child_to_remove);

	void SetParent(OLQuadTreeNode * parent);
public:
	AABB box;
	OLQuadTreeNode *parent = nullptr;
	std::vector<OLQuadTreeNode*> children;



};

#endif //_OLQUADTREENODE_H_
