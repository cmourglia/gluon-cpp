#pragma once

#include <gluon/lang/cell.h>

#include <beard/containers/array.h>
#include <beard/core/macros.h>

#include <memory>

class ZInterpreter;
struct ZCell;

struct ZHeapBlock
{
	static constexpr usize BLOCK_SIZE = 16_KB;

	NONCOPYABLE(ZHeapBlock);
	NONMOVEABLE(ZHeapBlock);

	explicit ZHeapBlock(usize cellSize);
	~ZHeapBlock() = default;

	usize CellCount() const
	{
		return (BLOCK_SIZE - sizeof(ZHeapBlock)) / m_CellSize;
	}

	ZCell* Cell(usize index)
	{
		return reinterpret_cast<ZCell*>(&m_Storage[index * m_CellSize]);
	}

	usize CellSize() const
	{
		return m_CellSize;
	}

	ZCell* Allocate();
	void   Deallocate(ZCell* cell);

private:
	struct FreeListItem : public ZCell
	{
		FreeListItem* next = nullptr;
	};

	usize m_CellSize = 0;

	// This stuff works because we initialize the `next` pointer when we
	// initialize the ZHeapBlock and when we Deallocate the cell.
	// Then, when we return a cell, we call the constructor (placement new)
	// on the given memory.
	// Hence, the same memory space contains two different (although related)
	// items.
	FreeListItem* m_FreeList = nullptr;

	VARIABLE_LENGTH_ARRAY(u8, m_Storage);
};

class ZHeap
{
public:
	NONCOPYABLE(ZHeap);
	NONMOVEABLE(ZHeap);

	explicit ZHeap(ZInterpreter* interpreter);
	~ZHeap() = default;

	template <typename T, typename... Args>
	T* Allocate(Args&&... args)
	{
		void* memory = AllocateCell(sizeof(T));
		new (memory) T(std::forward<Args>(args)...);
		return reinterpret_cast<T*>(memory);
	}

	void Garbage();

private:
	void* AllocateCell(usize Size);

	ZInterpreter* m_Interpreter;

	beard::array<std::unique_ptr<ZHeapBlock>> m_Blocks;
};
