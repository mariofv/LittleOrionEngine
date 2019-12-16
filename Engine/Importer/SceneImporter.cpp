#include "SceneImporter.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

void SceneImporter::LoadImportedFilesRegister() const
{
	char* buffer;
	App->filesystem->Load(REGISTER_PATH, &buffer);
	std::string full_register_json = std::string(buffer);

	rapidjson::Document json_document;
	json_document.Parse(full_register_json.c_str());

	rapidjson::Value& results = json_document[0];
	assert(results.IsArray());
	for (rapidjson::SizeType i = 0; i < results.Size(); i++) {
		// Store the value of the element in a vector.
		std::string assets_object = std::string(results[i][0].GetString());
		std::string library_object = std::string(results[i][1].GetString());
		//imported_file_register.emplace(assets_object, library_object);
	}

}

void SceneImporter::SaveImportedFilesRegister() const
{
	std::string full_register_json;
	for (auto & file_register : imported_file_register)
	{
		full_register_json += "\""+ file_register.first+ "\"" + ":" + "\"" +file_register.second + "\"" + ",";
	}

	App->filesystem->Save(REGISTER_PATH, full_register_json.c_str(), sizeof(full_register_json));
}
