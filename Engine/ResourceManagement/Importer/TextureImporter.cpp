#include "TextureImporter.h"

#include "Main/Application.h"
#include "Helper/Utils.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Metafile/TextureMetafile.h"

#include <algorithm>
#include <Brofiler/Brofiler.h>

TextureImporter::TextureImporter() : Importer(ResourceType::TEXTURE) 
{
	APP_LOG_INIT("Initializing DevIL image loader.")
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);
	iluInit();
	ilutInit();
	APP_LOG_SUCCESS("DevIL image loader initialized correctly.")

}

FileData TextureImporter::ExtractData(Path& assets_file_path, const Metafile& metafile) const
{
	const TextureMetafile& texture_metafile = static_cast<const TextureMetafile&>(metafile);
	FileData file_data;
	if (texture_metafile.texture_options.texture_type != TextureType::DEFAULT)
	{
		file_data = ExtractDataToTGA(assets_file_path);
	}
	else 
	{
		file_data = ExtractDataToDDS(assets_file_path);
	}

	return CreateBinary(assets_file_path, file_data, texture_metafile);
}

const FileData &TextureImporter::CreateBinary(Path & assets_file_path, FileData &file_data, const TextureMetafile & texture_metafile) const
{
	std::string extension = assets_file_path.GetExtension();

	uint32_t size = sizeof(TextureOptions) + extension.size() + file_data.size;

	char* data = new char[size];
	char* cursor = data;
	size_t bytes = sizeof(TextureOptions);
	memcpy(cursor, &texture_metafile.texture_options, bytes);

	cursor += bytes;
	bytes = extension.size();
	assert(bytes == 3);
	memcpy(cursor, extension.data(), bytes);

	cursor += bytes;
	bytes = file_data.size;
	memcpy(cursor, file_data.buffer, bytes);
	delete[] file_data.buffer;
	FileData texture_file_data{ data, size };
	return texture_file_data;
}

FileData TextureImporter::ExtractDataToDDS(const Path& assets_file_path) const
{
	FileData texture_data;

	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	int width, height;
	ILubyte* save_data = LoadImageDataInMemory(assets_file_path, IL_RGBA, width, height);

	//Save data
	ILuint size;

	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
	if (size > 0)
	{
		if (ilSaveL(IL_DDS, save_data, size) > 0) // Save to buffer with the ilSaveIL function
		{
			texture_data.size = size;
			char* texture_bytes_data = new char[size];
			memcpy(texture_bytes_data, save_data, size);
			texture_data.buffer = texture_bytes_data;
		}		
		ilDeleteImages(1, &image);
	};

	return texture_data;
}

FileData TextureImporter::ExtractDataToTGA(const Path& assets_file_path) const
{
	return assets_file_path.GetFile()->Load();
}

ILubyte* TextureImporter::LoadImageDataInMemory(const Path& file_path, int image_type, int& width, int& height) const
{
	FileData texture_data = file_path.GetFile()->Load();
	ILenum original_image_type = Utils::GetImageType(file_path.GetExtension());
	ilLoadL(original_image_type, texture_data.buffer, texture_data.size);
	delete[] texture_data.buffer;
	ILenum error;
	error = ilGetError();
	if (error == IL_COULD_NOT_OPEN_FILE)
	{
		APP_LOG_ERROR("Error loading texture %s. File not found", file_path.GetFullPath().c_str());
		return nullptr;
	}

	ilConvertImage(image_type, IL_UNSIGNED_BYTE);

	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT && image_type != IL_DDS)
	{
		iluFlipImage();
	}

	ILubyte* data = ilGetData();
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	return data;
}
