#include <iostream>
#include <windows.h>
#include "scene.h"

int main()
{
	bool is_continue = true;
	while (is_continue)
	{
		switch (title_loop())
		{
			case 0:
				game_loop();
				break;

			case 1:
				std::cout << "option" << std::endl;
				Sleep(3000);
				break;

			case 2:
				is_continue = false;
				break;

			default:
				break;
		}
	}
	return 0;
}