#include "gluon/lang/heap.h"

#include <beard/math/math.h>
#include <beard/misc/timer.h>

#include <loguru.hpp>

#include "gluon/lang/interpreter.h"
#include "gluon/lang/object.h"

namespace gluon::lang {

Heap::Heap(Interpreter* interpreter) : interpreter(interpreter) {}

void* Heap::AllocateCell(usize size) {
  for (auto&& block : m_blocks) {
    // This block's cell Size is not compatible
    if (size > block->cell_size()) {
      continue;
    }

    // This block has compatible cell Size, try to Allocate memory
    if (Cell* cell = block->Allocate(); cell != nullptr) {
      return cell;
    }
  }

  // We did not find a compatible heap block, make a new one
  auto* memory = malloc(HeapBlock::kBlockSize);
  auto* block = new (memory) HeapBlock{size};
  void* cell = block->Allocate();
  m_blocks.add(std::unique_ptr<HeapBlock>(block));

  return cell;
}

inline void clear_objects(
    const beard::array<std::unique_ptr<HeapBlock>>& blocks) {
  // TODO: We could keep a "bUsed cells" list somewhere to avoid
  // having to iterate over all the memory
  for (auto&& block : blocks) {
    for (usize i = 0; i < block->cell_count(); ++i) {
      if (auto* cell = block->cell(i); cell->is_used) {
        LOG_F(INFO, "Clearing cell (%s) %p mark flag", cell->to_string(), cell);
        block->cell(i)->is_marked = false;
      }
    }
  }
}

inline beard::array<Cell*> collect_roots(Interpreter* interpreter) {
  beard::array<Cell*> roots;

  if (auto* go = interpreter->global_object(); go != nullptr) {
    roots.add(go);
  }

  return roots;
}

inline void mark_objects(const beard::array<Cell*>& roots) {
  auto MarkObject = [](Cell* visited) {
    visited->is_marked = true;
    LOG_F(INFO, "Marking cell (%s) %p as visited", visited->to_string(),
          visited);
  };

  for (auto&& root : roots) {
    root->visit_graph(MarkObject);
  }
}

inline void sweep_objects(
    const beard::array<std::unique_ptr<HeapBlock>>& blocks) {
  // TODO: We could keep a "bUsed cells" list somewhere to avoid
  // having to iterate over all the memory
  for (auto&& block : blocks) {
    for (usize i = 0; i < block->cell_count(); ++i) {
      if (auto* cell = block->cell(i); cell->is_used && !cell->is_marked) {
        LOG_F(INFO, "Deallocating  cell (%s) %p", cell->to_string(), cell);
        block->Deallocate(cell);
      }
    }
  }
}

void Heap::Garbage() {
  LOG_SCOPE_F(INFO, "Garbage started");
  beard::timer timer;

  // First, we mark all cells as "not bMarked".
  clear_objects(m_blocks);

  // Collect all the root cells.
  auto roots = collect_roots(interpreter);

  // Then, we visit all the "root nodes" (the interpreter's GO for now)
  // and recursively tag all the objects we visit as bUsed.
  mark_objects(roots);

  // Finally, we iterate over the (bUsed) cells, and Deallocate them if they
  // are not bMarked.
  sweep_objects(m_blocks);

  timer.tick();
  LOG_F(INFO, "Garbage done in %lfs (%ziμs)", timer.delta_time(),
        timer.delta_time_us());
}

HeapBlock::HeapBlock(usize cell_size) {
  // Make sure we are always allocating at least the Size of a
  // FreeListItem, to avoid problems.
  m_cell_size = beard::clamp_bot(cell_size, sizeof(FreeListItem));

  for (usize i = 0; i < cell_count(); ++i) {
    auto* entry = static_cast<FreeListItem*>(cell(i));
    entry->is_used = false;
    if (i == cell_count() - 1) {
      entry->next = nullptr;
    } else {
      entry->next = static_cast<FreeListItem*>(cell(i + 1));
    }
  }

  m_free_list = static_cast<FreeListItem*>(cell(0));
}

Cell* HeapBlock::Allocate() {
  if (m_free_list == nullptr) {
    return nullptr;
  }

  // Copy the pointer and make the freelist head pointing to the next element
  auto* cell = m_free_list;
  std::swap(m_free_list, m_free_list->next);

  return cell;
}

void HeapBlock::Deallocate(Cell* cell) {
  // Claim back memory (ok, just push the item back into the free list)
  cell->~Cell();
  auto* item = static_cast<FreeListItem*>(cell);
  item->next = m_free_list;
  item->is_used = false;
  m_free_list = item;
}

}  // namespace gluon::lang