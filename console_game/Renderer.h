#pragma once

#include <string>
#include <windows.h>
#include "Color.h"

class Renderer final
{
public:
	Renderer();
	void init_window();  // ��ʂ̐ݒ������������B
	void init_screen_buffer();
	void set_char(short x, short y, char character, Color foreground_color, Color background_color);  // x, y �͕`��J�n�ʒu
	void set_string(short x, short y, const std::string& str, Color foreground_color, Color background_color);
	char get_char(short x, short y) const;
	void change_monochrome();  // �o�b�t�@�ɏ����ꂽ���������m�N���ɂ���
	void render();
	short get_max_width() const;
	short get_max_height() const;

private:
	static const short MAX_WIDTH = 120;               // �ő剡��
	static const short MAX_HEIGHT = 30;               // �ő�c��
	const COORD BUFFER_SIZE;                          // �o�b�t�@�[�T�C�Y
	const COORD START_COORD;                          // �`��J�n�ʒu
	const CONSOLE_CURSOR_INFO INVISIBLE_CURSOR_INFO;  // �s���J�[�\���̈���
	const SMALL_RECT WINDOW_SIZE;                     // ��ʃT�C�Y
	CHAR_INFO screen_buffer[MAX_HEIGHT][MAX_WIDTH];   // �X�N���[���o�b�t�@
};