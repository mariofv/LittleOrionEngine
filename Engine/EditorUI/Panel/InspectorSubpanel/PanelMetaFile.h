#ifndef _PANELMETAFILE_H_
#define _PANELMETAFILE_H_

#include "EditorUI/Panel/Panel.h"

class Metafile;
class ModelMetafile;
class TextureMetafile;
enum TextureType;
enum WrapMode;
enum FilterMode;

class PanelMetaFile : public Panel
{
public:
	PanelMetaFile();
	~PanelMetaFile() = default;

	void Render(Metafile* meta_file);
	void ApplyMetafileChanges(Metafile * metafile);
private:
	void ShowSpecializedMetaFile(Metafile* meta_file);

	void ShowModelMetaFile(ModelMetafile * metafile);
	void ShowTextureMetaFile(TextureMetafile * metafile);

	std::string GetTextureTypeName(TextureType texture_type_id) const;

	std::string GetTextureFilterModeName(FilterMode filter_mode) const;

	std::string GetTextureWrapModeName(WrapMode wrap_mode) const;

};

#endif //_PANELMETAFILE_H_
