#include "Config.h"

Config::Config()
{
	config_document.SetObject();
}

Config::~Config()
{

}

void Config::AddInt(int int_to_add, const std::string &name)
{
	if (!config_document.HasMember(name.c_str()))
	{
		rapidjson::Value member_name(name.c_str(), config_document.GetAllocator());
		config_document.AddMember(member_name, int_to_add, config_document.GetAllocator());
	}
	else 
	{
		rapidjson::Value& current_value = config_document[name.c_str()];
		current_value.SetInt(int_to_add);
	}
}

void Config::AddFloat(float float_to_add, const std::string& name)
{

}