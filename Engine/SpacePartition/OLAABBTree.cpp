#include "OLAABBTree.h"
#include <assert.h>
#include <stack>
#include "UI/DebugDraw.h"


OLAABBTree::OLAABBTree(unsigned initial_size)
{
	node_capacity = initial_size;
	//Resize
	nodes.resize(initial_size);
	for (unsigned node_index = 0; node_index < initial_size; ++node_index)
	{
		NodeAABB &node = nodes[node_index];
		node.next_node_index = node_index + 1;
	}
	nodes[initial_size - 1].next_node_index = AABB_NULL_NODE;
}


OLAABBTree::~OLAABBTree()
{
	nodes.clear();
	object_node_index_map.clear();
}


unsigned OLAABBTree::AllocateNode()
{
	// if we have no free tree nodes then grow the pool
	if (next_free_node_index == AABB_NULL_NODE)
	{
		assert(allocated_node_count == node_capacity);

		node_capacity += growth_size;
		nodes.resize(node_capacity);
		nodes[allocated_node_count - 1].next_node_index = allocated_node_count;
		for (unsigned nodeIndex = allocated_node_count; nodeIndex < node_capacity; ++nodeIndex)
		{
			NodeAABB& node = nodes[nodeIndex];
			node.next_node_index = nodeIndex + 1;
		}
		nodes[node_capacity - 1].next_node_index = AABB_NULL_NODE;
		next_free_node_index = allocated_node_count;
	}

	unsigned nodeIndex = next_free_node_index;
	NodeAABB& allocatedNode = nodes[nodeIndex];
	next_free_node_index = allocatedNode.next_node_index;
	++allocated_node_count;

	return nodeIndex;
}

void OLAABBTree::DeallocateNode(unsigned nodeIndex)
{
	NodeAABB& deallocateNode = nodes[nodeIndex];
	deallocateNode.next_node_index = next_free_node_index;
	next_free_node_index = nodeIndex;
	allocated_node_count--;

	return;
}

void OLAABBTree::Insert(GameObject * go)
{
	assert(go != nullptr);

	//TODO: aabbtree/quadtree only contain meshes GO?

	unsigned nodeIndex = AllocateNode();
	NodeAABB& node = nodes[nodeIndex];

	//AABB a bit bigger for avoiding little movements
	AABB auxAABB = AABB(go->aabb.bounding_box.minPoint - float3(2, 2, 2), go->aabb.bounding_box.maxPoint + float3(2, 2, 2));
	node.aabb = auxAABB;
	node.go = go;

	InsertLeaf(nodeIndex);
	object_node_index_map[go] = nodeIndex;

	return;
}

