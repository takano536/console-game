#pragma once

//! �������A���P�[�^�@�C���^�[�t�F�[�X
class iMemoryAllocator
{
public:
	virtual ~iMemoryAllocator() {}

	virtual void* Alloc(const size_t size) = 0;
	virtual void Free(void* ptr) = 0;
};