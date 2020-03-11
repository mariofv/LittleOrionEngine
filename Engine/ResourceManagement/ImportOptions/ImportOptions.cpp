#include "ImportOptions.h"
#include <chrono>
#include "Helper/Config.h"


void ImportOptions::Save(Config& config) const
{
	using namespace std::chrono;
	milliseconds currentTimeStamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	config.AddInt(version, "ImporterVersion");
	config.AddUInt(uuid, "UUID");
	config.AddUInt(static_cast<unsigned int>(resource_type), "ResourceType");
	config.AddString(exported_file, "ExportedFile");
	config.AddString(imported_file, "OriginalFile");
	config.AddInt(currentTimeStamp.count(), "TimeStamp"); //TODO: Create support for long type
}
void ImportOptions::Load(const Config& config) {

	version = config.GetInt("ImporterVersion", 0);
	uuid = config.GetUInt("UUID", uuid);
	resource_type = static_cast<ResourceType>(config.GetUInt("ResourceType",  static_cast<unsigned int>(ResourceType::UNKNOWN)));
	config.GetString("ExportedFile", exported_file, "");
	config.GetString("ImportedFile", imported_file, "");
	timestamp = config.GetInt("TimeStamp", 0);
}