void OLAABBTree::InsertLeaf(unsigned leafNodeIndex)
{
	// make sure we're inserting a new leaf
	assert(nodes[leafNodeIndex].parent_node_index == AABB_NULL_NODE);
	assert(nodes[leafNodeIndex].left_node_index == AABB_NULL_NODE);
	assert(nodes[leafNodeIndex].right_node_index == AABB_NULL_NODE);


	//if the tree is empty then we make the root the leaf
	if (root_node_index == AABB_NULL_NODE)
	{
		root_node_index = leafNodeIndex;
		return;
	}


	// search for the best place to put the new leaf in the tree
	// we use surface area and depth as search heuristics
	unsigned treeNodeIndex = root_node_index;
	NodeAABB& leafNode = nodes[leafNodeIndex];
	while (!nodes[treeNodeIndex].isLeaf())
	{
		// because of the test in the while loop above we know we are never a leaf inside it
		const NodeAABB& treeNode = nodes[treeNodeIndex];
		unsigned left_node_index = treeNode.left_node_index;
		unsigned right_node_index = treeNode.right_node_index;
		const NodeAABB& leftNode = nodes[left_node_index];
		const NodeAABB& rightNode = nodes[right_node_index];

		AABB combinedAabb = MergeAABB(treeNode.aabb, leafNode.aabb);

		float newParentNodeCost = 2.0f * combinedAabb.SurfaceArea();
		float minimumPushDownCost = 2.0f * (combinedAabb.SurfaceArea() - treeNode.aabb.SurfaceArea());

		// use the costs to figure out whether to create a new parent here or descend
		float costLeft;
		float costRight;
		if (leftNode.isLeaf())
		{
			costLeft = MergeAABB(leftNode.aabb, leafNode.aabb).SurfaceArea() + minimumPushDownCost;
		}
		else
		{
			AABB newLeftAabb = MergeAABB(leftNode.aabb, leafNode.aabb);
			costLeft = (newLeftAabb.SurfaceArea() - leftNode.aabb.SurfaceArea()) + minimumPushDownCost;
		}
		if (rightNode.isLeaf())
		{
			costRight = MergeAABB(rightNode.aabb, leafNode.aabb).SurfaceArea() + minimumPushDownCost;
		}
		else
		{
			AABB newRightAabb = MergeAABB(rightNode.aabb, leafNode.aabb);
			costRight = (newRightAabb.SurfaceArea() - rightNode.aabb.SurfaceArea()) + minimumPushDownCost;
		}

		// if the cost of creating a new parent node here is less than descending in either direction then
		// we know we need to create a new parent node, errrr, here and attach the leaf to that
		if (newParentNodeCost < costLeft && newParentNodeCost < costRight)
		{
			break;
		}

		// otherwise descend in the cheapest direction
		if (costLeft < costRight)
		{
			treeNodeIndex = left_node_index;
		}
		else
		{
			treeNodeIndex = right_node_index;
		}
	}

	// the leafs sibling is going to be the node we found above and we are going to create a new
	// parent node and attach the leaf and this item

	unsigned newParentIndex = AllocateNode();
	NodeAABB& newParent = nodes[newParentIndex];
	unsigned leafSiblingIndex = treeNodeIndex;
	NodeAABB& leafSibling = nodes[leafSiblingIndex];
	unsigned oldParentIndex = leafSibling.parent_node_index;
	newParent.parent_node_index = oldParentIndex;
	newParent.aabb = MergeAABB(leafNode.aabb, leafSibling.aabb); // the new parents aabb is the leaf aabb combined with it's siblings aabb
	newParent.left_node_index = leafSiblingIndex;
	newParent.right_node_index = leafNodeIndex;
	nodes[leafNodeIndex].parent_node_index = newParentIndex;
	//TODO: this was the source of the error, leafNode didnt change nodes[X] if x is 9, wonder why
	//leafNode.parent_node_index = newParentIndex;
	leafSibling.parent_node_index = newParentIndex;

	if (oldParentIndex == AABB_NULL_NODE)
	{
		// the old parent was the root and so this is now the root
		root_node_index = newParentIndex;
	}
	else
	{
		// the old parent was not the root and so we need to patch the left or right index to
		// point to the new node
		NodeAABB& oldParent = nodes[oldParentIndex];
		if (oldParent.left_node_index == leafSiblingIndex)
		{
			oldParent.left_node_index = newParentIndex;
		}
		else
		{
			oldParent.right_node_index = newParentIndex;
		}
	}

	// finally we need to walk back up the tree fixing heights and areas
	//treeNodeIndex = leafNode.parent_node_index;
	treeNodeIndex = nodes[leafNodeIndex].parent_node_index;
	FixUpwardsTree(treeNodeIndex);

	return;
}

void OLAABBTree::Remove(GameObject * go)
{
	unsigned nodeIndex = object_node_index_map[go];
	RemoveLeaf(nodeIndex);
	DeallocateNode(nodeIndex);
	object_node_index_map.erase(go);

	return;
}


void OLAABBTree::RemoveLeaf(unsigned leafNodeIndex)
{
	// if the leaf is the root then we can just clear the root pointer and return
	if (leafNodeIndex == root_node_index)
	{
		root_node_index = AABB_NULL_NODE;
		return;
	}

	NodeAABB& leafNode = nodes[leafNodeIndex];
	unsigned parent_node_index = leafNode.parent_node_index;
	const NodeAABB& parentNode = nodes[parent_node_index];
	unsigned grandparent_node_index = parentNode.parent_node_index;
	unsigned siblingNodeIndex = parentNode.left_node_index == leafNodeIndex ? parentNode.right_node_index : parentNode.left_node_index;
	assert(siblingNodeIndex != AABB_NULL_NODE); // we must have a sibling
	NodeAABB& siblingNode = nodes[siblingNodeIndex];

	if (grandparent_node_index != AABB_NULL_NODE)
	{
		// if we have a grand parent (i.e. the parent is not the root) then destroy the parent and connect the sibling to the grandparent in its
		// place
		NodeAABB& grandParentNode = nodes[grandparent_node_index];
		if (grandParentNode.left_node_index == parent_node_index)
		{
			grandParentNode.left_node_index = siblingNodeIndex;
		}
		else
		{
			grandParentNode.right_node_index = siblingNodeIndex;
		}
		siblingNode.parent_node_index = grandparent_node_index;
		DeallocateNode(parent_node_index);

		FixUpwardsTree(grandparent_node_index);
	}
	else
	{
		// if we have no grandparent then the parent is the root and so our sibling becomes the root and has it's parent removed
		root_node_index = siblingNodeIndex;
		siblingNode.parent_node_index = AABB_NULL_NODE;
		DeallocateNode(parent_node_index);
	}

	leafNode.parent_node_index = AABB_NULL_NODE;

	return;
}

void OLAABBTree::UpdateObject(GameObject * go)
{
	unsigned nodeIndex = object_node_index_map[go];
	UpdateLeaf(nodeIndex, go->aabb.bounding_box);

	return;
}

