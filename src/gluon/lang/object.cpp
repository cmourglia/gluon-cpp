#include "gluon/lang/object.h"

namespace gluon::lang {

Value Object::get(std::string_view name) const {
  return m_values.get_value_or(std::string{name.data(), name.size()},
                               Value::kUndefined);
}

void Object::add(std::string_view property_name, Value value) {
  m_values[std::string{property_name.data(), property_name.size()}] = value;
}

void Object::visit_graph(VisitorCallback callback)  // NOLINT
{
  Cell::visit_graph(callback);

  for (auto&& value : m_values) {
    if (value.second.is_object()) {
      value.second.as_object()->visit_graph(callback);
    }
  }
}

}  // namespace gluon::lang