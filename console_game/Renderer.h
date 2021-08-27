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
	static const short MAX_WIDTH = 120;               // �ő剡��
	static const short MAX_HEIGHT = 30;               // �ő�c��
	const COORD BUFFER_SIZE;                          // �o�b�t�@�[�T�C�Y
	const COORD START_COORD;                          // �`��J�n�ʒu
	const CONSOLE_CURSOR_INFO INVISIBLE_CURSOR_INFO;  // �s���J�[�\���̈���
	const SMALL_RECT WINDOW_SIZE;                     // ��ʃT�C�Y
	CHAR_INFO screen_buffer[MAX_HEIGHT][MAX_WIDTH];   // �X�N���[���o�b�t�@
};