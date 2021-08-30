#pragma once

#include <memory>
#include <functional>
#include <vector>
#include "LinearAllocator.h"

// かなりやばいことしてそう...

extern std::unique_ptr<iMemoryAllocator> custom_allocator;  // LinearAllocator.cpp にインスタンス有り

// STL コンテナに自作アロケータを使用するためのテンプレート
template <class T>
struct StlAllocator
{
	// 要素の型
	using value_type = T;

	// 特殊関数 (デフォルトコンストラクタ, コピーコンストラクタ, ムーブコンストラクタ)
	StlAllocator()
	{}

	// 別な要素型のアロケータを受け取るコンストラクタ
	template <class U>
	StlAllocator(const StlAllocator<U>&)
	{}

	// メモリ確保
	T* allocate(std::size_t n)
	{
		return reinterpret_cast<T*>(custom_allocator->Alloc((size_t)sizeof(T) * n));
	}

	// メモリ解放
	void deallocate(T* p, std::size_t n)
	{
		static_cast<void>(n);
		custom_allocator->Free(p);
	}
};

// 比較演算子
template <class T, class U>
bool operator==(const StlAllocator<T>&, const StlAllocator<U>&)
{
	return true;
}
template <class T, class U>
bool operator!=(const StlAllocator<T>&, const StlAllocator<U>&)
{
	return false;
}
