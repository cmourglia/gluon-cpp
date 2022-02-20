#pragma once

#include <beard/containers/hash_map.h>

#include <string>

#include "gluon/lang/cell.h"
#include "gluon/lang/value.h"

namespace gluon::lang {

class Object : public Cell {
 public:
  Value get(std::string_view name) const;
  void add(std::string_view property_name, Value value);

  virtual const char* to_string() const override { return "Object"; }

  virtual bool is_function() const { return false; }

  void visit_graph(VisitorCallback Callback) override;

 private:
  beard::string_hash_map<Value> m_values;
};

}  // namespace gluon::lang