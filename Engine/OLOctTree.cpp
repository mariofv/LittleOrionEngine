#include "OLOctTree.h"

#include <algorithm>

OLOctTree::OLOctTree()
{
}

OLOctTree::~OLOctTree()
{
	Clear();
}

void OLOctTree::Create(AABB limits)
{
	Clear();
	root = new OLOctTreeNode(limits);
	flattened_tree.push_back(root);
}

void OLOctTree::Clear()
{
	for (auto & node : flattened_tree)
	{
		delete node;
	}
	flattened_tree.clear();
	root = nullptr;
}

void OLOctTree::Insert(GameObject &game_object)
{
	assert(root->box.Intersects(game_object.aabb.bounding_box));

	std::vector<OLOctTreeNode*> intersecting_leaves;
	FindLeaves(game_object.aabb.bounding_box, intersecting_leaves);
	assert(intersecting_leaves.size() > 0);

	bool reinsert = false;
	for (auto &leaf : intersecting_leaves)
	{
		if (leaf->depth == max_depth)
		{
			leaf->InsertGameObject(&game_object);
		}
		else
		{
			if (leaf->objects.size() == bucket_size)
			{
				std::vector<OLOctTreeNode*> generated_nodes;
				leaf->Split(generated_nodes);
				flattened_tree.insert(flattened_tree.end(), generated_nodes.begin(), generated_nodes.end());
				reinsert = true;

			}
			else
			{
				leaf->InsertGameObject(&game_object);
			}
		}
	}

	if (reinsert)
	{
		Insert(game_object);
	}
}

void OLOctTree::CollectIntersect(std::vector<GameObject*> &game_objects, const ComponentCamera &camera)
{
	root->CollectIntersect(game_objects, camera);
	auto it = std::unique(game_objects.begin(), game_objects.end());
	game_objects.erase(it, game_objects.end());
}

void OLOctTree::FindLeaves(const AABB &game_object_aabb, std::vector<OLOctTreeNode*> &leaves) const
{
	for (auto &node : flattened_tree)
	{
		if (node->IsLeaf() && node->box.Intersects(game_object_aabb))
		{
			leaves.push_back(node);
		}
	}
}


std::vector<float> OLOctTree::GetVertices(const AABB &box)
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

