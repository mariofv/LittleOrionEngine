#include "OLOctTreeNode.h"
#include <algorithm>

OLOctTreeNode::OLOctTreeNode()
{
	AABB box;
	OLOctTreeNode *parent = nullptr;
	std::vector<OLOctTreeNode*> children;
}

OLOctTreeNode::OLOctTreeNode(const AABB aabb) : box(aabb)
{
	OLOctTreeNode *parent = nullptr;
	std::vector<OLOctTreeNode*> children;
}



OLOctTreeNode::~OLOctTreeNode()
{

}

void OLOctTreeNode::AddChild(OLOctTreeNode * new_child)
{
	new_child->parent = this;
	children.push_back(new_child);
}

void OLOctTreeNode::RemoveChild(const OLOctTreeNode * child_to_remove)
{
	auto it = std::remove_if(children.begin(), children.end(), [child_to_remove](auto const & child)
	{
		return child == child_to_remove;
	});
	children.erase(it);
}

void OLOctTreeNode::SetParent(OLOctTreeNode * new_parent)
{
	new_parent->children.push_back(this);
	if (this->parent != nullptr) {
		this->parent->RemoveChild(this);
	}
	this->parent = new_parent;
}

bool OLOctTreeNode::IsLeaf() const
{
	return children.size() == 0;
}

void OLOctTreeNode::InsertGameObject(GameObject *game_object)
{
	objects.push_back(game_object);
}

void OLOctTreeNode::Split(std::vector<OLOctTreeNode*> &generated_nodes)
{

}
