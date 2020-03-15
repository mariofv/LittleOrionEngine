#ifndef _IMPORTOPTIONS_H_
#define _IMPORTOPTIONS_H_
#include <string>
class Config;

struct ImportOptions
{
	ImportOptions() = default;
	virtual ~ImportOptions() = default;

	//Copy and move
	ImportOptions(const ImportOptions& component_to_copy) = default;
	ImportOptions(ImportOptions&& component_to_move) = default;

	ImportOptions & operator=(const ImportOptions & component_to_copy) = default;
	ImportOptions & operator=(ImportOptions && component_to_move) = default;

	uint64_t uid = 0;
	long long timestamp = 0; //PLACEHOLDER
	int version = 0;
	std::string exported_file;
	std::string original_file;

	virtual void Save(Config& config) const;
	virtual void Load(const Config& config);

};


#endif // !1

