#include "Application.h"
#include "Module/ModuleWindow.h"
#include "Globals.h"
#include "TimerUs.h"

#include "SDL.h"

#include <stdlib.h>

#pragma comment( lib, "Libraries/SDL/libx86/SDL2.lib" )
#pragma comment( lib, "Libraries/SDL/libx86/SDL2main.lib" )

enum main_states
{
	MAIN_CREATION,
	MAIN_START,
	MAIN_UPDATE,
	MAIN_FINISH,
	MAIN_EXIT
};

Application* App = NULL;

int main(int argc, char ** argv)
{
	int main_return = EXIT_FAILURE;
	main_states state = MAIN_CREATION;
	TimerUs main_timer = TimerUs();

	while (state != MAIN_EXIT)
	{
		switch (state)
		{
		case MAIN_CREATION:

			APP_LOG_SECTION("-------------- Application Creation --------------");
			main_timer.Start();

			App = new Application();

			APP_LOG_SUCCESS("Application Creation ends. Elapsed time: %f ms", main_timer.Read() * 1000);
			state = MAIN_START;
			break;

		case MAIN_START:

			APP_LOG_SECTION("-------------- Application Init --------------");
			main_timer.Start();

			if (App->Init() == false)
			{
				APP_LOG_ERROR("Application Init exits with error -----");
				state = MAIN_EXIT;
			}
			else
			{
				APP_LOG_SUCCESS("Application Init ends. Elapsed time: %f ms", main_timer.Read() * 1000);
				state = MAIN_UPDATE;
				APP_LOG_SECTION("-------------- Application Update --------------");
			}
			break;

		case MAIN_UPDATE:
		{
			update_status update_return = App->Update();
			if (update_return == update_status::UPDATE_ERROR)
			{
				APP_LOG_ERROR("Application Update exits with error -----");
				state = MAIN_EXIT;
			}

			if (update_return == update_status::UPDATE_STOP)
			{
				state = MAIN_FINISH;
			}
		}
			break;

		case MAIN_FINISH:

			APP_LOG_SECTION("-------------- Application CleanUp --------------");
			main_timer.Start();
			if (App->CleanUp() == false)
			{
				APP_LOG_ERROR("Application CleanUp exits with error -----");
			}
			else
			{
				APP_LOG_SUCCESS("Application CleanUp ends. Elapsed time: %f ms", main_timer.Read() * 1000);
				main_return = EXIT_SUCCESS;
			}

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	return main_return;
}
