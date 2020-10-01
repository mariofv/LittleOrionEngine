#include "ComponentVideoPlayer.h"

#include "Helper/Quad.h"

#include "Main/GameObject.h"
#include "Module/ModuleResourceManager.h"
#include "Module/ModuleProgram.h"
#include "Module/ModuleUI.h"

#include "ResourceManagement/Resources/Video.h"
#include "ResourceManagement/Resources/SoundBank.h"


ComponentVideoPlayer::ComponentVideoPlayer() : Component(ComponentType::VIDEO_PLAYER)
{
	Init();
}

ComponentVideoPlayer::ComponentVideoPlayer(GameObject * owner) : Component(owner, ComponentType::VIDEO_PLAYER)
{
	Init();
}

ComponentVideoPlayer::~ComponentVideoPlayer()
{
	StopVideo();
	AK::SoundEngine::UnregisterGameObj(gameobject_source);
}


void ComponentVideoPlayer::Init()
{
	quad = App->ui->quad.get();
	AK::SoundEngine::RegisterGameObj(gameobject_source);
}
void ComponentVideoPlayer::Render(float4x4* projection)
{
	if (!active)
	{
		return;
	}

	if (playing_video)
	{
		GLuint opengl_video_frame = video_to_render->GenerateFrame();
		playing_video = opengl_video_frame > 0 ? true : false;
		if (playing_video)
		{
			RenderTexture(projection, opengl_video_frame);
		}
		else
		{
			StopVideo();
		}
	}
}

void ComponentVideoPlayer::RenderTexture(math::float4x4 * projection, GLuint texture)
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
	glUniform1i(glGetUniformLocation(program, "image"), 0);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_TRUE, model.ptr());
	glUniform4fv(glGetUniformLocation(program, "spriteColor"), 1, color.ptr());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);

	quad->RenderArray();
	glBindTexture(GL_TEXTURE_2D, 0);
	glUseProgram(0);
}

Component* ComponentVideoPlayer::Clone(GameObject* owner, bool original_prefab)
{
	ComponentVideoPlayer * created_component;
	if (original_prefab)
	{
		created_component = new ComponentVideoPlayer();
	}
	else
	{
		created_component = App->ui->CreateComponentUI<ComponentVideoPlayer>();
	}
	*created_component = *this;
	CloneBase(static_cast<Component*>(created_component));

	created_component->ReassignResource();

	return created_component;
};

void ComponentVideoPlayer::CopyTo(Component* component_to_copy) const
{
	*component_to_copy = *this;
	*static_cast<ComponentVideoPlayer*>(component_to_copy) = *this;
}

void ComponentVideoPlayer::InitResource(uint32_t uuid, ResourceType resource)
{
	if (video_to_render && !video_to_render.get()->initialized)
	{
		video_to_render.get()->LoadInMemory();
	}
}


void ComponentVideoPlayer::Delete()
{
	App->ui->RemoveComponentUI(this);
}

void ComponentVideoPlayer::SpecializedSave(Config& config) const
{
	config.AddColor(color, "ImageColor");
	config.AddUInt(video_uuid, "VideoUUID");
	config.AddString(sound_event, "SoundEvent");
	config.AddBool(preserve_aspect_ratio, "PreserveAspectRatio");
	if (soundbank)
	{
		config.AddUInt(soundbank->GetUUID(), "AudioUUID");
	}
}

void ComponentVideoPlayer::SpecializedLoad(const Config& config)
{
	config.GetColor("ImageColor", color, float4::one);
	preserve_aspect_ratio = config.GetBool("PreserveAspectRatio", false);
	config.GetString("SoundEvent", sound_event, {});
	video_uuid = config.GetUInt32("VideoUUID", 0);
	if (video_uuid != 0)
	{
		SetVideoToRender(video_uuid);
	}

	uint32_t soundbank_uuid = config.GetUInt32("AudioUUID", 0);
	if (soundbank_uuid != 0)
	{
		SetSoundBank(soundbank_uuid);
	}
}

void ComponentVideoPlayer::LoadResource(uint32_t uuid, ResourceType resource)
{

	video_to_render = std::static_pointer_cast<Video>(App->resources->RetrieveFromCacheIfExist(uuid));

	if (video_to_render)
	{
		return;
	}

	FileData file_data;
	bool success = App->resources->RetrieveFileDataByUUID(uuid, file_data);
	if (success)
	{
		//THINK WHAT TO DO IF IS IN CACHE
		video_to_render = ResourceManagement::Load<Video>(uuid, file_data, true);
		//Delete file data buffer
		delete[] file_data.buffer;
		App->resources->AddResourceToCache(std::static_pointer_cast<Resource>(video_to_render));
	}

}

void ComponentVideoPlayer::ReassignResource()
{
	if (video_uuid != 0)
	{
		SetVideoToRender(video_uuid);
	}
}

void ComponentVideoPlayer::SetVideoToRender(uint32_t video_uuid)
{
	//Prepare multithreading loading
	App->resources->loading_thread_communication.current_component_loading = this;
	App->resources->loading_thread_communication.current_type = ResourceType::VIDEO;
	this->video_uuid = video_uuid;
	video_to_render = App->resources->Load<Video>(video_uuid);

	//Set to default loading component
	App->resources->loading_thread_communication.current_component_loading = nullptr;
}

void ComponentVideoPlayer::SetVideoToRenderFromInspector(uint32_t video_uuid)
{
	this->video_uuid = video_uuid;
	App->resources->loading_thread_communication.normal_loading_flag = true;
	video_to_render = App->resources->Load<Video>(video_uuid);
	App->resources->loading_thread_communication.normal_loading_flag = false;
}

void ComponentVideoPlayer::SetSoundBank(uint32_t uuid)
{
	soundbank = App->resources->Load<SoundBank>(uuid);
}

void ComponentVideoPlayer::PlayVideo()
{
	if (App->time->isGameRunning())
	{
		StopVideo();
		playing_video = video_to_render != nullptr;
		if (soundbank)
		{
			playing_id = AK::SoundEngine::PostEvent(sound_event.c_str(), gameobject_source);
		}
	}
}

ENGINE_API void ComponentVideoPlayer::StopVideo()
{
	playing_video = false;
	video_to_render->Stop();
	AK::SoundEngine::StopPlayingID(playing_id);
}

ENGINE_API bool ComponentVideoPlayer::IsFinish()
{
	return !playing_video;
}

