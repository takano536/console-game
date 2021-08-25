#pragma once

#include <iostream>
#include <memory>
#include "LinearAllocator.h"

extern std::unique_ptr<iMemoryAllocator> custom_allocator;  // LinearAllocator.cpp �ɃC���X�^���X�L��

// unique_ptr �Ŏ���A���P�[�^���g�p���邽�߂̃e���v���[�g
template <typename T>
struct uniquePtrDeleter
{
	void operator()(T* ptr) const
	{
		std::cout << "delete" << std::endl;
		custom_allocator->Free(ptr);
	}
};