#ifndef _IMPORTOPTIONS_H_
#define _IMPORTOPTIONS_H_
#include <string>
class Config;
struct ImportOptions
{
	std::string uid;
	long long timestamp;

	virtual void Save(Config& config) const = 0;
	virtual void Load(const Config& config) = 0;

private:
	static const int IMPORTER_VERSION = 1;
};


#endif // !1

