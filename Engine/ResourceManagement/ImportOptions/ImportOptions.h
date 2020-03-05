#ifndef _IMPORTOPTIONS_H_
#define _IMPORTOPTIONS_H_
#include <string>
class Config;
struct ImportOptions
{
	ImportOptions() = default;
	ImportOptions(const std::string & uid, int version);
	virtual ~ImportOptions() = default;

	std::string uid;
	long long timestamp = 0; //PLACEHOLDER
	int version = 0;

	virtual void Save(Config& config) const;
	virtual void Load(const Config& config);

};


#endif // !1

