// CommandLineProject.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <iostream>
#include "Main/Application.h"

int main()
{
	Application* App = new Application();

	delete App;
	return 0;
}
