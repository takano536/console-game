#include "option.h"

#include <string>
#include <cstring>
#include <windows.h>
#include "Renderer.h"
#include "Choices.h"
#include "Color.h"

static constexpr LPCTSTR filepath = L"./settings.ini";

void update_game_level()
{
	Renderer renderer;
	renderer.init_window();

	Choices choices({" EASY ", "NORMAL", " HARD ", "INSANE"});
	int level = choices.choices_loop(static_cast<short>(renderer.get_max_height() - choices.get_choices_num()) * 0.5, renderer) - 1;
	TCHAR str[8];
	wsprintf(str, TEXT("%d"), level);
	WritePrivateProfileString(L"game", L"level", str, filepath);

	renderer.init_screen_buffer();
	std::string message = "SUCCESSFULLY CHANGED THE DIFFICULTY";
	renderer.set_string(static_cast<short>((renderer.get_max_width() - message.size()) * 0.5), static_cast<short>(renderer.get_max_height() * 0.5), message, Color::WHITE, Color::BLACK);
	Choices back({"BACK"});
	back.choices_loop(5, renderer);
}

void update_movement_speed()
{
	Renderer renderer;
	renderer.init_window();

	Choices choices({" SLOW ", "NORMAL", " FAST "});
	int speed = choices.choices_loop(static_cast<short>(renderer.get_max_height() - choices.get_choices_num()) * 0.5, renderer) - 1;
	TCHAR str[8];
	wsprintf(str, TEXT("%d"), speed);
	WritePrivateProfileString(L"game", L"speed", str, filepath);

	renderer.init_screen_buffer();
	std::string message = "SUCCESSFULLY CHANGED THE MOVEMENT SPEED";
	renderer.set_string(static_cast<short>((renderer.get_max_width() - message.size()) * 0.5), static_cast<short>(renderer.get_max_height() * 0.5), message, Color::WHITE, Color::BLACK);
	Choices back({"BACK"});
	back.choices_loop(5, renderer);
}
