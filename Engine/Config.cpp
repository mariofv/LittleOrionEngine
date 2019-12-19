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
	if (!config_document.HasMember(name.c_str()))
	{
		rapidjson::Value member_name(name.c_str(), config_document.GetAllocator());
		config_document.AddMember(member_name, float_to_add, config_document.GetAllocator());
	}
	else
	{
		rapidjson::Value& current_value = config_document[name.c_str()];
		current_value.SetFloat(float_to_add);
	}
}

void Config::AddFloat3(const float3& float3_to_add, const std::string& name)
{
	if (!config_document.HasMember(name.c_str()))
	{
		rapidjson::Value member_name(name.c_str(), config_document.GetAllocator());
		rapidjson::Value array_value(rapidjson::kArrayType);
		array_value.PushBack(float3_to_add.x, config_document.GetAllocator());
		array_value.PushBack(float3_to_add.y, config_document.GetAllocator());
		array_value.PushBack(float3_to_add.z, config_document.GetAllocator());

		config_document.AddMember(member_name, array_value, config_document.GetAllocator());
	}
	else
	{
		rapidjson::Value& current_value = config_document[name.c_str()];
		current_value[0].SetFloat(float3_to_add.x);
		current_value[1].SetFloat(float3_to_add.y);
		current_value[2].SetFloat(float3_to_add.z);
	}
}

void Config::AddQuat(const Quat& quat_to_add, const std::string& name)
{
	if (!config_document.HasMember(name.c_str()))
	{
		rapidjson::Value member_name(name.c_str(), config_document.GetAllocator());
		rapidjson::Value quat_value(rapidjson::kArrayType);
		quat_value.PushBack(quat_to_add.x, config_document.GetAllocator());
		quat_value.PushBack(quat_to_add.y, config_document.GetAllocator());
		quat_value.PushBack(quat_to_add.z, config_document.GetAllocator());
		quat_value.PushBack(quat_to_add.w, config_document.GetAllocator());

		config_document.AddMember(member_name, quat_value, config_document.GetAllocator());
	}
	else
	{
		rapidjson::Value& current_value = config_document[name.c_str()];
		current_value[0].SetFloat(quat_to_add.x);
		current_value[1].SetFloat(quat_to_add.y);
		current_value[2].SetFloat(quat_to_add.z);
		current_value[3].SetFloat(quat_to_add.w);
	}
}
