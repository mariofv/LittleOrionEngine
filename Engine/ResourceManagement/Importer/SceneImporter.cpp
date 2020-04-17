#include "SceneImporter.h"

FileData SceneImporter::ExtractData(Path & assets_file_path) const
{
	return assets_file_path.GetFile()->Load();
}
