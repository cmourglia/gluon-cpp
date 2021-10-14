#pragma once

#include <Gluon/VM/Cell.h>

#include <Gluon/Core/Defines.h>
#include <Gluon/Core/Containers/Array.h>

#include <memory>

class Interpreter;
struct Cell;

struct HeapBlock
{
	static constexpr usize BLOCK_SIZE = 16_KB;

	NONCOPYABLE(HeapBlock);
	NONMOVEABLE(HeapBlock);

	explicit HeapBlock(usize cellSize);
	~HeapBlock() = default;

	usize num_cells() const
	{
		return (BLOCK_SIZE - sizeof(HeapBlock)) / m_cell_size;
	}

	Cell* cell(usize index)
	{
		return reinterpret_cast<Cell*>(&m_storage[index * m_cell_size]);
	}

	usize cell_size() const { return m_cell_size; }

	Cell* allocate();
	void  deallocate(Cell* cell);

private:
	struct FreeListItem : public Cell
	{
		FreeListItem* next = nullptr;
	};

	usize m_cell_size = 0;

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
	T* allocate(Args&&... args)
	{
		void* memory = allocate_cell(sizeof(T));
		new (memory) T(std::forward<Args>(args)...);
		return reinterpret_cast<T*>(memory);
	}

	void garbage();

private:
	void* allocate_cell(usize size);

	Interpreter* m_interpreter;

	Array<std::unique_ptr<HeapBlock>> m_blocks;
};
