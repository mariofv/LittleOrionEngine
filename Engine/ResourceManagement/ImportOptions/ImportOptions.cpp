#include "ImportOptions.h"
#include <chrono>
#include "Helper/Config.h"

void ImportOptions::Save(Config& config) const
{
	using namespace std::chrono;
	milliseconds currentTimeStamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	config.AddInt(version, "ImporterVersion");
	config.AddUInt(uid, "UID");
	config.AddString(exported_file, "ExportedFile");
	config.AddString(original_file, "OriginalFile");
	config.AddInt(currentTimeStamp.count(), "TimeStamp"); //TODO: Create support for long type
}
void ImportOptions::Load(const Config& config) {

	version = config.GetInt("ImporterVersion", 0);
	version = config.GetUInt("UID", uid);
	config.GetString("ExportedFile", exported_file, "");
	config.GetString("OriginalFile", original_file, "");
	timestamp = config.GetInt("TimeStamp", 0);
}
