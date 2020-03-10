#include "TextureImporter.h"
#include "Main/Application.h"

#include <algorithm>
#include "Brofiler/Brofiler.h"
TextureImporter::TextureImporter()
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
std::pair<bool, std::string> TextureImporter::Import(const File & file) const
{
	if (file.filename.empty())
	{
		APP_LOG_ERROR("Importing material error: Couldn't find the file to import.")
		return std::pair<bool, std::string>(false,"");
	}

	std::string already_imported = GetAlreadyImportedResource(file);
	if (!already_imported.empty()) {
		return std::pair<bool, std::string>(true, already_imported);
	}

	App->filesystem->MakeDirectory(LIBRARY_TEXTURES_FOLDER);
	
	std::string output_file;

	if (file.filename.find("_normal") != std::string::npos)
	{
		output_file = ImportToTGA(file);
	}
	else 
	{
		output_file = ImportToDDS(file);
	}

	SaveMetaFile(file, output_file);
	return std::pair<bool, std::string>(true, output_file);
}

std::string TextureImporter::ImportMaterialData(const std::string & material_path, const std::string model_base_path) const
{
	APP_LOG_INIT("Loading material texture in described path %s.", material_path.c_str());
	std::pair<bool, std::string> imported = Import(material_path);
	if (imported.first)
	{
		APP_LOG_SUCCESS("Material loaded correctly.");
		return imported.second;
	}

	std::string texture_file_name = GetTextureFileName(material_path);
	std::string textures_path = model_base_path+ "/" + texture_file_name;
	APP_LOG_INIT("Loading material texture in model folder path %s.", model_base_path.c_str());
	imported = Import(textures_path);
	if (imported.first)
	{
		APP_LOG_SUCCESS("Material loaded correctly.");
		return imported.second;
	}

	textures_path = std::string(TEXTURES_PATH) + texture_file_name;
	APP_LOG_INIT("Loading material texture in textures folder %s.", textures_path.c_str());
	imported = Import(textures_path);
	if (imported.first)
	{
		APP_LOG_SUCCESS("Material loaded correctly.");
		return imported.second;
	}
	return "";
}


ILubyte * TextureImporter::LoadImageDataInMemory(const std::string& file_path, int image_type ,int & width, int & height ) const
{
	ilLoadImage(file_path.c_str());

	ILenum error;
	error = ilGetError();
	if (error == IL_COULD_NOT_OPEN_FILE)
	{
		APP_LOG_ERROR("Error loading texture %s. File not found", file_path.c_str());
		return nullptr;
	}

	ilConvertImage(image_type, IL_UNSIGNED_BYTE);

	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT && image_type != IL_DDS)
	{
		iluFlipImage();
	}

	ILubyte * data = ilGetData();
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	return data;
}




//Remove the material from the cache if the only owner is the cache itself


std::string TextureImporter::GetTextureFileName(std::string texture_file_path) const
{
	std::replace(texture_file_path.begin(), texture_file_path.end(), '\\', '/');
	std::size_t found = texture_file_path.find_last_of("/");
	if (found == std::string::npos)
	{
		return texture_file_path;
	}
	else
	{
		std::string texture_filename = texture_file_path.substr(found, texture_file_path.length());

		return texture_filename;
	}
}

std::string TextureImporter::ImportToDDS(const File & file) const
{
	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	int width, height;
	ILubyte * save_data = LoadImageDataInMemory(file.file_path.c_str(), IL_RGBA, width, height);
	//Get new Name

	std::string texture_name_no_extension = file.filename.substr(0, file.filename.find_last_of("."));
	std::string output_file = LIBRARY_TEXTURES_FOLDER + "/" + texture_name_no_extension + ".dds";

	//Save data
	ILuint size;

	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
	if (size > 0) {

		if (ilSaveL(IL_DDS, save_data, size) > 0) // Save to buffer with the ilSaveIL function
		{
			App->filesystem->Save(output_file.c_str(), (unsigned int *)save_data, size);
		}
		ilDeleteImages(1, &image);
	}
	return output_file;
}

std::string TextureImporter::ImportToTGA(const File & file) const
{
	std::string output_file = LIBRARY_TEXTURES_FOLDER + "/" +file.filename;
	bool copied = App->filesystem->Copy(file.file_path.c_str(), output_file.c_str());
	if (!copied)
	{
		return "";
	}
	return output_file;
}