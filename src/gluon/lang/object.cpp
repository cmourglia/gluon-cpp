#include <gluon/lang/object.h>

Value Object::Get(const std::string& name) const {
  return m_values.get_value_or(name, Value::kUndefined);
}

void Object::Add(const std::string& property_name, Value value) {
  m_values[property_name] = value;
}

void Object::VisitGraph(VisitorCallback callback)  // NOLINT
{
  Cell::VisitGraph(callback);

  for (auto&& value : m_values) {
    if (value.second.is_object()) {
      value.second.as_object()->VisitGraph(callback);
    }
  }
}
