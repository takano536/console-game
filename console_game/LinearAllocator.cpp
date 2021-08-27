#include "LinearAllocator.h"

#include <iostream>
#include <functional>
#include <chrono>
using namespace std::chrono;

LinearAllocator::LinearAllocator(void* ptr, const size_t size)
{
	// TODO ここを適切に実装してください

	buffer_info = static_cast<BufferInfo*>(ptr);
	buffer_info->top_ptr = ptr;
	buffer_info->bottom_ptr = reinterpret_cast<void*>(reinterpret_cast<char*>(ptr) + size);
	buffer_info->size = size;
	buffer_info->received_time = system_clock::now();
	buffer_info->block_total = 1;

	top_cell = reinterpret_cast<InfoCell*>(reinterpret_cast<char*>(ptr) + sizeof(BufferInfo));
	top_cell->block_info.size = size;
	top_cell->block_info.is_used = false;
	top_cell->debug_info.alloc_ptr = nullptr;
	top_cell->debug_info.memory_size = 0;
	top_cell->next = nullptr;
	top_cell->prev = nullptr;
}

void* LinearAllocator::Alloc(const size_t size)
{
	// TODO ここを適切に実装してください

	// 適切な空きブロックの検索
	InfoCell* allocate_ptr = static_cast<InfoCell*>(search_free_block(size));
	if (allocate_ptr == nullptr)
		return nullptr;

	// メモリブロックの分割
	InfoCell* extra_ptr = reinterpret_cast<InfoCell*>(reinterpret_cast<char*>(allocate_ptr) + sizeof(InfoCell) + size);  // 分割して余ったメモリブロックの先頭アドレス
	extra_ptr->block_info.size = allocate_ptr->block_info.size - sizeof(InfoCell) - size;
	extra_ptr->block_info.is_used = false;
	extra_ptr->debug_info.alloc_ptr = nullptr;
	extra_ptr->debug_info.memory_size = 0;
	extra_ptr->next = allocate_ptr->next;
	extra_ptr->prev = allocate_ptr;
	allocate_ptr->next = extra_ptr;  // 割り当て予定のメモリブロックの next のリンク
	if (allocate_ptr->next->next)
	{
		// 分割したメモリブロックの次のブロックの prev のリンク
		allocate_ptr->next->next->prev = extra_ptr;
	}

	// 情報の埋め込み
	allocate_ptr->block_info.size = sizeof(InfoCell) + size;
	allocate_ptr->block_info.is_used = true;
	allocate_ptr->debug_info.alloc_ptr = reinterpret_cast<void*>(reinterpret_cast<char*>(allocate_ptr) + sizeof(InfoCell));
	allocate_ptr->debug_info.memory_size = size;
	allocate_ptr->debug_info.alloc_time = system_clock::now();

	buffer_info->block_total++;

	//show_debug_info();
	return allocate_ptr->debug_info.alloc_ptr;
}

void LinearAllocator::Free(void* ptr)
{
	// TODO ここを適切に実装してください

	if (ptr == nullptr)  // nullptr を許容
		return;

	if (is_invalid(ptr))  // 渡されたポインタが不正なアドレス
		return;

	InfoCell* release_ptr = reinterpret_cast<InfoCell*>(reinterpret_cast<char*>(ptr) - sizeof(InfoCell));  // 開放するメモリブロックの cell の先頭アドレス

	// メモリブロクの開放
	release_ptr->block_info.is_used = false;
	release_ptr->debug_info.alloc_ptr = nullptr;
	release_ptr->debug_info.memory_size = 0;

	//メモリブロックの結合
	if (release_ptr->prev && !release_ptr->prev->block_info.is_used && release_ptr->next && !release_ptr->next->block_info.is_used)
	{
		// 開放メモリブロックの両端を結合
		release_ptr->prev->block_info.size += release_ptr->block_info.size + release_ptr->next->block_info.size;
		release_ptr->prev->next = release_ptr->next->next;  // 結合元の next のリンク
		if (release_ptr->next->next)
		{
			// 結合元の次のブロックの prev のリンク
			release_ptr->next->next->prev = release_ptr->prev;
		}
		buffer_info->block_total -= 2;
	}
	else if (release_ptr->prev && !release_ptr->prev->block_info.is_used)
	{
		// 開放メモリブロックの前を結合
		release_ptr->prev->block_info.size += release_ptr->block_info.size;
		release_ptr->prev->next = release_ptr->next;  // 結合元の next のリンク
		if (release_ptr->next)
		{
			// 結合元の次のブロックの prev のリンク
			release_ptr->next->prev = release_ptr->prev;
		}
		buffer_info->block_total--;
	}
	else if (release_ptr->next && !release_ptr->next->block_info.is_used)
	{
		// 開放メモリブロックの次を結合
		release_ptr->block_info.size += release_ptr->next->block_info.size;
		release_ptr->next = release_ptr->next->next;  // 結合元の next のリンク
		if (release_ptr->next)
		{
			// 結合元の次のブロックの prev のリンク
			release_ptr->next->prev = release_ptr;
		}
		buffer_info->block_total--;
	}
	//show_debug_info();
}

