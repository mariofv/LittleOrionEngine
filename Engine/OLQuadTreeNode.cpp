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

std::vector<float> OLQuadTreeNode::GetVertices() const
{
	static const int num_of_vertices = 4;
	float3 tmp_vertices[num_of_vertices];
	float2 max_point = box.maxPoint;
	float2 min_point = box.minPoint;
	//ClockWise from Top left
	tmp_vertices[0] = float3(min_point.x, max_point.y, 0.0f); // 0
	tmp_vertices[1] = float3(max_point, 0.0f); // 1
	tmp_vertices[2] = float3(max_point.x, min_point.y, 0.0f); // 2
	tmp_vertices[3] = float3(min_point, 0.0f); // 3

	std::vector<float> vertices(num_of_vertices * 3);
	for (unsigned int i = 0; i < num_of_vertices; ++i)
	{
		vertices[i * 3] = tmp_vertices[i].x;
		vertices[i * 3 + 1] = tmp_vertices[i].z;
		vertices[i * 3 + 2] = tmp_vertices[i].y;
	}

	return vertices;
}

