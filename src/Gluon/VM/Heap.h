#pragma once

#include <Gluon/VM/Cell.h>

#include <Gluon/Core/Defines.h>
#include <Gluon/Core/Containers/DynArray.h>

#include <memory>

namespace VM
{

class Interpreter;
struct Cell;

struct HeapBlock
{
	static constexpr usize BLOCK_SIZE = 16_KB;

	NONCOPYABLE(HeapBlock);
	NONMOVEABLE(HeapBlock);

	explicit HeapBlock(usize cellSize);
	~HeapBlock() = default;

	usize NumCells() const
	{
		return (BLOCK_SIZE - sizeof(HeapBlock)) / m_cellSize;
	}

	Cell* GetCell(usize index)
	{
		return reinterpret_cast<Cell*>(&m_storage[index * m_cellSize]);
	}

	usize GetCellSize() const { return m_cellSize; }

	Cell* Allocate();
	void  Deallocate(Cell* cell);

private:
	struct FreeListItem : public Cell
	{
		FreeListItem* next = nullptr;
	};

	usize m_cellSize = 0;

	// This stuff works because we initialize the `next` pointer when we
	// initialize the HeapBlock and when we deallocate the cell.
	// Then, when we return a cell, we call the constructor (placement new)
	// on the given memory.
	// Hence, the same memory space contains two different (although related)
	// items.
	FreeListItem* m_freelist = nullptr;

	VARIABLE_LENGTH_ARRAY(u8, m_storage);
};

class Heap
{
public:
	NONCOPYABLE(Heap);
	NONMOVEABLE(Heap);

	explicit Heap(Interpreter* interpreter);
	~Heap() = default;

	template <typename T, typename... Args>
	T* Allocate(Args&&... args)
	{
		void* memory = AllocateCell(sizeof(T));
		new (memory) T(std::forward<Args>(args)...);
		return reinterpret_cast<T*>(memory);
	}

	void Garbage();

private:
	void* AllocateCell(usize size);

	Interpreter* m_interpreter;

	DynArray<std::unique_ptr<HeapBlock>> m_blocks;
};

}