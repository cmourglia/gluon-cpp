#include <Gluon/VM/Heap.h>

#include <Gluon/VM/Interpreter.h>
#include <Gluon/VM/Object.h>

#include <Beard/Timer.h>

#include <loguru.hpp>

Heap::Heap(Interpreter* interpreter)
    : m_interpreter(interpreter)
{
}

void* Heap::allocate_cell(usize size)
{
	for (auto&& block : m_blocks)
	{
		// This block's cell size is not compatible
		if (size > block->cell_size())
		{
			continue;
		}

		// This block has compatible cell size, try to allocate memory
		if (Cell* cell = block->allocate(); cell != nullptr)
		{
			return cell;
		}
	}

	// We did not find a compatible heap block, make a new one
	auto* memory = malloc(HeapBlock::BLOCK_SIZE);
	auto* block  = new (memory) HeapBlock{size};
	void* cell   = block->allocate();
	m_blocks.Add(std::unique_ptr<HeapBlock>(block));

	return cell;
}

inline void clear_objects(const Beard::Array<std::unique_ptr<HeapBlock>>& blocks)
{
	// TODO: We could keep a "used cells" list somewhere to avoid
	// having to iterate over all the memory
	for (auto&& block : blocks)
	{
		for (usize i = 0; i < block->num_cells(); ++i)
		{
			if (auto* cell = block->cell(i); cell->used)
			{
				LOG_F(INFO, "Clearing cell (%s) %p mark flag", cell->to_string(), cell);
				block->cell(i)->marked = false;
			}
		}
	}
}

inline Beard::Array<Cell*> collect_roots(Interpreter* interpreter)
{
	Beard::Array<Cell*> roots;

	if (auto* go = interpreter->global_object(); go != nullptr)
	{
		roots.Add(go);
	}

	return roots;
}

inline void mark_objects(const Beard::Array<Cell*>& roots)
{
	auto MarkObject = [](Cell* visited)
	{
		visited->marked = true;
		LOG_F(INFO, "Marking cell (%s) %p as visited", visited->to_string(), visited);
	};

	for (auto&& root : roots)
	{
		root->visit_graph(MarkObject);
	}
}

inline void sweep_objects(const Beard::Array<std::unique_ptr<HeapBlock>>& blocks)
{
	// TODO: We could keep a "used cells" list somewhere to avoid
	// having to iterate over all the memory
	for (auto&& block : blocks)
	{
		for (usize i = 0; i < block->num_cells(); ++i)
		{
			if (auto* cell = block->cell(i); cell->used && !cell->marked)
			{
				LOG_F(INFO, "Deallocating  cell (%s) %p", cell->to_string(), cell);
				block->deallocate(cell);
			}
		}
	}
}

void Heap::garbage()
{
	LOG_SCOPE_F(INFO, "Garbage started");
	Beard::Timer timer;

	// First, we mark all cells as "not marked".
	clear_objects(m_blocks);

	// Collect all the root cells.
	auto roots = collect_roots(m_interpreter);

	// Then, we visit all the "root nodes" (the interpreter's GO for now)
	// and recursively tag all the objects we visit as used.
	mark_objects(roots);

	// Finally, we iterate over the (used) cells, and deallocate them if they
	// are not marked.
	sweep_objects(m_blocks);

	timer.Tick();
	LOG_F(INFO, "Garbage done in %lfs (%lldμs)", timer.DeltaTime(), timer.DeltaTimeMicroseconds());
}

HeapBlock::HeapBlock(usize cellSize)
{
	// Make sure we are always allocating at least the size of a
	// FreeListItem, to avoid problems.
	m_cell_size = Beard::ClampBot(cellSize, sizeof(FreeListItem));

	for (usize i = 0; i < num_cells(); ++i)
	{
		auto* entry = static_cast<FreeListItem*>(cell(i));
		entry->used = false;
		if (i == num_cells() - 1)
		{
			entry->next = nullptr;
		}
		else
		{
			entry->next = static_cast<FreeListItem*>(cell(i + 1));
		}
	}

	m_freelist = static_cast<FreeListItem*>(cell(0));
}

Cell* HeapBlock::allocate()
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

void HeapBlock::deallocate(Cell* cell)
{
	// Claim back memory (ok, just push the item back into the free list)
	cell->~Cell();
	auto* item = static_cast<FreeListItem*>(cell);
	item->next = m_freelist;
	item->used = false;
	m_freelist = item;
}
