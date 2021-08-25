#pragma once

#include <cstdint>
#include <chrono>
#include "iMemoryAllocator.h"
using namespace std::chrono;

class LinearAllocator : public iMemoryAllocator
{
public:
	LinearAllocator(void* ptr, const size_t size);

	virtual ~LinearAllocator(){};

	void* Alloc(const size_t size);
	void Free(void* ptr);

private:
	// TODO ここを適切に実装してください

	struct BufferInfo  // 受け取ったバッファの情報
	{
		void* top_ptr;                           // バッファの先端アドレス
		void* bottom_ptr;                        // バッファの終端アドレス
		size_t size;                             // バッファの総サイズ
		system_clock::time_point received_time;  // バッファを受け取った時間
		uint32_t block_total;                    // メモリブロックの総数
	};

	struct BlockInfo  // メモリブロックの情報
	{
		size_t size;   // メモリブロックのサイズ
		bool is_used;  // 割り当てられているかどうか
	};

	struct DebugInfo  // デバッグ情報
	{
		void* alloc_ptr;                      // 実際に使用されるメモリの先頭ポインタ
		size_t memory_size;                   // メモリサイズ
		system_clock::time_point alloc_time;  // メモリを確保したときの時間
		uint32_t kind;                        // 種類
		uint32_t usage;                       // 利用方法
		uint32_t user;                        // 利用者
	};

	struct InfoCell  // 管理情報を持ったセル
	{
		BlockInfo block_info;  // メモリブロックの情報
		DebugInfo debug_info;  // デバッグ情報
		InfoCell* next;        // 次のリンクポインタ
		InfoCell* prev;        // 前のリンクポインタ
	};

	BufferInfo* buffer_info;  // バッファの管理情報
	InfoCell* top_cell;       // 双方向リンクリスト

	void* search_free_block(size_t size);
	bool is_invalid(void* ptr);
	void show_debug_info();
};
