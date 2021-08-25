#pragma once

//! メモリアロケータ　インターフェース
class iMemoryAllocator
{
public:
	virtual ~iMemoryAllocator() {}

	virtual void* Alloc(const size_t size) = 0;
	virtual void Free(void* ptr) = 0;
};