#ifndef _PANELMETAFILE_H_
#define _PANELMETAFILE_H_

#include "EditorUI/Panel/Panel.h"

class Metafile;
class ModelMetafile;
class PanelMetaFile : public Panel
{
public:
	PanelMetaFile();
	~PanelMetaFile() = default;

	void Render(Metafile* meta_file);
private:
	void ShowSpecializedMetaFile(Metafile* meta_file);

	void ShowModelMetaFile(ModelMetafile * metafile);

};

#endif //_PANELMETAFILE_H_
