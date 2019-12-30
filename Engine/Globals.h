#ifndef _GLOBALS_H_
#define _GLOBALS_H_

#include "UI/EngineLog.h"

#include "imgui.h"

#include <windows.h>
#include <stdio.h>

#define APP_LOG_SECTION(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_SECTION, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_INFO(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_INIT(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_INIT, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_SUCCESS(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_SUCCESS, __FILE__, __LINE__, format, __VA_ARGS__);
#define APP_LOG_ERROR(format, ...) logentry(EngineLog::LogEntrySource::APP_LOG, EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);

#define OPENGL_LOG_INFO(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define OPENGL_LOG_INIT(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_INIT, __FILE__, __LINE__, format, __VA_ARGS__);
#define OPENGL_LOG_SUCCESS(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_SUCCESS, __FILE__, __LINE__, format, __VA_ARGS__);
#define OPENGL_LOG_ERROR(format, ...) logentry(EngineLog::LogEntrySource::OPENGL_LOG, EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);

#define MYASSIMP_LOG_INFO(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_INFO, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_INIT(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_INIT, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_SUCCESS(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_SUCCESS, __FILE__, __LINE__, format, __VA_ARGS__);
#define MYASSIMP_LOG_ERROR(format, ...) logentry(EngineLog::LogEntrySource::ASSIMP_LOG, EngineLog::LogEntryType::LOG_ERROR, __FILE__, __LINE__, format, __VA_ARGS__);

void logentry(const EngineLog::LogEntrySource source, const EngineLog::LogEntryType type, const char file[], const int line, const char* format, ...);


static void HelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

enum class update_status
{
	UPDATE_CONTINUE = 1,
	UPDATE_STOP,
	UPDATE_ERROR
};

enum class Fonts
{
	FONT_FA = 0,
	FONT_FAR = 1,
	FONT_FAB = 2
};

// Paths -------
# define SHADERS_PATH "./Resources/Shaders"
# define TEXTURES_PATH "./Assets/Textures/"

# define DEFAULT_VERTEX_SHADER_PATH "./Resources/Shaders/default.vs"
# define DEFAULT_FRAGMENT_SHADER_PATH "./Resources/Shaders/default.fs"

# define PRIMITIVE_VERTEX_SHADER_PATH "./Resources/Shaders/primitive.vs"
# define PRIMITIVE_FRAGMENT_SHADER_PATH "./Resources/Shaders/primitive.fs"

# define TEXTURE_FRAGMENT_SHADER_PATH "./Resources/Shaders/texture.fs"

# define WOBBLE_VERTEX_SHADER_PATH "./Resources/Shaders/wobble.vs"
# define WOBBLE_FRAGMENT_SHADER_PATH "./Resources/Shaders/wobble.fs"

# define SKYBOX_VERTEX_SHADER_PATH "./Resources/Shaders/skybox.vs"
# define SKYBOX_FRAGMENT_SHADER_PATH "./Resources/Shaders/skybox.fs"

# define LINEPOINT_VERTEX_SHADER_PATH "./Resources/Shaders/linepoint.vs"
# define LINEPOINT_FRAGMENT_SHADER_PATH "./Resources/Shaders/linepoint.fs"

# define TEXT_VERTEX_SHADER_PATH "./Resources/Shaders/text.vs"
# define TEXT_FRAGMENT_SHADER_PATH "./Resources/Shaders/text.fs"

# define FLAT_VERTEX_SHADER_PATH "./Resources/Shaders/flat.vs"
# define FLAT_FRAGMENT_SHADER_PATH "./Resources/Shaders/flat.fs"

# define GOURAUND_VERTEX_SHADER_PATH "./Resources/Shaders/gouraund.vs"
# define GOURAUND_FRAGMENT_SHADER_PATH "./Resources/Shaders/gouraund.fs"

# define HOUSE_MODEL_PATH "./Assets/Models/baker_house/BakerHouse.fbx"
# define BUNNY_MODEL_PATH "./Assets/Models/ZomBunny/Zombunny.fbx"
# define GRID_TEXTURE_PATH "./Resources/Textures/grid.png"

# define PRIMITIVE_CUBE_PATH "./Resources/Primitives/Cube/Cube.ol"
# define PRIMITIVE_CYLINDER_PATH "./Resources/Primitives/Cylinder/Cylinder.ol"
# define PRIMITIVE_SPHERE_PATH "./Resources/Primitives/Sphere/Sphere.ol"
# define PRIMITIVE_TORUS_PATH "./Resources/Primitives/Torus/Torus.ol"

# define TMP_SCENE_PATH "./Assets/Scenes/tmp_scene.scene"

# define OPENGL_MAJOR_VERSION 4
# define OPENGL_MINOR_VERSION 0

#define FULLSCREEN false
#define RESIZABLE true
#define BORDERED true

#define VSYNC true
#define TITLE "Little Orion Engine"

#endif //_GLOBALS_H_