void* LinearAllocator::search_free_block(size_t size)
{
	InfoCell* writing_limit_ptr = reinterpret_cast<InfoCell*>(reinterpret_cast<char*>(buffer_info->bottom_ptr) - sizeof(InfoCell));  // バッファの書き込み限界アドレス
	size_t required_size = size + sizeof(InfoCell) * 2;                                                                              // 確保するときに必要なサイズ

	InfoCell* cur_ptr;
	bool is_found = false;
	for (cur_ptr = top_cell; cur_ptr; cur_ptr = cur_ptr->next)
	{
		InfoCell* cur_bottom_ptr = reinterpret_cast<InfoCell*>(reinterpret_cast<char*>(cur_ptr) + sizeof(InfoCell) + size);  // cur_ptrの末端アドレス

		if (cur_bottom_ptr >= writing_limit_ptr)  //バッファの範囲外
			break;
		if (cur_ptr->block_info.is_used)  // 使用済み
			continue;
		if (required_size > cur_ptr->block_info.size)  // メモリブロックのサイズが足りない
			continue;

		is_found = true;
		break;
	}

	return is_found ? cur_ptr : nullptr;
}

bool LinearAllocator::is_invalid(void* ptr)
{
	InfoCell* release_ptr = reinterpret_cast<InfoCell*>(reinterpret_cast<char*>(ptr) - sizeof(InfoCell));                            // 開放するメモリブロックの cell の先頭アドレス
	InfoCell* writing_limit_ptr = reinterpret_cast<InfoCell*>(reinterpret_cast<char*>(buffer_info->bottom_ptr) - sizeof(InfoCell));  // バッファの書き込み限界のアドレス

	if (release_ptr < top_cell || release_ptr >= writing_limit_ptr)  // 開放するメモリブロックの cell の先頭アドレスが、buffer の範囲外
		return true;

	// 開放するメモリブロックが不正なアドレスか判定
	InfoCell* cur_ptr;
	bool is_invalid = true;
	for (cur_ptr = top_cell; cur_ptr; cur_ptr = cur_ptr->next)
	{
		if (release_ptr == cur_ptr)
		{
			// 開放するメモリブロックが不正なアドレスでない
			is_invalid = false;
		}
	}
	if (is_invalid)
		return true;
	if (!release_ptr->block_info.is_used)
		return true;

	return false;
}

void LinearAllocator::show_debug_info()
{
	InfoCell* cur_ptr;
	uint32_t cnt;
	size_t alloc_size = 0;
	size_t free_size = 0;
	for (cur_ptr = top_cell, cnt = 1; cur_ptr; cur_ptr = cur_ptr->next, cnt++)
	{
		if (cur_ptr->block_info.is_used)
		{
			auto alloc_time = duration_cast<milliseconds>(cur_ptr->debug_info.alloc_time - buffer_info->received_time).count();
			std::printf("%04d: alloc -> %#p %8zu[byte]  alloc_time=%lld\n", cnt, cur_ptr, cur_ptr->block_info.size, alloc_time);
			alloc_size += cur_ptr->block_info.size;
		}
		else
		{
			std::printf("%04d: free  -> %#p %8zu[byte]\n", cnt, cur_ptr, cur_ptr->block_info.size);
			free_size += cur_ptr->block_info.size;
		}
	}
	std::cout << "alloc: " << alloc_size << "[byte]" << std::endl;
	std::cout << "free : " << free_size << "[byte]" << std::endl;
	std::cout << "total: " << alloc_size + free_size << "[byte]" << std::endl;
	std::cout << "block: " << buffer_info->block_total << std::endl;
	std::cout << std::endl;
}

// unique_ptr<void> でデストラクタを呼ぶための構造体　意味はよく理解していない;o;
struct void_deleter
{
	std::function<void(void*)> deleter;

	template <class F>
	void_deleter(F f)
	{
		deleter = f;
	}

	void operator()(void* p) const noexcept
	{
		deleter(p);
	}
};
const size_t allocate_size = static_cast<size_t>(1 * 1024) * 1024;  // 1M[byte]確保
// unique_ptr で memory_pool を管理するから、メモリも自動で開放されるはず...?
std::unique_ptr<void, void_deleter> memory_pool(new char[allocate_size], [](void* p) { delete static_cast<void*>(p); });  // メモリプール
std::unique_ptr<iMemoryAllocator> custom_allocator(new LinearAllocator(memory_pool.get(), allocate_size));                // グローバル変数として扱う　扱い方がわからない;o;