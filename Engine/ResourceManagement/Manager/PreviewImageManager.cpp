#include "PreviewImageManager.h"

#include "Main/Application.h"
#include "Module/ModuleCamera.h"
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

void PreviewImageManager::TakeScreenShot(const char * screenshot_save_path)
{
		ILuint imageID = App->cameras->scene_camera->fbo;
		ilBindImage(imageID);
		ilutGLScreen();
		ilEnable(IL_FILE_OVERWRITE);
		ilSaveImage(screenshot_save_path);
		ilDeleteImage(imageID);
		APP_LOG_INFO("Screenshot saved to: %s", screenshot_save_path);
}
