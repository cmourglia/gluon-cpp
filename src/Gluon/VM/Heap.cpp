#include <Gluon/VM/Heap.h>

#include <Gluon/VM/Interpreter.h>
#include <Gluon/VM/Object.h>

#include <Gluon/Core/Timer.h>

#include <loguru.hpp>

namespace VM
{

Heap::Heap(Interpreter* interpreter)
    : m_interpreter(interpreter)
{
}

void* Heap::AllocateCell(usize size)
{
	for (auto&& block : m_blocks)
	{
		// This block's cell size is not compatible
		if (size > block->GetCellSize())
		{
			continue;
		}

		// This block has compatible cell size, try to allocate memory
		if (Cell* cell = block->Allocate(); cell != nullptr)
		{
			return cell;
		}
	}

	// We did not find a compatible heap block, make a new one
	auto* memory = malloc(HeapBlock::BLOCK_SIZE);
	auto* block  = new (memory) HeapBlock{size};
	void* cell   = block->Allocate();
	m_blocks.Add(std::unique_ptr<HeapBlock>(block));

	return cell;
}

inline void ClearObjects(const DynArray<std::unique_ptr<HeapBlock>>& blocks)
{
	// TODO: We could keep a "used cells" list somewhere to avoid
	// having to iterate over all the memory
	for (auto&& block : blocks)
	{
		for (usize i = 0; i < block->NumCells(); ++i)
		{
			if (auto* cell = block->GetCell(i); cell->used)
			{
				LOG_F(INFO,
				      "Clearing cell (%s) %p mark flag",
				      cell->GetTypename(),
				      cell);
				block->GetCell(i)->marked = false;
			}
		}
	}
}

inline DynArray<Cell*> CollectRoots(Interpreter* interpreter)
{
	DynArray<Cell*> roots;

	if (auto* go = interpreter->GetGlobalObject(); go != nullptr)
	{
		roots.Add(go);
	}

	return roots;
}

inline void MarkObjects(const DynArray<Cell*>& roots)
{
	auto MarkObject = [](Cell* visited)
	{
		visited->marked = true;
		LOG_F(INFO,
		      "Marking cell (%s) %p as visited",
		      visited->GetTypename(),
		      visited);
	};

	for (auto&& root : roots)
	{
		root->VisitGraph(MarkObject);
	}
}

inline void SweepObjects(const DynArray<std::unique_ptr<HeapBlock>>& blocks)
{
	// TODO: We could keep a "used cells" list somewhere to avoid
	// having to iterate over all the memory
	for (auto&& block : blocks)
	{
		for (usize i = 0; i < block->NumCells(); ++i)
		{
			if (auto* cell = block->GetCell(i); cell->used && !cell->marked)
			{
				LOG_F(INFO,
				      "Deallocating  cell (%s) %p",
				      cell->GetTypename(),
				      cell);
				block->Deallocate(cell);
			}
		}
	}
}

void Heap::Garbage()
{
	LOG_SCOPE_F(INFO, "Garbage started");
	Timer timer;

	// First, we mark all cells as "not marked".
	ClearObjects(m_blocks);

	// Collect all the root cells.
	auto roots = CollectRoots(m_interpreter);

	// Then, we visit all the "root nodes" (the interpreter's GO for now)
	// and recursively tag all the objects we visit as used.
	MarkObjects(roots);

	// Finally, we iterate over the (used) cells, and deallocate them if they
	// are not marked.
	SweepObjects(m_blocks);

	timer.Tick();
	LOG_F(INFO,
	      "Garbage done in %lfs (%lldμs)",
	      timer.GetDeltaTime(),
	      timer.GetDeltaTimeInMicroseconds());
}

HeapBlock::HeapBlock(usize cellSize)
{
	// Make sure we are always allocating at least the size of a
	// FreeListItem, to avoid problems.
	m_cellSize = Max(cellSize, sizeof(FreeListItem));

	for (usize i = 0; i < NumCells(); ++i)
	{
		auto* entry = static_cast<FreeListItem*>(GetCell(i));
		entry->used = false;
		if (i == NumCells() - 1)
		{
			entry->next = nullptr;
		}
		else
		{
			entry->next = static_cast<FreeListItem*>(GetCell(i + 1));
		}
	}

	m_freelist = static_cast<FreeListItem*>(GetCell(0));
}

Cell* HeapBlock::Allocate()
{
	if (m_freelist == nullptr)
	{
		return nullptr;
	}

	// Copy the pointer and make the freelist head pointing to the next element
	auto* cell = m_freelist;
	std::swap(m_freelist, m_freelist->next);

	return cell;
}

void HeapBlock::Deallocate(Cell* cell)
{
	// Claim back memory (ok, just push the item back into the free list)
	auto* item = static_cast<FreeListItem*>(cell);
	item->next = m_freelist;
	item->used = false;
	m_freelist = item;
}

}