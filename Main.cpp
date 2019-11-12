#include <stdlib.h>
#include "Engine/Application.h"
#include "Engine/ModuleWindow.h"
#include "Engine/Globals.h"
#include "Engine/TimerUs.h"

#include "SDL.h"
#pragma comment( lib, "Libraries/SDL/libx86/SDL2.lib" )
#pragma comment( lib, "Libraries/SDL/libx86/SDL2main.lib" ) //TODO: Why is this needed?

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

			LOG("Application Creation --------------");
			main_timer.Start();

			App = new Application();

			LOG("Application Creation ends. Elapsed time: %f ms", main_timer.Read() * 1000);
			state = MAIN_START;
			break;

		case MAIN_START:

			LOG("Application Init --------------");
			main_timer.Start();

			if (App->Init() == false)
			{
				LOG("Application Init exits with error -----");
				state = MAIN_EXIT;
			}
			else
			{
				LOG("Application Init ends. Elapsed time: %f ms", main_timer.Read() * 1000);
				state = MAIN_UPDATE;
				LOG("Application Update --------------");
			}

			break;

		case MAIN_UPDATE:
		{
			int update_return = App->Update();
			if (update_return == UPDATE_ERROR)
			{
				LOG("Application Update exits with error -----");
				state = MAIN_EXIT;
			}

			if (update_return == UPDATE_STOP)
				state = MAIN_FINISH;
		}
			break;

		case MAIN_FINISH:

			LOG("Application CleanUp --------------");
			main_timer.Start();
			if (App->CleanUp() == false)
			{
				LOG("Application CleanUp exits with error -----");
			}
			else
			{
				LOG("Application CleanUp ends. Elapsed time: %f ms", main_timer.Read() * 1000);
				main_return = EXIT_SUCCESS;
			}

			state = MAIN_EXIT;

			break;

		}
	}

	delete App;
	return main_return;
}
