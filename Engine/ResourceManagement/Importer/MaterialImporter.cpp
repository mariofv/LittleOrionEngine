#include "MaterialImporter.h"
#include "Main/Application.h"

#include <algorithm>

#include <assimp/scene.h>
#include "Brofiler/Brofiler.h"
MaterialImporter::MaterialImporter()
{
	APP_LOG_INIT("Initializing DevIL image loader.")
	ilInit();
	ilEnable(IL_ORIGIN_SET);
	ilOriginFunc(IL_ORIGIN_UPPER_LEFT);
	iluInit();
	ilutInit();
	APP_LOG_SUCCESS("DevIL image loader initialized correctly.")

}
std::pair<bool, std::string> MaterialImporter::Import(const File & file) const
{
	if (file.filename.empty())
	{
		APP_LOG_ERROR("Importing material error: Couldn't find the file to import.")
		return std::pair<bool, std::string>(false,"");
	}

	std::shared_ptr<File> already_imported = GetAlreadyImportedResource(LIBRARY_TEXTURES_FOLDER, file);
	if (already_imported != nullptr) {
		return std::pair<bool, std::string>(true, already_imported->file_path);
	}

	App->filesystem->MakeDirectory(LIBRARY_TEXTURES_FOLDER);
	//Bound image

	ILuint image;
	ilGenImages(1, &image);
	ilBindImage(image);
	int width, height;
	ILubyte * save_data = LoadImageData(file.file_path.c_str(), IL_RGBA, width, height);
	//Get new Name

	std::string texture_name_no_extension = file.filename.substr(0, file.filename.find_last_of("."));
	std::string output_file = LIBRARY_TEXTURES_FOLDER + "//" + texture_name_no_extension + ".dds";

	//Save data
	ILuint size;

	ilSetInteger(IL_DXTC_FORMAT, IL_DXT5);// To pick a specific DXT compression use
	size = ilSaveL(IL_DDS, NULL, 0); // Get the size of the data buffer
	if (size > 0) {

		if (ilSaveL(IL_DDS, save_data, size) > 0) // Save to buffer with the ilSaveIL function
		{
			App->filesystem->Save(output_file.c_str(), (unsigned int *)save_data, size);
		}
		ilDeleteImages(1, &image);
	}
	return std::pair<bool, std::string>(true, output_file);
}

void MaterialImporter::ImportMaterialFromMesh(const aiScene* scene, size_t mesh_index, const char* file_path,std::vector<std::string> & loaded_meshes_materials) const
{
	int mesh_material_index = scene->mMeshes[mesh_index]->mMaterialIndex;
	std::string model_base_path = std::string(file_path);
	aiTextureMapping mapping = aiTextureMapping_UV;
	for (size_t i = 0; i < AI_TEXTURE_TYPE_MAX; i++)
	{
		aiTextureType type = static_cast<aiTextureType>(i);
		for (size_t j = 0; j < scene->mMaterials[mesh_material_index]->GetTextureCount(type); j++)
		{
			aiString file;
			scene->mMaterials[mesh_material_index]->GetTexture(type, j, &file, &mapping, 0);
			std::string material_texture = ImportMaterialData(file.data, model_base_path);
			int texture_type = static_cast<int>(GetTextureTypeFromAssimpType(type));
			material_texture = std::to_string(texture_type) + ":" + material_texture;
			if (!material_texture.empty())
			{
				loaded_meshes_materials.push_back(material_texture);
			}
		}
	}
}

std::string MaterialImporter::ImportMaterialData(const std::string & material_path, const std::string model_base_path) const
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
std::shared_ptr<Texture> MaterialImporter::Load(const std::string& file_path) const{

	if (!App->filesystem->Exists(file_path.c_str()))
	{
		return nullptr;
	}
	BROFILER_CATEGORY("Load Texture", Profiler::Color::BurlyWood);
	//Check if the texture is already loaded
	auto it = std::find_if(texture_cache.begin(), texture_cache.end(), [file_path](const std::shared_ptr<Texture> & texture)
	{
		return texture->exported_file == file_path;
	});
	if (it != texture_cache.end())
	{
		APP_LOG_INIT("Model %s exists in cache.", file_path.c_str());
		return *it;
	}
	size_t size;
	DDS_HEADER ddsHeader;
	char * data = LoadCompressedDDS(file_path.c_str(), ddsHeader,size);
	if (data)
	{
		size_t dds_header_offset = sizeof(DDS_HEADER) + magic_number;
		std::shared_ptr<Texture> loaded_texture = std::make_shared<Texture>(data + dds_header_offset, size - dds_header_offset, ddsHeader.dwWidth, ddsHeader.dwHeight, file_path);
		texture_cache.push_back(loaded_texture);
		free(data);
		return loaded_texture;
	}
	return nullptr;
}

ILubyte * MaterialImporter::LoadImageData(const std::string& file_path, int image_type ,int & width, int & height ) const
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

unsigned int MaterialImporter::LoadCubemap(std::vector<std::string> faces_paths) const
{
	unsigned int texture_id;

	glGenTextures(1, &texture_id);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_id);
	ILuint image;

	for (unsigned int i = 0; i < faces_paths.size(); i++)
	{
		size_t size;
		DDS_HEADER ddsHeader;
		char * data = LoadCompressedDDS(faces_paths[i].c_str(), ddsHeader, size);
		size_t dds_header_offset = sizeof(DDS_HEADER) + magic_number;
		if (data)
		{
			glCompressedTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_COMPRESSED_RGBA_S3TC_DXT5_EXT, ddsHeader.dwWidth, ddsHeader.dwHeight, 0, size - dds_header_offset, data + dds_header_offset);
			free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return texture_id;
}

//Remove the material from the cache if the only owner is the cache itself
void MaterialImporter::RemoveTextureFromCacheIfNeeded(const std::shared_ptr<Texture> & texture) 
{
	auto it = std::find(texture_cache.begin(), texture_cache.end(), texture);
	if (it != texture_cache.end() && (*it).use_count() <= 2)
	{
		texture_cache.erase(it);
	}
}

std::string MaterialImporter::GetTextureFileName(std::string texture_file_path) const
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

Texture::TextureType MaterialImporter::GetTextureTypeFromAssimpType(aiTextureType type) const
{
	switch (type)
	{
	case aiTextureType_DIFFUSE:
		return Texture::TextureType::DIFUSSE;
		break;
	case aiTextureType_SPECULAR:
		return Texture::TextureType::SPECULAR;
		break;
	case aiTextureType_EMISSIVE:
		return Texture::TextureType::EMISSIVE;
		break;
	case aiTextureType_AMBIENT_OCCLUSION:
		return Texture::TextureType::OCLUSION;
		break;
	default:
		return Texture::TextureType::UNKNOWN;
		break;
	}
}

char * MaterialImporter::LoadCompressedDDS(const std::string& file_path, DDS_HEADER & dds_header, size_t & dds_content_size) const
{
	char * data = App->filesystem->Load(file_path.c_str(), dds_content_size);
	if (data) 
	{
		memcpy(&dds_header, data + magic_number, sizeof(DDS_HEADER));
	}
	return data;
}
