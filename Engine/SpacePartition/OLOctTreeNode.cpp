#include "OLOctTreeNode.h"

#include "Main/GameObject.h"

#include <algorithm>

OLOctTreeNode::OLOctTreeNode()
{
	box = AABB();
	parent = nullptr;
}

OLOctTreeNode::OLOctTreeNode(const AABB aabb) : box(aabb)
{
	parent = nullptr;
}


void OLOctTreeNode::AddChild(OLOctTreeNode * new_child)
{
	new_child->parent = this;
	children.push_back(new_child);
}

void OLOctTreeNode::RemoveChild(const OLOctTreeNode * child_to_remove)
{
	const auto it = std::remove_if(children.begin(), children.end(), [child_to_remove](auto const& child)
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
	float3 box_center = box.CenterPoint();
	float3 box_min_point = box.minPoint;
	float3 box_max_point = box.maxPoint;

	// FIRST FLOOR
	OLOctTreeNode *first_floor_bottom_left_node = new OLOctTreeNode(AABB(box_min_point, box_center));
	first_floor_bottom_left_node->depth = depth + 1;
	
	float3 first_floor_bottom_right_node_min_point = float3(box_center.x, box_min_point.y, box_min_point.z);
	float3 first_floor_bottom_right_node_max_point = float3(box_max_point.x, box_center.y, box_center.z);
	OLOctTreeNode *first_floor_bottom_right_node = new OLOctTreeNode(AABB(first_floor_bottom_right_node_min_point, first_floor_bottom_right_node_max_point));
	first_floor_bottom_right_node->depth = depth + 1;

	float3 first_floor_top_left_node_min_point = float3(box_min_point.x, box_min_point.y, box_center.z);
	float3 first_floor_top_left_node_max_point = float3(box_center.x, box_center.y, box_max_point.z);
	OLOctTreeNode *first_floor_top_left_node = new OLOctTreeNode(AABB(first_floor_top_left_node_min_point, first_floor_top_left_node_max_point));
	first_floor_top_left_node->depth = depth + 1;

	float3 first_floor_top_right_node_min_point = float3(box_center.x, box_min_point.y, box_center.z);
	float3 first_floor_top_right_node_max_point = float3(box_max_point.x, box_center.y, box_max_point.z);
	OLOctTreeNode *first_floor_top_right_node = new OLOctTreeNode(AABB(first_floor_top_right_node_min_point, first_floor_top_right_node_max_point));
	first_floor_top_right_node->depth = depth + 1;

	// SECOND FLOOR
	float3 second_floor_bottom_left_node_min_point = float3(box_min_point.x, box_center.y, box_min_point.z);
	float3 second_floor_bottom_left_node_max_point = float3(box_center.x, box_max_point.y, box_center.z);
	OLOctTreeNode *second_floor_bottom_left_node = new OLOctTreeNode(AABB(second_floor_bottom_left_node_min_point, second_floor_bottom_left_node_max_point));
	second_floor_bottom_left_node->depth = depth + 1;

	float3 second_floor_bottom_right_node_min_point = float3(box_center.x, box_center.y, box_min_point.z);
	float3 second_floor_bottom_right_node_max_point = float3(box_max_point.x, box_max_point.y, box_center.z);
	OLOctTreeNode *second_floor_bottom_right_node = new OLOctTreeNode(AABB(second_floor_bottom_right_node_min_point, second_floor_bottom_right_node_max_point));
	second_floor_bottom_right_node->depth = depth + 1;

	float3 second_floor_top_left_node_min_point = float3(box_min_point.x, box_center.y, box_center.z);
	float3 second_floor_top_left_node_max_point = float3(box_center.x, box_max_point.y, box_max_point.z);
	OLOctTreeNode *second_floor_top_left_node = new OLOctTreeNode(AABB(second_floor_top_left_node_min_point, second_floor_top_left_node_max_point));
	second_floor_top_left_node->depth = depth + 1;

	OLOctTreeNode *second_floor_top_right_node = new OLOctTreeNode(AABB(box_center, box_max_point));
	second_floor_top_right_node->depth = depth + 1;


	generated_nodes.push_back(first_floor_bottom_left_node);
	generated_nodes.push_back(first_floor_bottom_right_node);
	generated_nodes.push_back(first_floor_top_left_node);
	generated_nodes.push_back(first_floor_top_right_node);

	generated_nodes.push_back(second_floor_bottom_left_node);
	generated_nodes.push_back(second_floor_bottom_right_node);
	generated_nodes.push_back(second_floor_top_left_node);
	generated_nodes.push_back(second_floor_top_right_node);
	
	for (const auto& node : generated_nodes)
	{
		AddChild(node);
	}
	DistributeGameObjectsAmongChildren();
}

void OLOctTreeNode::DistributeGameObjectsAmongChildren()
{
	for (const auto& game_object : objects)
	{
		for (unsigned int i = 0; i < 8; ++i)
		{
			if (children[i]->box.Intersects(game_object->aabb.bounding_box))
			{
				children[i]->InsertGameObject(game_object);
			}
		}
	}
	objects.clear();
	assert(objects.size() == 0);
}

void OLOctTreeNode::CollectIntersect(std::vector<GameObject*>& game_objects, const Frustum& camera_frustum)
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

std::vector<float> OLOctTreeNode::GetVertices() const
{
	static const int num_of_vertices = 8;
	float3 tmp_vertices[num_of_vertices];
	box.GetCornerPoints(&tmp_vertices[0]);

	std::vector<float> vertices(num_of_vertices * 3);
	for (unsigned int i = 0; i < num_of_vertices; ++i)
	{
		vertices[i * 3] = tmp_vertices[i].x;
		vertices[i * 3 + 1] = tmp_vertices[i].y;
		vertices[i * 3 + 2] = tmp_vertices[i].z;
	}

	return vertices;
}

