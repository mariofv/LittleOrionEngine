#include "MaterialImporter.h"
#include "Module/ModuleFileSystem.h"
#include <Application.h>

MaterialImporter::MaterialImporter()
{

	APP_LOG_INIT("Initializing DevIL image loader.")
	ilInit();
	iluInit();
	ilutInit();
	APP_LOG_SUCCESS("DevIL image loader initialized correctly.")

}
bool MaterialImporter::Import(const char* texture_path, std::string& output_file) const
{
	ModuleFileSystem::File file = ModuleFileSystem::File(texture_path);
	if (file.filename.empty())
	{
		APP_LOG_SUCCESS("Importing material error: Couldn't find the file to import.")
		return false;
	}
	//Save data
	ILuint size;
	int width, height;
	ILubyte *save_data;
	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
	if (size > 0) {
		save_data = LoadImageData(texture_path, width, height);
		if (ilSaveL(IL_DDS, save_data, size) > 0) // Save to buffer with the ilSaveIL function
		{
			std::string texture_name_no_extension = file.filename.substr(0, file.filename.find_last_of("."));
			output_file = LIBRARY_TEXTURES_FOLDER + "//" + texture_name_no_extension + ".dds";
			App->filesystem->Save(output_file.c_str(), save_data, size);
		}
		delete[] save_data;
	}
	return true;
}

ILubyte * MaterialImporter::LoadImageData(const char* texture_path, int& width, int& height) const
{
	ilLoadImage(texture_path);

	ILenum error;
	error = ilGetError();
	if (error == IL_COULD_NOT_OPEN_FILE)
	{
		APP_LOG_ERROR("Error loading texture %s. File not found", texture_path);
		return nullptr;
	}

	ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);

	ILinfo ImageInfo;
	iluGetImageInfo(&ImageInfo);
	if (ImageInfo.Origin == IL_ORIGIN_UPPER_LEFT)
	{
		iluFlipImage();
	}

	ILubyte * data = ilGetData();
	width = ilGetInteger(IL_IMAGE_WIDTH);
	height = ilGetInteger(IL_IMAGE_HEIGHT);
	return data;
}