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
	void AddFloat(float float_to_add, const std::string& name);

	void AddFloat3(const float3 &float3_to_add, const std::string& name);
	void AddQuat(const Quat &quat_to_add, const std::string& name);

public:
	rapidjson::Document config_document;

};

#endif //_CONFIG_H_
