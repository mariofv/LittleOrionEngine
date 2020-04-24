#ifndef _TEXTUREMETAFILE_H_
#define _TEXTUREMETAFILE_H_

#include "Metafile.h"
class TextureMetafile : public Metafile
{

public:
	TextureMetafile() = default;
	~TextureMetafile() = default;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

};

#endif