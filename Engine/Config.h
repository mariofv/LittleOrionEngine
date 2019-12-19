#ifndef _CONFIG_H_
#define _CONFIG_H_

#include <rapidjson/document.h>
#include <MathGeoLib.h>

class Config
{
public:
	Config();
	~Config();

	void AddInt(int int_to_add, const std::string &name);
	int GetInt(const std::string &name, int opt_value) const;

	void AddFloat(float float_to_add, const std::string& name);
	float GetFloat(const std::string &name, float opt_value) const;

	void AddFloat3(const float3 &float3_to_add, const std::string& name);
	void GetFloat3(const std::string &name, float3 &return_value, const float3 &opt_value) const;

	void AddQuat(const Quat &quat_to_add, const std::string& name);
	void GetQuat(const std::string& name, Quat& return_value, const Quat& opt_value) const;

public:
	rapidjson::Document config_document;
};

#endif //_CONFIG_H_
