#include "Config.h"

#include "Filesystem/File.h"
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

Config::Config()
{
	config_document.SetObject();
	allocator = &config_document.GetAllocator();
}

Config::Config(FileData & data)
{
	std::string serialized_string((char*)data.buffer, data.size);
	delete[] data.buffer;
	config_document.Parse(serialized_string.c_str());
	allocator = &config_document.GetAllocator();
}

Config::Config(const rapidjson::Value& object_value)
{
	config_document.CopyFrom(object_value, config_document.GetAllocator());
}

Config::Config(const std::string& serialized_scene_string)
{
	config_document.Parse(serialized_scene_string.c_str());
	allocator = &config_document.GetAllocator();
}

Config::Config(const Config& other)
{
	config_document.CopyFrom(other.config_document, config_document.GetAllocator());
	allocator = &config_document.GetAllocator();
}

Config& Config::operator=(const Config& other)
{
	config_document.CopyFrom(other.config_document, config_document.GetAllocator());
	allocator = &config_document.GetAllocator();
	return *this;
}

rapidjson::Document::AllocatorType& Config::GetAllocator() const
{
	return *allocator;
}

void Config::AddInt(int value_to_add, const std::string &name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	config_document.AddMember(member_name, value_to_add, *allocator);
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

void Config::AddUInt(uint64_t value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	config_document.AddMember(member_name, value_to_add, *allocator);
}

uint64_t Config::GetUInt(const std::string& name, unsigned int opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return current_value.GetUint64();
	}
}
ENGINE_API void Config::AddUInt32(uint32_t value_to_add, const std::string & name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	config_document.AddMember(member_name, value_to_add, *allocator);
}
ENGINE_API uint32_t Config::GetUInt32(const std::string & name, unsigned int opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return current_value.GetUint();
	}
}
void Config::AddInt64(int64_t value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	config_document.AddMember(member_name, value_to_add, *allocator);
}

int64_t Config::GetInt64(const std::string& name, int64_t opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return current_value.GetInt64();
	}
}
void Config::AddFloat(float value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	config_document.AddMember(member_name, value_to_add, *allocator);
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

void Config::AddBool(bool value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	config_document.AddMember(member_name, value_to_add, *allocator);
}

bool Config::GetBool(const std::string& name, bool opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return current_value.GetBool();
	}
}

void Config::AddString(const std::string value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	rapidjson::Value string_value(value_to_add.c_str(), value_to_add.size(), *allocator);
	config_document.AddMember(member_name, string_value, *allocator);
}

void Config::GetString(const std::string& name, std::string& return_value, const std::string& opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return_value = opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return_value = std::string(current_value.GetString(), current_value.GetStringLength());
	}
}

void Config::AddFloat3(const float3& value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	rapidjson::Value array_value(rapidjson::kArrayType);
	array_value.PushBack(value_to_add.x, *allocator);
	array_value.PushBack(value_to_add.y, *allocator);
	array_value.PushBack(value_to_add.z, *allocator);

	config_document.AddMember(member_name, array_value, *allocator);
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

void Config::AddFloat2(const float2& value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	rapidjson::Value array_value(rapidjson::kArrayType);
	array_value.PushBack(value_to_add.x, *allocator);
	array_value.PushBack(value_to_add.y, *allocator);

	config_document.AddMember(member_name, array_value, *allocator);
}

void Config::GetFloat2(const std::string& name, float2 &return_value, const float2 &opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return_value = opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return_value = float2(
			current_value[0].GetFloat(),
			current_value[1].GetFloat()
		);
	}
}

void Config::AddRect(const SDL_Rect& value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	rapidjson::Value array_value(rapidjson::kArrayType);
	array_value.PushBack(value_to_add.x, *allocator);
	array_value.PushBack(value_to_add.y, *allocator);
	array_value.PushBack(value_to_add.w, *allocator);
	array_value.PushBack(value_to_add.h, *allocator);

	config_document.AddMember(member_name, array_value, *allocator);
}

void Config::GetRect(const std::string &name, SDL_Rect &return_value, const SDL_Rect &opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return_value = opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return_value = SDL_Rect();
		return_value.x = static_cast<int>(current_value[0].GetFloat());
		return_value.y = static_cast<int>(current_value[1].GetFloat());
		return_value.w = static_cast<int>(current_value[2].GetFloat());
		return_value.h = static_cast<int>(current_value[3].GetFloat());
	}
}

void Config::AddQuat(const Quat& value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	rapidjson::Value quat_value(rapidjson::kArrayType);
	quat_value.PushBack(value_to_add.x, *allocator);
	quat_value.PushBack(value_to_add.y, *allocator);
	quat_value.PushBack(value_to_add.z, *allocator);
	quat_value.PushBack(value_to_add.w, *allocator);

	config_document.AddMember(member_name, quat_value, *allocator);
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

void Config::AddColor(const float4& value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	rapidjson::Value color_value(rapidjson::kArrayType);
	color_value.PushBack(value_to_add.x, *allocator);
	color_value.PushBack(value_to_add.y, *allocator);
	color_value.PushBack(value_to_add.z, *allocator);
	color_value.PushBack(value_to_add.w, *allocator);

	config_document.AddMember(member_name, color_value, *allocator);
}

void Config::GetColor(const std::string& name, float4& return_value, const float4& opt_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return_value = opt_value;
	}
	else
	{
		const rapidjson::Value& current_value = config_document[name.c_str()];
		return_value = float4(
			current_value[0].GetFloat(),
			current_value[1].GetFloat(),
			current_value[2].GetFloat(),
			current_value[3].GetFloat()
		);
	}
}

void Config::AddChildConfig(Config& value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	rapidjson::Value tmp_value(rapidjson::kObjectType);
	tmp_value.CopyFrom(value_to_add.config_document, *allocator);
	config_document.AddMember(member_name, tmp_value, *allocator);
}

void Config::GetChildConfig(const std::string& name, Config& value_to_add) const
{
	assert(config_document.HasMember(name.c_str()));

	const rapidjson::Value& tmp_value = config_document[name.c_str()];
	value_to_add = Config(tmp_value);
}

void Config::AddChildrenConfig(std::vector<Config> &value_to_add, const std::string& name)
{
	rapidjson::Value member_name(name.c_str(), *allocator);
	rapidjson::Value children_configs_value(rapidjson::kArrayType);
	for (unsigned int i = 0; i < value_to_add.size(); ++i)
	{
		rapidjson::Value tmp_value(rapidjson::kObjectType);
		tmp_value.CopyFrom(value_to_add[i].config_document.Move(), *allocator);
		children_configs_value.PushBack(tmp_value.Move(), *allocator);
	}
	config_document.AddMember(member_name, children_configs_value, *allocator);
}

void Config::GetChildrenConfig(const std::string& name, std::vector<Config>& return_value) const
{
	if (!config_document.HasMember(name.c_str()))
	{
		return;
	}

	const rapidjson::Value& children_configs_value = config_document[name.c_str()];
	return_value = std::vector<Config>();
	for (unsigned int i = 0; i < children_configs_value.Size(); ++i)
	{
		const rapidjson::Value& tmp_value = children_configs_value[i];
		Config tmp_config(tmp_value);
		return_value.push_back(tmp_config);
	}
}

void Config::GetSerializedString(std::string& return_string)
{
	rapidjson::StringBuffer buffer;
	rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);

	config_document.Accept(writer);
	return_string = buffer.GetString();
}
