#include "StateMachineImporter.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Resources/StateMachine.h"

FileData StateMachineImporter::ExtractData(Path& assets_file_path) const
{
	return assets_file_path.GetFile()->Load();
}
