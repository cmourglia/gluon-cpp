#include <Gluon/Language/Gluon_Heap.h>

#include <Gluon/Language/Gluon_Interpreter.h>
#include <Gluon/Language/Gluon_Object.h>

#include <Beard/Math.h>
#include <Beard/Timer.h>

#include <loguru.hpp>

ZHeap::ZHeap(ZInterpreter* interpreter)
    : m_Interpreter(interpreter)
{
}

void* ZHeap::AllocateCell(usize Size)
{
	for (auto&& block : m_Blocks)
	{
		// This block's cell Size is not compatible
		if (Size > block->CellSize())
		{
			continue;
		}

		// This block has compatible cell Size, try to Allocate memory
		if (ZCell* cell = block->Allocate(); cell != nullptr)
		{
			return cell;
		}
	}

	// We did not find a compatible heap block, Make a new one
	auto* memory = malloc(ZHeapBlock::BLOCK_SIZE);
	auto* block  = new (memory) ZHeapBlock{Size};
	void* cell   = block->Allocate();
	m_Blocks.Add(std::unique_ptr<ZHeapBlock>(block));

	return cell;
}

inline void clear_objects(const Beard::Array<std::unique_ptr<ZHeapBlock>>& blocks)
{
	// TODO: We could keep a "bUsed cells" list somewhere to avoid
	// having to iterate over all the memory
	for (auto&& block : blocks)
	{
		for (usize i = 0; i < block->CellCount(); ++i)
		{
			if (auto* cell = block->Cell(i); cell->bUsed)
			{
				LOG_F(INFO, "Clearing cell (%s) %p mark flag", cell->ToString(), cell);
				block->Cell(i)->bMarked = false;
			}
		}
	}
}

inline Beard::Array<ZCell*> collect_roots(ZInterpreter* interpreter)
{
	Beard::Array<ZCell*> roots;

	if (auto* go = interpreter->GlobalObject(); go != nullptr)
	{
		roots.Add(go);
	}

	return roots;
}

inline void mark_objects(const Beard::Array<ZCell*>& roots)
{
	auto MarkObject = [](ZCell* visited)
	{
		visited->bMarked = true;
		LOG_F(INFO, "Marking cell (%s) %p as visited", visited->ToString(), visited);
	};

	for (auto&& root : roots)
	{
		root->VisitGraph(MarkObject);
	}
}

inline void sweep_objects(const Beard::Array<std::unique_ptr<ZHeapBlock>>& blocks)
{
	// TODO: We could keep a "bUsed cells" list somewhere to avoid
	// having to iterate over all the memory
	for (auto&& block : blocks)
	{
		for (usize i = 0; i < block->CellCount(); ++i)
		{
			if (auto* cell = block->Cell(i); cell->bUsed && !cell->bMarked)
			{
				LOG_F(INFO, "Deallocating  cell (%s) %p", cell->ToString(), cell);
				block->Deallocate(cell);
			}
		}
	}
}

void ZHeap::Garbage()
{
	LOG_SCOPE_F(INFO, "Garbage started");
	Beard::Timer timer;

	// First, we mark all cells as "not bMarked".
	clear_objects(m_Blocks);

	// Collect all the root cells.
	auto roots = collect_roots(m_Interpreter);

	// Then, we visit all the "root nodes" (the interpreter's GO for now)
	// and recursively tag all the objects we visit as bUsed.
	mark_objects(roots);

	// Finally, we iterate over the (bUsed) cells, and Deallocate them if they
	// are not bMarked.
	sweep_objects(m_Blocks);

	timer.Tick();
	LOG_F(INFO, "Garbage done in %lfs (%lldμs)", timer.DeltaTime(), timer.DeltaTimeMicroseconds());
}

ZHeapBlock::ZHeapBlock(usize cellSize)
{
	// Make sure we are always allocating at least the Size of a
	// FreeListItem, to avoid problems.
	m_CellSize = Beard::ClampBot(cellSize, sizeof(FreeListItem));

	for (usize i = 0; i < CellCount(); ++i)
	{
		auto* entry  = static_cast<FreeListItem*>(Cell(i));
		entry->bUsed = false;
		if (i == CellCount() - 1)
		{
			entry->next = nullptr;
		}
		else
		{
			entry->next = static_cast<FreeListItem*>(Cell(i + 1));
		}
	}

	m_FreeList = static_cast<FreeListItem*>(Cell(0));
}

ZCell* ZHeapBlock::Allocate()
{
	if (m_FreeList == nullptr)
	{
		return nullptr;
	}

	// Copy the pointer and Make the freelist head pointing to the next element
	auto* cell = m_FreeList;
	std::swap(m_FreeList, m_FreeList->next);

	return cell;
}

void ZHeapBlock::Deallocate(ZCell* cell)
{
	// Claim back memory (ok, just push the item back into the free list)
	cell->~ZCell();
	auto* item  = static_cast<FreeListItem*>(cell);
	item->next  = m_FreeList;
	item->bUsed = false;
	m_FreeList  = item;
}
