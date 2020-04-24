#include "TextureMetafile.h"
#include "Helper/Config.h"

void TextureMetafile::Save(Config & config) const
{
	Metafile::Save(config);
	config.AddInt(texture_type, "Type");
	config.AddBool(generate_mipmaps, "MipMaps");
}

void TextureMetafile::Load(const Config & config)
{
	Metafile::Load(config);
	texture_type = static_cast<TextureType>(config.GetInt("Type", TextureType::DEFAULT));
	generate_mipmaps = config.GetBool("MipMaps", true);
}
