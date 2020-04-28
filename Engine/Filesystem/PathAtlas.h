#ifndef _PATHATLAS_H_
#define _PATHATLAS_H_

#define ASSETS_PATH "/Assets"
#define TEXTURES_PATH "/Assets/Textures"

#define LIBRARY_PATH "/Library"
#define LIBRARY_METADATA_PATH "/Library/Metadata"
#define WWISE_INIT_PATH "/Library/Wwise"

#define RESOURCES_PATH "/Resources"

#define SHADERS_PATH "/Resources/Shaders/shaders.json"

#define RESOURCES_SCRIPT_PATH "/Resources/Scripts"

#if DEBUG
#define RESOURCES_SCRIPT_DLL_PATH "Resources/Scripts/GameplaySystemDebug.dll"
#define RESOURCE_SCRIPT_DLL_FILE "GameplaySystemDebu_.dll"
#elif GAME
#define RESOURCES_SCRIPT_DLL_PATH "Resources/Scripts/GameplaySystem.dll"
#define RESOURCE_SCRIPT_DLL_FILE "GameplaySystem.dll"
#else
#define RESOURCES_SCRIPT_DLL_PATH "Resources/Scripts/GameplaySystemRelease.dll"
#define RESOURCE_SCRIPT_DLL_FILE "GameplaySystemReleas_.dll"
#endif
#define RESOURCES_SCRIPT_LIST_FILENAME "scripts_list.scripts"
#define WWISE_INIT_NAME "Init.bnk"

#define RESOURCES_SCRIPT_TEMPLATE_CPP "/Resources/Scripts/TemplateScript.cpp"
#define RESOURCES_SCRIPT_TEMPLATE_H "/Resources/Scripts/TemplateScript.h"
#define SCRIPT_PATH "../GamePlaySystem/src/Script/"


#define RESOURCES_GAME_INPUTS_PATH "/Resources/GameInputs"
#define RESOURCES_GAME_INPUTS_FILENAME "game_inputs.inputs"

#define RESOURCES_NAVMESH_PATH "/Resources/NavMesh"

#define RESOURCES_BUILD_OPTIONS "/Resources/Scenes/build.options"

#define DEFAULT_SCENE_PATH "/Resources/Scenes/default_scene.scene"
#define SAVED_SCENE_PATH "/Assets/Scenes/scene.scene"
#define TMP_SCENE_PATH "/Assets/Scenes/tmp_scene.scene"
#define SCENE_ROOT_PATH "/Assets/Scenes"


#endif // !_PATHATLAS_H_