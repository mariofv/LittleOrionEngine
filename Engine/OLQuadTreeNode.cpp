#include "OLQuadTreeNode.h"
#include <algorithm>

OLQuadTreeNode::OLQuadTreeNode()
{
	AABB2D box;
	OLQuadTreeNode *parent = nullptr;
	std::vector<OLQuadTreeNode*> children;
}

OLQuadTreeNode::OLQuadTreeNode(const AABB2D aabb) : box(aabb)
{
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

bool OLQuadTreeNode::IsLeaf() const
{
	return children.size() == 0;
}

void OLQuadTreeNode::InsertGameObject(GameObject *game_object)
{
	objects.push_back(game_object);
}

void OLQuadTreeNode::Split(std::vector<OLQuadTreeNode*> &generated_nodes)
{
	OLQuadTreeNode *bottom_left_node = new OLQuadTreeNode(AABB2D(box.minPoint, (box.maxPoint + box.minPoint)/2));

	float2 bottom_right_node_min_point = float2((box.maxPoint.x + box.minPoint.x) / 2, box.minPoint.y);
	float2 bottom_right_node_max_point = float2(box.maxPoint.x, (box.maxPoint.y + box.minPoint.y) / 2);
	OLQuadTreeNode *bottom_right_node = new OLQuadTreeNode(AABB2D(bottom_right_node_min_point, bottom_right_node_max_point));

	float2 top_left_node_min_point = float2(box.minPoint.x, (box.maxPoint.y + box.minPoint.y) / 2);
	float2 top_left_node_max_point = float2((box.maxPoint.x + box.minPoint.x) / 2, box.maxPoint.y);
	OLQuadTreeNode *top_left_node = new OLQuadTreeNode(AABB2D(top_left_node_min_point, top_left_node_max_point));

	OLQuadTreeNode *top_right_node = new OLQuadTreeNode(AABB2D((box.maxPoint + box.minPoint) / 2, box.maxPoint));

	generated_nodes.push_back(bottom_left_node);
	generated_nodes.push_back(bottom_right_node);
	generated_nodes.push_back(top_left_node);
	generated_nodes.push_back(top_right_node);

	for (auto &node : generated_nodes)
	{
		AddChild(node);
	}
}

