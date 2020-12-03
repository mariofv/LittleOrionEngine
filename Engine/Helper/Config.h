#ifndef _CONFIG_H_
#define _CONFIG_H_

#ifndef ENGINE_EXPORTS
#define ENGINE_EXPORTS
#endif

#include "Main/Globals.h"
#include <MathGeoLib.h>
#include <SDL/SDL.h>

#include <rapidjson/document.h>

struct FileData;
class Config
{
public:
	Config();
	Config(FileData & data);
	Config(const rapidjson::Value& object_value);
	Config(const std::string& serialized_scene_string);
	~Config() = default;
	
	Config(const Config& other);
	Config& operator=(const Config& other);

	rapidjson::Document::AllocatorType& GetAllocator() const;

	ENGINE_API void AddInt(int value_to_add, const std::string &name);
	ENGINE_API int GetInt(const std::string &name, int opt_value) const;

	ENGINE_API void AddUInt(uint64_t value_to_add, const std::string& name);
	ENGINE_API uint64_t GetUInt(const std::string& name, unsigned int opt_value) const;

	ENGINE_API void AddUInt32(uint32_t value_to_add, const std::string& name);
	ENGINE_API uint32_t GetUInt32(const std::string& name, unsigned int opt_value) const;

	void AddInt64(int64_t value_to_add, const std::string &name);
	int64_t GetInt64(const std::string& name, int64_t opt_value) const;

	ENGINE_API void AddFloat(float value_to_add, const std::string& name);
	ENGINE_API float GetFloat(const std::string &name, float opt_value) const;

	ENGINE_API void AddBool(bool value_to_add, const std::string& name);
	ENGINE_API bool GetBool(const std::string& name, bool opt_value) const;

	ENGINE_API void AddString(const std::string value_to_add, const std::string& name);
	ENGINE_API void GetString(const std::string& name, std::string& return_value, const std::string& opt_value) const;

	ENGINE_API void AddFloat3(const float3 &value_to_addvalue_to_add, const std::string& name);
	ENGINE_API void GetFloat3(const std::string &name, float3 &return_value, const float3 &opt_value) const;

	void AddFloat2(const float2 &value_to_addvalue_to_add, const std::string& name);
	void GetFloat2(const std::string &name, float2 &return_value, const float2 &opt_value) const;
	
	void AddRect(const SDL_Rect &value_to_addvalue_to_add, const std::string& name);
	void GetRect(const std::string &name, SDL_Rect &return_value, const SDL_Rect &opt_value) const;

	void AddQuat(const Quat & value_to_add, const std::string& name);
	void GetQuat(const std::string& name, Quat& return_value, const Quat& opt_value) const;

	ENGINE_API void AddColor(const float4& value_to_add, const std::string& name);
	ENGINE_API void GetColor(const std::string& name, float4& return_value, const float4& opt_value) const;

	void AddChildConfig(Config& value_to_add, const std::string& name);
	void GetChildConfig(const std::string& name, Config& value_to_add) const;

	void AddChildrenConfig(std::vector<Config>& value_to_add, const std::string& name);
	void GetChildrenConfig(const std::string& name, std::vector<Config>& return_value) const;

	template<class T>
	void AddVector(const std::vector<T>& value_to_add, const std::string& name)
	{
		rapidjson::Value member_name(name.c_str(), *allocator);
		rapidjson::Value vector_value(rapidjson::kArrayType);
		for (size_t i = 0; i < value_to_add.size(); ++i)
		{
			vector_value.PushBack(value_to_add[i], *allocator);
		}

		config_document.AddMember(member_name, vector_value, *allocator);
	};

	template<>
	void AddVector(const std::vector<std::string>& value_to_add, const std::string& name)
	{
		rapidjson::Value member_name(name.c_str(), *allocator);
		rapidjson::Value vector_value(rapidjson::kArrayType);
		for (size_t i = 0; i < value_to_add.size(); ++i)
		{
			rapidjson::Value string_value(value_to_add[i].c_str(), value_to_add[i].size(), *allocator);
			vector_value.PushBack(string_value, *allocator);
		}

		config_document.AddMember(member_name, vector_value, *allocator);
	};

	template<>
	void AddVector(const std::vector<float3>& value_to_add, const std::string& name)
	{
		rapidjson::Value member_name(name.c_str(), *allocator);
		rapidjson::Value vector_value(rapidjson::kArrayType);
		for (size_t i = 0; i < value_to_add.size(); ++i)
		{
			rapidjson::Value array_value(rapidjson::kArrayType);
			array_value.PushBack(value_to_add[i].x, *allocator);
			array_value.PushBack(value_to_add[i].y, *allocator);
			array_value.PushBack(value_to_add[i].z, *allocator);

			vector_value.PushBack(array_value, *allocator);
		}

		config_document.AddMember(member_name, vector_value, *allocator);
	};

	template<class T> void GetVector(const std::string& name, std::vector<T>& return_value, const std::vector<T>& opt_value) const
	{
		if (!config_document.HasMember(name.c_str()))
		{
			return_value = opt_value;
		}
		else
		{
			const rapidjson::Value& current_value = config_document[name.c_str()];
			return_value = std::vector<T>();
			for (size_t i = 0; i < current_value.Capacity(); ++i)
			{
				return_value.push_back(current_value[i].Get<T>());
			}
		}
	};

	template<>
	void GetVector(const std::string& name, std::vector<std::string>& return_value, const std::vector<std::string>& opt_value) const
	{
		if (!config_document.HasMember(name.c_str()))
		{
			return_value = opt_value;
		}
		else
		{
			const rapidjson::Value& current_value = config_document[name.c_str()];
			return_value = std::vector<std::string>();
			for (size_t i = 0; i < current_value.Capacity(); ++i)
			{
				return_value.push_back(current_value[i].GetString());
			}
		}
	};

	template<>
	void GetVector(const std::string& name, std::vector<float3>& return_value, const std::vector<float3>& opt_value) const
	{
		if (!config_document.HasMember(name.c_str()))
		{
			return_value = opt_value;
		}
		else
		{
			const rapidjson::Value& current_value = config_document[name.c_str()];
			return_value = std::vector<float3>();

			for (size_t i = 0; i < current_value.Capacity(); ++i)
			{
				const rapidjson::Value& current_float3_value = current_value[i];
				return_value.push_back(
					float3(
						current_float3_value[0].GetFloat(),
						current_float3_value[1].GetFloat(),
						current_float3_value[2].GetFloat()
					)
				);
			}
		}
	};

	void GetSerializedString(std::string& return_string);

public:
	rapidjson::Document config_document;
	rapidjson::Document::AllocatorType* allocator;

};

#endif //_CONFIG_H_
