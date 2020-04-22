#ifndef _PANELHIERARCHY_H_
#define _PANELHIERARCHY_H_

#include "Panel.h"
#include <string>

class GameObject;

class PanelHierarchy : public Panel
{
public:
	PanelHierarchy();
	~PanelHierarchy() = default;

	void Render() override;

	std::string GetNextGameObjectName();
	int GetNextBranch();

private:
	void ShowGameObjectHierarchy(GameObject *game_object);

	void DragAndDrop(GameObject *game_object) const;
	void DragSource(GameObject *source_game_object) const;
	void DropTarget(GameObject *target_game_object) const;

	void ShowGameObjectActionsMenu(GameObject *game_object);
	void Show3DObjectCreationMenu(GameObject *game_object) const;
	void ShowComponentObjectCreationMenu(GameObject *game_object) const;

	void ProcessMouseInput(GameObject *game_object);

private:
	int num_game_objects = 0;
	int branch_counter = 0;
};

#endif //_PANELHIERARCHY_H_

