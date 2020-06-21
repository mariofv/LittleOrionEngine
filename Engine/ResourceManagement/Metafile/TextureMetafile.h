#ifndef _TEXTUREMETAFILE_H_
#define _TEXTUREMETAFILE_H_

#include "Metafile.h"

enum TextureType
{
	DEFAULT,
	NORMAL,
	TOTAL
};
enum WrapMode
{
	REPEAT,
	CLAMP,
	MIRROR,
	MIRROR_ONCE
};

enum FilterMode
{
	NEAREST,
	LINEAR
};
struct TextureOptions
{
	TextureType texture_type = TextureType::DEFAULT;
	WrapMode wrap_mode = WrapMode::REPEAT;
	FilterMode filter_mode = FilterMode::LINEAR;
	bool generate_mipmaps = true;

};
class TextureMetafile : public Metafile
{

public:
	TextureMetafile() = default;
	~TextureMetafile() = default;

	void Save(Config& config) const override;
	void Load(const Config& config) override;

	TextureOptions texture_options;
};

#endif