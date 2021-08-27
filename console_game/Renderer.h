#pragma once

#include <string>
#include <windows.h>
#include "Color.h"

class Renderer final
{
public:
	Renderer();
	void init_window();
	void init_screen_buffer();
	void set_char(short x, short y, char character, Color foreground_color, Color background_color);
	void set_string(short x, short y, const std::string& str, Color foreground_color, Color background_color);
	void change_monochrome();
	void render();
	short get_max_width();
	short get_max_height();

private:
	static const short MAX_WIDTH = 120;               // 最大横幅
	static const short MAX_HEIGHT = 30;               // 最大縦幅
	const COORD BUFFER_SIZE;                          // バッファーサイズ
	const COORD START_COORD;                          // 描画開始位置
	const CONSOLE_CURSOR_INFO INVISIBLE_CURSOR_INFO;  // 不可視カーソルの引数
	const SMALL_RECT WINDOW_SIZE;                     // 画面サイズ
	CHAR_INFO screen_buffer[MAX_HEIGHT][MAX_WIDTH];   // スクリーンバッファ
};