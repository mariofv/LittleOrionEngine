#include "TextureMetafile.h"
#include "Helper/Config.h"

void TextureMetafile::Save(Config & config) const
{
	Metafile::Save(config);
	config.AddInt(texture_options.texture_type, "Type");
	config.AddInt(texture_options.wrap_mode, "Wrap");
	config.AddInt(texture_options.filter_mode, "Filter");
	config.AddBool(texture_options.generate_mipmaps, "MipMaps");
}

void TextureMetafile::Load(const Config & config)
{
	Metafile::Load(config);
	texture_options.texture_type = static_cast<TextureType>(config.GetInt("Type", TextureType::DEFAULT));
	texture_options.wrap_mode = static_cast<WrapMode>(config.GetInt("Wrap", WrapMode::REPEAT));
	texture_options.filter_mode = static_cast<FilterMode>(config.GetInt("Filter", FilterMode::LINEAR));
	texture_options.generate_mipmaps = config.GetBool("MipMaps", true);
}
