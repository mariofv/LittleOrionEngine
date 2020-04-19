#include "SkyboxImporter.h"

#include "Helper/Config.h"
#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"

FileData SkyboxImporter::ExtractData(Path& assets_file_path, const Metafile& metafile) const
{
	return assets_file_path.GetFile()->Load();
}