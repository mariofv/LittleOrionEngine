#include "OLQuadTreeNode.h"

#include "Main/GameObject.h"

#include <algorithm>

OLQuadTreeNode::OLQuadTreeNode()
{
	box = AABB2D();
	parent = nullptr;
}

OLQuadTreeNode::OLQuadTreeNode(const AABB2D aabb) : box(aabb)
{
	parent = nullptr;
}


void OLQuadTreeNode::AddChild(OLQuadTreeNode * new_child)
{
	new_child->parent = this;
	children.push_back(new_child);
}

void OLQuadTreeNode::RemoveChild(const OLQuadTreeNode * child_to_remove)
{
	const auto it = std::remove_if(children.begin(), children.end(), [child_to_remove](auto const& child)
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
	bottom_left_node->depth = depth + 1;

	float2 bottom_right_node_min_point = float2((box.maxPoint.x + box.minPoint.x) / 2, box.minPoint.y);
	float2 bottom_right_node_max_point = float2(box.maxPoint.x, (box.maxPoint.y + box.minPoint.y) / 2);
	OLQuadTreeNode *bottom_right_node = new OLQuadTreeNode(AABB2D(bottom_right_node_min_point, bottom_right_node_max_point));
	bottom_right_node->depth = depth + 1;

	float2 top_left_node_min_point = float2(box.minPoint.x, (box.maxPoint.y + box.minPoint.y) / 2);
	float2 top_left_node_max_point = float2((box.maxPoint.x + box.minPoint.x) / 2, box.maxPoint.y);
	OLQuadTreeNode *top_left_node = new OLQuadTreeNode(AABB2D(top_left_node_min_point, top_left_node_max_point));
	top_left_node->depth = depth + 1;

	OLQuadTreeNode *top_right_node = new OLQuadTreeNode(AABB2D((box.maxPoint + box.minPoint) / 2, box.maxPoint));
	top_right_node->depth = depth + 1;

	generated_nodes.push_back(bottom_left_node);
	generated_nodes.push_back(bottom_right_node);
	generated_nodes.push_back(top_left_node);
	generated_nodes.push_back(top_right_node);

	for (const auto& node : generated_nodes)
	{
		AddChild(node);
	}
	DistributeGameObjectsAmongChildren();
}

void OLQuadTreeNode::DistributeGameObjectsAmongChildren()
{
	for (const auto& game_object : objects)
	{
		for (unsigned int i = 0; i < 4; ++i)
		{
			if (children[i]->box.Intersects(game_object->aabb.bounding_box2D))
			{
				children[i]->InsertGameObject(game_object);
			}
		}
	}
	objects.clear();
	assert(objects.size() == 0);
}

void OLQuadTreeNode::CollectIntersect(std::vector<GameObject*>& game_objects, const Frustum& camera_frustum)
{
	if (ComponentCamera::IsInsideFrustum(camera_frustum, box))
	{
		for (const auto& object : objects)
		{
			if (object->IsVisible(camera_frustum))
			{
				game_objects.push_back(object);
			}
		}
		for (const auto& child : children)
		{
			child->CollectIntersect(game_objects, camera_frustum);
		}
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

