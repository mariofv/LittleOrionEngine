#include "OLQuadTreeNode.h"
#include <algorithm>

OLQuadTreeNode::OLQuadTreeNode()
{
	AABB box;
	OLQuadTreeNode *parent = nullptr;
	std::vector<OLQuadTreeNode*> children;
}


OLQuadTreeNode::~OLQuadTreeNode()
{

}

void OLQuadTreeNode::AddChild(OLQuadTreeNode * new_child)
{
	new_child->parent = this;
	children.push_back(new_child);
}
void OLQuadTreeNode::RemoveChild(const OLQuadTreeNode * child_to_remove)
{
	auto it = std::remove_if(children.begin(), children.end(), [child_to_remove](auto const & child)
	{
		return child == child_to_remove;
	});
	children.erase(it);
}

void OLQuadTreeNode::SetParent(OLQuadTreeNode * new_parent)
{
	new_parent->children.push_back(this);
	if (this->parent != nullptr) {
		this->parent->RemoveChild(this);
	}
	this->parent = new_parent;
}