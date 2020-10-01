#ifndef _OLAABBTREE_H_
#define _OLAABBTREE_H_

#include "Component/ComponentCamera.h"
#include <MathGeoLib.h>
#include <map>
#include <vector>

class GameObject;

const unsigned AABB_NULL_NODE =  0xffffffff;

struct NodeAABB
{
	AABB aabb;
	GameObject* go = nullptr;
	// tree links
	unsigned parent_node_index = AABB_NULL_NODE;
	unsigned left_node_index = AABB_NULL_NODE;
	unsigned right_node_index = AABB_NULL_NODE;
	// node linked list link
	unsigned next_node_index = AABB_NULL_NODE;

	bool isLeaf() const { return left_node_index == AABB_NULL_NODE; }

};

class OLAABBTree
{
public:
	OLAABBTree(unsigned initialSize);
	~OLAABBTree();

	void Insert(GameObject* go);
	void Remove(GameObject* go);
	void UpdateObject(GameObject* go);

	void GetIntersection(std::vector<GameObject*> &intersection_go, const Frustum& camera_frustum) const;

	void Draw() const;

	//Tree
	std::map<GameObject*, unsigned> object_node_index_map;
	std::vector<NodeAABB> nodes;
	unsigned root_node_index = AABB_NULL_NODE;
	unsigned allocated_node_count = 0;
	unsigned next_free_node_index = 0;
	unsigned node_capacity = 0;
	unsigned growth_size = 10;


private:
	AABB MergeAABB(const AABB &first, const AABB &second) const;
	unsigned AllocateNode();
	void DeallocateNode(unsigned nodeIndex);
	void InsertLeaf(unsigned leafNodeIndex);
	void FixUpwardsTree(unsigned treeNodeIndex);
	void RemoveLeaf(unsigned leafNodeIndex);
	void UpdateLeaf(unsigned leafNodeIndex, const AABB& newAaab);

	bool ValidNodeLeaf(unsigned leafNodeIndex);
};

#endif _OLAABBTREE_H_
