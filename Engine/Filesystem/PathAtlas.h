#ifndef _PATHATLAS_H_
#define _PATHATLAS_H_

#define ASSETS_PATH "/Assets"
#define TEXTURES_PATH "/Assets/Textures"

#define LIBRARY_PATH "/Library"
#define LIBRARY_METADATA_PATH "/Library/Metadata"
#define WWISE_INIT_PATH "/Library/Wwise"
#define WWISE_INIT_NAME "Init.bnk"

#define RESOURCES_PATH "/Resources"

#define SHADERS_PATH "/Resources/Shaders/shaders.json"

#define RESOURCE_SCRIPT_DLL_FILE "GameplaySystem.dll"
#define RESOURCES_SCRIPT_PATH "Resources/Scripts"
#if DEBUG
#define RESOURCES_SCRIPT_DLL_PATH "Resources/Scripts/GameplaySystemDebug.dll"
#define COMPILED_FOLDER_DLL_PATH "/Assets/Scripts/Binaries/Debug"
#define COMPILED_SCRIPT_DLL_PATH "/Assets/Scripts/Binaries/Debug/GameplaySystemDebug.dll"
#define COMMAND_FOR_COMPILING "\\MSBuild.exe\" Assets\\Scripts\\GameplaySystem.vcxproj /t:Build /p:Configuration=Debug /p:Platform=x86"
#elif GAME
#define RESOURCES_SCRIPT_DLL_PATH "Resources/Scripts/GameplaySystem.dll"
#else
#define RESOURCES_SCRIPT_DLL_PATH "Resources/Scripts/GameplaySystemRelease.dll"
#define COMPILED_FOLDER_DLL_PATH "/Assets/Scripts/Binaries/Release"
#define COMPILED_SCRIPT_DLL_PATH "/Assets/Scripts/Binaries/Release/GameplaySystemRelease.dll"
#define COMMAND_FOR_COMPILING "\\MSBuild.exe\" Assets\\Scripts\\GameplaySystem.vcxproj /t:Build /p:Configuration=Release /p:Platform=x86"
#endif

#define RESOURCES_SCRIPT_TEMPLATE_CPP "/Resources/Scripts/TemplateScript.cpp"
#define RESOURCES_SCRIPT_TEMPLATE_H "/Resources/Scripts/TemplateScript.h"
#define SCRIPT_PATH "Assets/Scripts/src/Script"


#define RESOURCES_GAME_INPUTS_PATH "/Resources/GameInputs"
#define RESOURCES_GAME_INPUTS_FILENAME "game_inputs.inputs"

#define RESOURCES_NAVMESH_PATH "/Resources/NavMesh"

#define RESOURCES_BUILD_OPTIONS "/Resources/Scenes/build.options"
#define PROJECT_TAGS_PATH "/Assets/ProjectSettings/tags.options"

#define DEFAULT_SCENE_PATH "/Resources/Scenes/default_scene.scene"
#define SAVED_SCENE_PATH "/Assets/Scenes/scene.scene"
#define TMP_SCENE_PATH "/Resources/Scenes/tmp_scene.scene"
#define SCENE_ROOT_PATH "/Resources/Scenes"


#endif // !_PATHATLAS_H_