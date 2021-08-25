#pragma once

#include <iostream>
#include <memory>
#include "LinearAllocator.h"

extern std::unique_ptr<iMemoryAllocator> custom_allocator;  // LinearAllocator.cpp にインスタンス有り

// unique_ptr で自作アロケータを使用するためのテンプレート
template <typename T>
struct uniquePtrDeleter
{
	void operator()(T* ptr) const
	{
		std::cout << "delete" << std::endl;
		custom_allocator->Free(ptr);
	}
};