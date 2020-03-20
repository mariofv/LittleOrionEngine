#include "Helper/Config.h"
#include "ImportOptions.h"

#include <chrono>

void ImportOptions::Save(Config& config) const
{
	using namespace std::chrono;
	seconds currentTimeStamp = duration_cast<seconds>(system_clock::now().time_since_epoch());
	long long n = currentTimeStamp.count();
	config.AddInt(version, "ImporterVersion");
	config.AddUInt(uuid, "UUID");
	config.AddUInt(static_cast<unsigned int>(resource_type), "ResourceType");
	config.AddString(exported_file, "ExportedFile");
	config.AddString(imported_file, "ImportedFile");
	config.AddInt64(currentTimeStamp.count(), "TimeStamp");
}

void ImportOptions::Load(const Config& config) {

	version = config.GetInt("ImporterVersion", 0);
	uuid = config.GetUInt("UUID", uuid);
	resource_type = static_cast<ResourceType>(config.GetUInt("ResourceType",  static_cast<unsigned int>(ResourceType::UNKNOWN)));
	config.GetString("ExportedFile", exported_file, "");
	config.GetString("ImportedFile", imported_file, "");
	timestamp = config.GetInt64("TimeStamp", 0);
}
