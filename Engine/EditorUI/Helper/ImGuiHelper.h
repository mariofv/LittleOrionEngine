#ifndef _IMGUIHELPER_H_
#define _IMGUIHELPER_H_

#include "ResourceManagement/Metafile/Metafile.h"

#include <memory>
#include <imgui.h>

class Animation;
class Font;
class Material;
class Mesh;
class Prefab;
class Skeleton;
class Skybox;
class StateMachine;
class Texture;
class SoundBank;
class Video;

namespace ImGui
{
	template<typename T>
	static uint32_t ResourceDropper()
	{
		if (ImGui::BeginDragDropTarget())
		{
			ImGui::EndDragDropTarget();
		}
		return 0;
	};
	
	template<>
	static uint32_t ResourceDropper<Animation>()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
			{
				assert(payload->DataSize == sizeof(Metafile*));
				Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
				if (incoming_resource_metafile->resource_type == ResourceType::ANIMATION)
				{
					return incoming_resource_metafile->uuid;
				}
			}
			ImGui::EndDragDropTarget();
		}
		return 0;
	};

	template<>
	static uint32_t ResourceDropper<Font>()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
			{
				assert(payload->DataSize == sizeof(Metafile*));
				Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
				if (incoming_resource_metafile->resource_type == ResourceType::FONT)
				{
					return incoming_resource_metafile->uuid;
				}
			}
			ImGui::EndDragDropTarget();
		}
		return 0;
	};

	template<>
	static uint32_t ResourceDropper<Material>()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
			{
				assert(payload->DataSize == sizeof(Metafile*));
				Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
				if (incoming_resource_metafile->resource_type == ResourceType::MATERIAL)
				{
					return incoming_resource_metafile->uuid;
				}
			}
			ImGui::EndDragDropTarget();
		}
		return 0;
	};

	template<>
	static uint32_t ResourceDropper<Mesh>()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
			{
				assert(payload->DataSize == sizeof(Metafile*));
				Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
				if (incoming_resource_metafile->resource_type == ResourceType::MESH)
				{
					return incoming_resource_metafile->uuid;
				}
			}
			ImGui::EndDragDropTarget();
		}
		return 0;
	};

	template<>
	static uint32_t ResourceDropper<Prefab>()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
			{
				assert(payload->DataSize == sizeof(Metafile*));
				Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
				if (incoming_resource_metafile->resource_type == ResourceType::PREFAB)
				{
					return incoming_resource_metafile->uuid;
				}
			}
			ImGui::EndDragDropTarget();
		}
		return 0;
	};

	template<>
	static uint32_t ResourceDropper<Skeleton>()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
			{
				assert(payload->DataSize == sizeof(Metafile*));
				Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
				if (incoming_resource_metafile->resource_type == ResourceType::SKELETON)
				{
					return incoming_resource_metafile->uuid;
				}
			}
			ImGui::EndDragDropTarget();
		}
		return 0;
	};

	template<>
	static uint32_t ResourceDropper<Skybox>()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
			{
				assert(payload->DataSize == sizeof(Metafile*));
				Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
				if (incoming_resource_metafile->resource_type == ResourceType::SKYBOX)
				{
					return incoming_resource_metafile->uuid;
				}
			}
			ImGui::EndDragDropTarget();
		}
		return 0;
	};

	template<>
	static uint32_t ResourceDropper<StateMachine>()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
			{
				assert(payload->DataSize == sizeof(Metafile*));
				Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
				if (incoming_resource_metafile->resource_type == ResourceType::STATE_MACHINE)
				{
					return incoming_resource_metafile->uuid;
				}
			}
			ImGui::EndDragDropTarget();
		}
		return 0;
	};

	template<>
	static uint32_t ResourceDropper<Texture>()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
			{
				assert(payload->DataSize == sizeof(Metafile*));
				Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
				if (incoming_resource_metafile->resource_type == ResourceType::TEXTURE)
				{
					return incoming_resource_metafile->uuid;
				}
			}
			ImGui::EndDragDropTarget();
		}
		return 0;
	};

	template<>
	static uint32_t ResourceDropper<SoundBank>()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
			{
				assert(payload->DataSize == sizeof(Metafile*));
				Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
				if (incoming_resource_metafile->resource_type == ResourceType::SOUND)
				{
					return incoming_resource_metafile->uuid;
				}
			}
			ImGui::EndDragDropTarget();
		}
		return 0;
	};

	template<>
	static uint32_t ResourceDropper<Video>()
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("DND_Resource"))
			{
				assert(payload->DataSize == sizeof(Metafile*));
				Metafile* incoming_resource_metafile = *(Metafile**)payload->Data;
				if (incoming_resource_metafile->resource_type == ResourceType::VIDEO)
				{
					return incoming_resource_metafile->uuid;
				}
			}
			ImGui::EndDragDropTarget();
		}
		return 0;
	};
}


#endif // _IMGUIHELPER_H_