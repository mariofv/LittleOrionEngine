#ifndef _PANELMETAFILE_H_
#define _PANELMETAFILE_H_

#include "EditorUI/Panel/Panel.h"

class Metafile;
class PanelMetaFile : public Panel
{
public:
	PanelMetaFile();
	~PanelMetaFile() = default;

	void Render(Metafile* meta_file);
	//void ShowPrefabMenu(GameObject* game_object);

};

#endif //_PANELMETAFILE_H_
