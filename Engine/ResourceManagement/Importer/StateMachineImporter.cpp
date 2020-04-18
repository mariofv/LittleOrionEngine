#include "StateMachineImporter.h"

#include "Main/Application.h"
#include "Module/ModuleFileSystem.h"
#include "ResourceManagement/Resources/StateMachine.h"

FileData StateMachineImporter::ExtractData(Path& assets_file_path) const
{
	// TODO: Transform from json to binary
	return assets_file_path.GetFile()->Load();
}
