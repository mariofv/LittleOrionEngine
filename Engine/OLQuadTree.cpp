#include "OLQuadTree.h"
#include <queue>

OLQuadTree::OLQuadTree()
{
}

OLQuadTree::~OLQuadTree()
{
	Clear();
}

void OLQuadTree::Create(AABB2D limits)
{
	Clear();
	root = new OLQuadTreeNode(limits);
	flattened_tree.push_back(root);
}

void OLQuadTree::Clear()
{
	for (auto & node : flattened_tree)
	{
		delete node;
	}
	flattened_tree.clear();
	root = nullptr;
}

void OLQuadTree::Insert(GameObject &game_object)
{
	if (root->box.Intersects(game_object.aabb.bounding_box2D))
	{
		OLQuadTreeNode *object_leaf = FindLeaf(game_object.aabb.bounding_box2D); //TODO: What happens if the object is inside more one node?
		assert(object_leaf != nullptr);
		if (object_leaf->objects.size() == bucket_size)
		{
			std::vector<OLQuadTreeNode*> generated_nodes;
			object_leaf->Split(generated_nodes);
			flattened_tree.insert(flattened_tree.end(), generated_nodes.begin(), generated_nodes.end());

			bool inserted = false;
			unsigned int i = 0;
			while (!inserted && i < 4)
			{
				if (generated_nodes[i]->box.Intersects(game_object.aabb.bounding_box2D))
				{
					generated_nodes[i]->InsertGameObject(&game_object);
					inserted = true;
				}
				++i;
			}
			assert(inserted);
		}
		else
		{
			object_leaf->InsertGameObject(&game_object);
		}
	}
	//TODO: What happens if object is outside the tree?

}

void OLQuadTree::CollectIntersect(std::vector<GameObject*> &game_objects, const ComponentCamera &camera)
{

}

OLQuadTreeNode* OLQuadTree::FindLeaf(const AABB2D &game_object_aabb) const
{
	for (auto &node : flattened_tree)
	{
		if (node->IsLeaf() && node->box.Intersects(game_object_aabb))
		{
			return node;
		}
	}

	return nullptr;
}


std::vector<float> OLQuadTree::GetVertices(const AABB2D &box)
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

