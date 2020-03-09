#include "ImportOptions.h"
#include <chrono>
#include "Helper/Config.h"

ImportOptions::ImportOptions(const std::string & uid, int version) : uid(uid), version(version)
{

}
void ImportOptions::Save(Config& config) const
{
	using namespace std::chrono;
	milliseconds currentTimeStamp = duration_cast<milliseconds>(system_clock::now().time_since_epoch());

	config.AddInt(version, "ImporterVersion");
	config.AddString(uid, "ExportedFile");
	config.AddInt(currentTimeStamp.count(), "TimeStamp"); //TODO: Create support for long type
}
void ImportOptions::Load(const Config& config) {

	version = config.GetInt("ImporterVersion", 0);
	config.GetString("ExportedFile", uid, "");
	timestamp = config.GetInt("TimeStamp", 0);
}
