#pragma once

#include <vector>
#include <string>
#include "StlAllocator.h"
#include "Renderer.h"

// 選択肢を表示し、結果を取得するクラス
class Choices
{
public:
	Choices(std::vector<std::string, StlAllocator<std::string>> choices);
	int choices_loop(short vertical_margin, Renderer& renderer);  // 選択肢を表示し、結果を取得する。戻り値の値は、選択肢の表示順と対応 zero-based
	short get_choices_num();                                      // 選択肢の総数を取得

private:
	std::vector<std::string, StlAllocator<std::string>> choices;
	short get_strings_max_len();  // 選択肢文字列の中で、最も長い文字列の文字列数を返す
};