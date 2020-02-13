#ifndef _HIERARCHY_H_
#define _HIERARCHY_H_

#include <string>

class GameObject;

class Hierarchy
{
public:
	Hierarchy() = default;
	~Hierarchy() = default;

	std::string GetNextGameObjectName();
	int GetNextBranch();

	void ShowHierarchyWindow();

private:
	void ShowGameObjectHierarchy(GameObject *game_object);

	void DragAndDrop(GameObject *game_object) const;
	void DragSource(GameObject *source_game_object) const;
	void DropTarget(GameObject *target_game_object) const;

	void ShowGameObjectActionsMenu(GameObject *game_object);
	void Show3DObjectCreationMenu() const;

	void ProcessMouseInput(GameObject *game_object);

public:
	GameObject *selected_game_object = nullptr;

private:
	int num_game_objects = 0;
	int branch_counter = 0;
};

#endif //_HIERARCHY_H_

