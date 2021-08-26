#include <cstdint>
#include <windows.h>
#include "Renderer.h"

Renderer::Renderer()
    : BUFFER_SIZE({MAX_WIDTH, MAX_HEIGHT}),
      START_COORD({0, 0}),
      INVISIBLE_CURSOR_INFO({1, FALSE}),
      WINDOW_SIZE({0, 0, MAX_WIDTH - 1, MAX_HEIGHT - 1})
{
	for (int i = 0; i < MAX_HEIGHT; i++)
	{
		for (int j = 0; j < MAX_WIDTH; j++)
		{
			screen_buffer[i][j].Char.AsciiChar = ' ';
			screen_buffer[i][j].Char.UnicodeChar = ' ';
			screen_buffer[i][j].Attributes = 0;
		}
	}
}

void Renderer::init_window()
{
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &INVISIBLE_CURSOR_INFO);  // カーソルを不可視化
	SetConsoleScreenBufferSize(GetStdHandle(STD_OUTPUT_HANDLE), BUFFER_SIZE);       // バッファサイズの指定
	SetConsoleWindowInfo(GetStdHandle(STD_OUTPUT_HANDLE), TRUE, &WINDOW_SIZE);      // ウインドウサイズの指定
}

void Renderer::init_screen_buffer()
{
	for (int i = 0; i < MAX_HEIGHT; i++)
	{
		for (int j = 0; j < MAX_WIDTH; j++)
		{
			screen_buffer[i][j].Char.AsciiChar = ' ';
			screen_buffer[i][j].Char.UnicodeChar = ' ';
			screen_buffer[i][j].Attributes = 0;
		}
	}
}

void Renderer::set_char(short x, short y, char character, Color foreground_color, Color background_color)
{
	screen_buffer[y][x].Char.AsciiChar = character;
	screen_buffer[y][x].Attributes = static_cast<WORD>(foreground_color) + static_cast<WORD>(background_color) * 16;
}

void Renderer::set_string(short x, short y, const std::string& str, Color foreground_color, Color background_color)
{
	short len = str.size();
	for (int i = 0; i < len; i++)
	{
		screen_buffer[y][x + i].Char.AsciiChar = str[i];
		screen_buffer[y][x + i].Attributes = static_cast<WORD>(foreground_color) + static_cast<WORD>(background_color) * 16;
	}
}

void Renderer::render()
{
	WriteConsoleOutput(GetStdHandle(STD_OUTPUT_HANDLE), reinterpret_cast<CHAR_INFO*>(screen_buffer), BUFFER_SIZE, START_COORD, const_cast<SMALL_RECT*>(&WINDOW_SIZE));
}

short Renderer::get_max_width()
{
	return MAX_WIDTH;
}

short Renderer::get_max_height()
{
	return MAX_HEIGHT;
}