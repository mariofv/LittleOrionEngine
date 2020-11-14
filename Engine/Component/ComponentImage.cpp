#include "ComponentImage.h"

#include "Main/GameObject.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleUI.h"

#include "ResourceManagement/Resources/Texture.h"

ComponentImage::ComponentImage() : Component(ComponentType::UI_IMAGE)
{
	InitData();
}

ComponentImage::ComponentImage(GameObject * owner) : Component(owner, ComponentType::UI_IMAGE)
{
	InitData();
}


void ComponentImage::InitData()
{
	quad = App->ui->quad.get();
}

void ComponentImage::Render(float4x4* projection)
{

	if (!active)
	{
		return;
	}

	if (texture_to_render != nullptr)
	{
		ScaleOp aspect_ratio_scaling;

		if (preserve_aspect_ratio)
		{
			if (owner->transform_2d.size.x / texture_aspect_ratio > owner->transform_2d.size.y)
			{
				aspect_ratio_scaling = float4x4::Scale(float3(owner->transform_2d.size.y * texture_aspect_ratio, owner->transform_2d.size.y, 1.f));
			}
			else
			{
				aspect_ratio_scaling = float4x4::Scale(float3(owner->transform_2d.size.x, owner->transform_2d.size.x / texture_aspect_ratio, 1.f));
			}
		}
		else
		{
			aspect_ratio_scaling = float4x4::Scale(float3(owner->transform_2d.size, 1.f));
		}
		float4x4 model = owner->transform_2d.GetGlobalModelMatrix()  * aspect_ratio_scaling;

		if (program == 0)
		{
			program = App->program->UseProgram("Sprite");
		}
		else
		{
			glUseProgram(program);
		}

		glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_TRUE, projection->ptr());
		glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model.ptr());
		glUniform4fv(glGetUniformLocation(program, "spriteColor"), 1, color.ptr());
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_to_render->opengl_texture);
		glUniform1i(glGetUniformLocation(program, "image"), 0);

		quad->RenderArray();
		glBindTexture(GL_TEXTURE_2D, 0);
		glUseProgram(0);
	}
}

Component* ComponentImage::Clone(GameObject* owner, bool original_prefab)
{
	ComponentImage * created_component;
	if (original_prefab)
	{
		created_component = new ComponentImage();
	}
	else
	{
		created_component = App->ui->CreateComponentUI<ComponentImage>();
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));

	created_component->owner = owner;
	created_component->owner->components.push_back(created_component);

	created_component->ReassignResource();

	return created_component;
};

void ComponentImage::CopyTo(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentImage*>(component_to_copy) = *this;
}


void ComponentImage::Delete()
{
	App->ui->RemoveComponentUI(this);
}

void ComponentImage::SpecializedSave(Config& config) const
{
	config.AddColor(color, "ImageColor");
	config.AddUInt(texture_uuid, "TextureUUID");
	config.AddBool(preserve_aspect_ratio, "PreserveAspectRatio");
}

void ComponentImage::SpecializedLoad(const Config& config)
{
	config.GetColor("ImageColor", color, float4::one);
	texture_uuid = config.GetUInt32("TextureUUID", 0);
	preserve_aspect_ratio = config.GetBool("PreserveAspectRatio", false);
	if (texture_uuid != 0)
	{
		SetTextureToRender(texture_uuid);
	}
}

void ComponentImage::LoadResource(uint32_t uuid, ResourceType resource)
{

	texture_to_render = std::static_pointer_cast<Texture>(App->resources->RetrieveFromCacheIfExist(uuid));

	if(texture_to_render)
	{
		return;
	}

	FileData file_data;
	bool succes = App->resources->RetrieveFileDataByUUID(uuid, file_data);
	if(succes)
	{
		//THINK WHAT TO DO IF IS IN CACHE
		texture_to_render = ResourceManagement::Load<Texture>(uuid, file_data, true);
		//Delete file data buffer
		delete[] file_data.buffer;
		App->resources->AddResourceToCache(std::static_pointer_cast<Resource>(texture_to_render));
		texture_aspect_ratio = (float)texture_to_render->width / texture_to_render->height;
	}

}

void ComponentImage::InitResource(uint32_t uuid, ResourceType resource)
{
	if(texture_to_render && !texture_to_render.get()->initialized)
	{
		texture_to_render.get()->LoadInMemory();
	}
}

void ComponentImage::ReassignResource()
{
	if(texture_uuid != 0)
	{
		SetTextureToRender(texture_uuid);
	}
}

void ComponentImage::SetTextureToRender(uint32_t texture_uuid)
{
	//Prepare multithreading loading
	App->resources->loading_thread_communication.current_component_loading = this;
	App->resources->loading_thread_communication.current_type = ResourceType::TEXTURE;
	this->texture_uuid = texture_uuid;
	texture_to_render = App->resources->Load<Texture>(texture_uuid);
	if (texture_to_render)
	{
		texture_aspect_ratio = (float)texture_to_render->width / texture_to_render->height;

	}
	//Set to default loading component
	App->resources->loading_thread_communication.current_component_loading = nullptr;
}

void ComponentImage::SetTextureToRenderFromInspector(uint32_t texture_uuid)
{
	this->texture_uuid = texture_uuid;

	App->resources->loading_thread_communication.normal_loading_flag = true;
	texture_to_render = App->resources->Load<Texture>(texture_uuid);
	App->resources->loading_thread_communication.normal_loading_flag = false;
}

void ComponentImage::SetColor(float4 color)
{
	this->color = color;
}

void ComponentImage::SetNativeSize() const
{
	if(texture_to_render)
	{
		owner->transform_2d.SetSize(float2(texture_to_render->width, texture_to_render->height));
	}
}