void OLAABBTree::UpdateLeaf(unsigned leafNodeIndex, const AABB & newAaab)
{
	NodeAABB& node = nodes[leafNodeIndex];

	// if the node contains the new aabb then we just leave things
	if (node.aabb.Contains(newAaab)) return;

	RemoveLeaf(leafNodeIndex);
	//AABB a bit bigger for avoiding little movements
	node.aabb = AABB(newAaab.minPoint - float3(2, 2, 2), newAaab.maxPoint + float3(2, 2, 2));
	InsertLeaf(leafNodeIndex);

	return;
}

void OLAABBTree::GetIntersection(std::vector<GameObject*>& intersectionGO, const ComponentCamera* camera) const
{

	AABB bbox = camera->GetMinimalEnclosingAABB();

	//DFS simulating recursivity using stack

	std::stack<unsigned> stack;
	stack.push(root_node_index);
	while (!stack.empty())
	{
		unsigned nodeIndex = stack.top();
		stack.pop();

		if (nodeIndex == AABB_NULL_NODE)
			continue;

		const NodeAABB& node = nodes[nodeIndex];
		if (bbox.Intersects(node.aabb))
		{
			if (node.isLeaf())
			{
				if(node.go->IsVisible(*camera))
					intersectionGO.push_back(node.go);
			}
			else
			{
				stack.push(node.left_node_index);
				stack.push(node.right_node_index);
			}
		}
	}

	return;
}

void OLAABBTree::Draw() const
{
	if (root_node_index == AABB_NULL_NODE)
		return;

	std::stack<NodeAABB> nodeStack;
	nodeStack.push(nodes[root_node_index]);
	while (!nodeStack.empty())
	{
		NodeAABB node = nodeStack.top();
		nodeStack.pop();

		if (node.parent_node_index != AABB_NULL_NODE)
		{
			NodeAABB parent = nodes[node.parent_node_index];
			dd::line(node.aabb.CenterPoint(), parent.aabb.CenterPoint(), float3(1.0f, 0.0f, 0.0f));
		}
		if (node.left_node_index != AABB_NULL_NODE)
			nodeStack.push(nodes[node.left_node_index]);
		if (node.right_node_index != AABB_NULL_NODE)
			nodeStack.push(nodes[node.right_node_index]);

		dd::aabb(node.aabb.minPoint, node.aabb.maxPoint, float3(1.0f, 0.0f, 0.0f));

	}

	return;
}

AABB OLAABBTree::MergeAABB(const AABB & first, const AABB & second) const
{
	return AABB(float3(Min(first.minPoint, second.minPoint)), float3(Max(first.maxPoint, second.maxPoint)));
}




void OLAABBTree::FixUpwardsTree(unsigned treeNodeIndex)
{
	while (treeNodeIndex != AABB_NULL_NODE)
	{
		NodeAABB& treeNode = nodes[treeNodeIndex];

		// every node should be a parent
		assert(treeNode.left_node_index != AABB_NULL_NODE && treeNode.right_node_index != AABB_NULL_NODE);

		// fix height and area
		const NodeAABB& leftNode = nodes[treeNode.left_node_index];
		const NodeAABB& rightNode = nodes[treeNode.right_node_index];
		treeNode.aabb = MergeAABB(rightNode.aabb, leftNode.aabb);

		treeNodeIndex = treeNode.parent_node_index;


		if (treeNode.parent_node_index == treeNode.left_node_index || treeNode.parent_node_index == treeNode.right_node_index || treeNode.left_node_index == treeNode.right_node_index)
		{
			return;
		}
	}

	return;
}





bool OLAABBTree::ValidNodeLeaf(unsigned leafNodeIndex)
{
	NodeAABB& leafNode = nodes[leafNodeIndex];
	//assert(leafNode.isLeaf());

	std::stack<NodeAABB> nodeStack;
	nodeStack.push(nodes[root_node_index]);
	int counter = 0;
	std::stack<unsigned>nodeIndexes;
	nodeIndexes.push(root_node_index);
	while (!nodeStack.empty())
	{
		NodeAABB node = nodeStack.top();
		nodeStack.pop();



		if (node.left_node_index == leafNodeIndex)
		{
			++counter;
			if (leafNode.parent_node_index != nodeIndexes.top())
				return false;

		}

		if (node.right_node_index == leafNodeIndex)
		{
			++counter;
			if (leafNode.parent_node_index != nodeIndexes.top())
				return false;
		}

		nodeIndexes.pop();

		if (node.left_node_index != AABB_NULL_NODE)
		{
			nodeStack.push(nodes[node.left_node_index]);
			nodeIndexes.push(node.left_node_index);
		}
		if (node.right_node_index != AABB_NULL_NODE)
		{
			nodeStack.push(nodes[node.right_node_index]);
			nodeIndexes.push(node.right_node_index);
		}


	}

	if (counter > 1)
		return false;

	return true;
}
