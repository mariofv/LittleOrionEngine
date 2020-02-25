#include "ImportOptions.h"
#include <chrono>
#include "Helper/Config.h"
void ImportOptions::Save(Config& config) const
{
	using namespace std::chrono;
	milliseconds currentTimeStamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	config.AddInt(IMPORTER_VERSION, "ImporterVersion");
	config.AddString(uid, "ExportedFile");
	config.AddInt(currentTimeStamp.count(), "TimeStamp"); //TODO: Create support for long type
}
void ImportOptions::Load(const Config& config) {

	config.GetString("ExportedFile", uid, "");
	config.GetInt("TimeStamp", timestamp);
}
