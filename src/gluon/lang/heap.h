#pragma once

#include <beard/containers/array.h>
#include <beard/core/macros.h>

#include <memory>

#include "gluon/lang/cell.h"

namespace gluon::lang {

class Interpreter;

struct HeapBlock {
  static constexpr usize kBlockSize = KB(16);

  NONCOPYABLE(HeapBlock);
  NONMOVEABLE(HeapBlock);

  explicit HeapBlock(usize cell_size);
  ~HeapBlock() = default;

  usize cell_count() const {
    return (kBlockSize - sizeof(HeapBlock)) / m_cell_size;
  }

  Cell* cell(usize index) {
    return reinterpret_cast<Cell*>(&m_storage[index * m_cell_size]);
  }

  usize cell_size() const { return m_cell_size; }

  Cell* Allocate();
  void Deallocate(Cell* cell);

 private:
  struct FreeListItem : public Cell {
    FreeListItem* next = nullptr;
  };

  usize m_cell_size = 0;

  // This stuff works because we initialize the `next` pointer when we
  // initialize the ZHeapBlock and when we Deallocate the cell.
  // Then, when we return a cell, we call the constructor (placement new)
  // on the given memory.
  // Hence, the same memory space contains two different (although related)
  // items.
  FreeListItem* m_free_list = nullptr;

  VARIABLE_LENGTH_ARRAY(u8, m_storage);
};

class Heap {
 public:
  NONCOPYABLE(Heap);
  NONMOVEABLE(Heap);

  explicit Heap(Interpreter* interpreter);
  ~Heap() = default;

  template <typename T, typename... Args>
  T* Allocate(Args&&... args) {
    void* memory = AllocateCell(sizeof(T));
    new (memory) T(std::forward<Args>(args)...);
    return reinterpret_cast<T*>(memory);
  }

  void Garbage();

 private:
  void* AllocateCell(usize size);

  Interpreter* interpreter;

  beard::array<std::unique_ptr<HeapBlock>> m_blocks;
};

}  // namespace gluon::lang