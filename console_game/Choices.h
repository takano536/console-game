#pragma once

#include <vector>
#include <string>
#include "StlAllocator.h"
#include "Renderer.h"

// �I������\�����A���ʂ��擾����N���X
class Choices
{
public:
	Choices(std::vector<std::string, StlAllocator<std::string>> choices);
	int choices_loop(short vertical_margin, Renderer& renderer);  // �I������\�����A���ʂ��擾����B�߂�l�̒l�́A�I�����̕\�����ƑΉ� zero-based
	short get_choices_num();                                      // �I�����̑������擾

private:
	std::vector<std::string, StlAllocator<std::string>> choices;
	short get_strings_max_len();  // �I����������̒��ŁA�ł�����������̕����񐔂�Ԃ�
};