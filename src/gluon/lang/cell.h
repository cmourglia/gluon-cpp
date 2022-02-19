#pragma once

#include <beard/core/macros.h>

struct Cell {
  NONCOPYABLE(Cell);
  NONMOVEABLE(Cell);

  Cell() = default;
  virtual ~Cell() = default;

  virtual const char* ToString() const = 0;

  using VisitorCallback = void (*)(Cell*);
  virtual void VisitGraph(VisitorCallback callback) { callback(this); }

  bool is_marked = false;
  bool is_used = true;
};
