#ifndef _PANELMETAFILE_H_
#define _PANELMETAFILE_H_

#include "EditorUI/Panel/Panel.h"

class Metafile;
class ModelMetafile;
class TextureMetafile;
enum TextureType;
class PanelMetaFile : public Panel
{
public:
	PanelMetaFile();
	~PanelMetaFile() = default;

	void Render(Metafile* meta_file);
private:
	void ShowSpecializedMetaFile(Metafile* meta_file);

	void ShowModelMetaFile(ModelMetafile * metafile);
	void ShowTextureMetaFile(TextureMetafile * metafile);

	std::string GetTextureTypeName(TextureType texture_type_id) const;

};

#endif //_PANELMETAFILE_H_
