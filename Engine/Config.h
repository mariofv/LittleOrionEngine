#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <rapidjson/document.h>
#include <MathGeoLib.h>

class Config
{
public:
	Config();
	~Config();

	void AddInt(int value_to_add, const std::string &name);
	int GetInt(const std::string &name, int opt_value) const;

	void AddUInt(unsigned int value_to_add, const std::string& name);
	int GetUInt(const std::string& name, unsigned int opt_value) const;

	void AddFloat(float value_to_add, const std::string& name);
	float GetFloat(const std::string &name, float opt_value) const;

	void AddBool(bool value_to_add, const std::string& name);
	bool GetBool(const std::string& name, bool opt_value) const;

	void AddString(const std::string value_to_add, const std::string& name);
	void GetString(const std::string& name, std::string& return_value, const std::string& opt_value) const;

	void AddFloat3(const float3 &value_to_addvalue_to_add, const std::string& name);
	void GetFloat3(const std::string &name, float3 &return_value, const float3 &opt_value) const;

	void AddQuat(const Quat & value_to_add, const std::string& name);
	void GetQuat(const std::string& name, Quat& return_value, const Quat& opt_value) const;

	void AddChildConfig(Config& value_to_add, const std::string& name);

	void AddChildrenConfig(std::vector<Config*> value_to_add, const std::string& name);

public:
	rapidjson::Document config_document;
};

#endif //_CONFIG_H_
