#ifndef _HIERARCHY_H_
#define _HIERARCHY_H_

#include <string>

class GameObject;

class Hierarchy
{
public:
	Hierarchy();
	~Hierarchy();

	std::string GetNextGameObjectName();
	int GetNextBranch();

	void ShowHierarchyWindow();

private:
	void ShowGameObjectHierarchy(GameObject *game_object);

	void DragAndDrop(GameObject *game_object);
	void DragSource(GameObject *source_game_object);
	void DropTarget(GameObject *target_game_object);

	void ShowGameObjectActionsMenu(GameObject *game_object);
	void Show3DObjectCreationMenu();

	void ProcessMouseInput(GameObject *game_object);

public:
	GameObject *selected_game_object = nullptr;

private:
	int num_game_objects = 0;
	int branch_counter = 0;
};

#endif //_HIERARCHY_H_

