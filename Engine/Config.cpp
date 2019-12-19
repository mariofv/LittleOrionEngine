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

int Config::GetInt(const std::string& name, int opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return current_value.GetInt();
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

float Config::GetFloat(const std::string& name, float opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return current_value.GetFloat();
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

void Config::GetFloat3(const std::string& name, float3 &return_value, const float3 &opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return_value = opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return_value = float3(
			current_value[0].GetFloat(),
			current_value[1].GetFloat(),
			current_value[2].GetFloat()
		);
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

void Config::GetQuat(const std::string& name, Quat& return_value, const Quat& opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return_value = opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return_value = Quat(
			current_value[0].GetFloat(),
			current_value[1].GetFloat(),
			current_value[2].GetFloat(),
			current_value[3].GetFloat()
		);
	}
}
