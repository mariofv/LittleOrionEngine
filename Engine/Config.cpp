#include "Config.h"

#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

Config::Config(rapidjson::Document::AllocatorType& allocator) : allocator(allocator)
{
	config_value = rapidjson::Value(rapidjson::kObjectType);
}

Config::~Config()
{

}

void Config::AddInt(int value_to_add, const std::string &name)
{
	rapidjson::Value member_name(name.c_str(), allocator);
	config_value.AddMember(member_name, value_to_add, allocator);
}

int Config::GetInt(const std::string& name, int opt_value) const
{
	if (!config_value.HasMember(name.c_str()))
	{
		return opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_value[name.c_str()];
		return current_value.GetInt();
	}
}

void Config::AddUInt(unsigned int value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), allocator);
	config_value.AddMember(member_name, value_to_add, allocator);
}

int Config::GetUInt(const std::string& name, unsigned int opt_value) const
{
	if (!config_value.HasMember(name.c_str()))
	{
		return opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_value[name.c_str()];
		return current_value.GetUint64();
	}
}

void Config::AddFloat(float value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), allocator);
	config_value.AddMember(member_name, value_to_add, allocator);
}

float Config::GetFloat(const std::string& name, float opt_value) const
{
	if (!config_value.HasMember(name.c_str()))
	{
		return opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_value[name.c_str()];
		return current_value.GetFloat();
	}
}

void Config::AddBool(bool value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), allocator);
	config_value.AddMember(member_name, value_to_add, allocator);
}

bool Config::GetBool(const std::string& name, bool opt_value) const
{
	if (!config_value.HasMember(name.c_str()))
	{
		return opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_value[name.c_str()];
		return current_value.GetBool();
	}
}

void Config::AddString(const std::string value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), allocator);
	rapidjson::Value string_value(value_to_add.c_str(), allocator);
	config_value.AddMember(member_name, string_value, allocator);
}

void Config::GetString(const std::string& name, std::string& return_value, const std::string& opt_value) const
{
	if (!config_value.HasMember(name.c_str()))
	{
		return_value = opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_value[name.c_str()];
		return_value = current_value.GetString();
	}
}

void Config::AddFloat3(const float3& value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), allocator);
	rapidjson::Value array_value(rapidjson::kArrayType);
	array_value.PushBack(value_to_add.x, allocator);
	array_value.PushBack(value_to_add.y, allocator);
	array_value.PushBack(value_to_add.z, allocator);

	config_value.AddMember(member_name, array_value, allocator);
}

void Config::GetFloat3(const std::string& name, float3 &return_value, const float3 &opt_value) const
{
	if (!config_value.HasMember(name.c_str()))
	{
		return_value = opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_value[name.c_str()];
		return_value = float3(
			current_value[0].GetFloat(),
			current_value[1].GetFloat(),
			current_value[2].GetFloat()
		);
	}
}

void Config::AddQuat(const Quat& value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), allocator);
	rapidjson::Value quat_value(rapidjson::kArrayType);
	quat_value.PushBack(value_to_add.x, allocator);
	quat_value.PushBack(value_to_add.y, allocator);
	quat_value.PushBack(value_to_add.z, allocator);
	quat_value.PushBack(value_to_add.w, allocator);

	config_value.AddMember(member_name, quat_value, allocator);
}

void Config::GetQuat(const std::string& name, Quat& return_value, const Quat& opt_value) const
{
	if (!config_value.HasMember(name.c_str()))
	{
		return_value = opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_value[name.c_str()];
		return_value = Quat(
			current_value[0].GetFloat(),
			current_value[1].GetFloat(),
			current_value[2].GetFloat(),
			current_value[3].GetFloat()
		);
	}
}

void Config::AddChildConfig(Config& value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), allocator);
	config_value.AddMember(member_name, value_to_add.config_value, allocator);
}


void Config::AddChildrenConfig(std::vector<Config*> &value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), allocator);
	rapidjson::Value children_configs_value(rapidjson::kArrayType);
	for (unsigned int i = 0; i < value_to_add.size(); ++i)
	{
		children_configs_value.PushBack(value_to_add[i]->config_value.Move(), allocator);
		delete value_to_add[i];
	}
	config_value.AddMember(member_name, children_configs_value, allocator);
}

