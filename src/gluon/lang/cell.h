#pragma once

#include <beard/core/macros.h>

namespace gluon::lang {

struct Cell {
  NONCOPYABLE(Cell);
  NONMOVEABLE(Cell);

  Cell() = default;
  virtual ~Cell() = default;

  virtual const char* to_string() const = 0;

  using VisitorCallback = void (*)(Cell*);
  virtual void visit_graph(VisitorCallback callback) { callback(this); }

  bool is_marked = false;
  bool is_used = true;
};

}  // namespace gluon::lang