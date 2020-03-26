#include "TextureImporter.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"

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

FileData TextureImporter::ExtractData(Path& assets_file_path) const
{
	if (assets_file_path.GetFilename().find("_normal") != std::string::npos)
	{
		return ExtractDataToTGA(assets_file_path);
	}
	else 
	{
		return ExtractDataToDDS(assets_file_path);
	}
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
	ILenum original_image_type = GetImageType(file_path);
	ilLoadL(original_image_type, texture_data.buffer, texture_data.size);

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

ILenum TextureImporter::GetImageType(const Path& file_path) const
{
	std::string file_extension = file_path.GetExtension();
	ILenum image_type;
		
	if (file_extension == "bmp")
	{
		image_type = IL_BMP;
	}
	else if (file_extension == "cut")
	{
		image_type = IL_CUT;
	}
	else if (file_extension == "dds")
	{
		image_type = IL_DDS;
	}
	else if (file_extension == "gif")
	{
		image_type = IL_GIF;
	}
	else if (file_extension == "ico")
	{
		image_type = IL_ICO;
	}
	else if (file_extension == "jpg")
	{
		image_type = IL_JPG;
	}
	else if (file_extension == "lif")
	{
		image_type = IL_LIF;
	}
	else if (file_extension == "mng")
	{
		image_type = IL_MNG;
	}
	else if (file_extension == "pcd")
	{
		image_type = IL_PCD;
	}
	else if (file_extension == "pcx")
	{
		image_type = IL_PCX;
	}
	else if (file_extension == "pic")
	{
		image_type = IL_PIC;
	}
	else if (file_extension == "png")
	{
		image_type = IL_PNG;
	}
	else if (file_extension == "pbm")
	{
		image_type = IL_PNM;
	}
	else if (file_extension == "pgm")
	{
		image_type = IL_PNM;
	}
	else if (file_extension == "ppm")
	{
		image_type = IL_PNM;
	}
	else if (file_extension == "psd")
	{
		image_type = IL_PSD;
	}
	else if (file_extension == "psp")
	{
		image_type = IL_PSP;
	}
	else if (file_extension == "bw")
	{
		image_type = IL_SGI;
	}
	else if (file_extension == "rgb")
	{
		image_type = IL_SGI;
	}
	else if (file_extension == "rgba")
	{
		image_type = IL_SGI;
	}
	else if (file_extension == "sgi")
	{
		image_type = IL_SGI;
	}
	else if (file_extension == "tga")
	{
		image_type = IL_TGA;
	}
	else if (file_extension == "tif")
	{
		image_type = IL_TIF;
	}
	else if (file_extension == "tiff")
	{
		image_type = IL_TIF;
	}
	else if (file_extension == "jasc")
	{
		image_type = IL_JASC_PAL;
	}
	else
	{
		image_type = IL_TYPE_UNKNOWN;
	}

	return image_type;
}