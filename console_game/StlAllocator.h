#pragma once

#include <memory>
#include <functional>
#include <vector>
#include "LinearAllocator.h"

// ���Ȃ��΂����Ƃ��Ă���...

extern std::unique_ptr<iMemoryAllocator> custom_allocator;  // LinearAllocator.cpp �ɃC���X�^���X�L��

// STL �R���e�i�Ɏ���A���P�[�^���g�p���邽�߂̃e���v���[�g
template <class T>
struct StlAllocator
{
	// �v�f�̌^
	using value_type = T;

	// ����֐� (�f�t�H���g�R���X�g���N�^, �R�s�[�R���X�g���N�^, ���[�u�R���X�g���N�^)
	StlAllocator()
	{}

	// �ʂȗv�f�^�̃A���P�[�^���󂯎��R���X�g���N�^
	template <class U>
	StlAllocator(const StlAllocator<U>&)
	{}

	// �������m��
	T* allocate(std::size_t n)
	{
		return reinterpret_cast<T*>(custom_allocator->Alloc((size_t)sizeof(T) * n));
	}

	// ���������
	void deallocate(T* p, std::size_t n)
	{
		static_cast<void>(n);
		custom_allocator->Free(p);
	}
};

// ��r���Z�q
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
