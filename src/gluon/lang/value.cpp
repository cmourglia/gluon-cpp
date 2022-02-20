#include "gluon/lang/value.h"

#include <stdio.h>

namespace gluon::lang {

const Value Value::kUndefined = {};
const Value Value::kNull = Value{nullptr};

std::string Value::to_string() const {
  switch (m_type) {
    case ValueType::Undefined:
      return "Value (Type <Undefined>): undefined";

    case ValueType::Null:
      return "Value (Type <Null>): null";

    case ValueType::Boolean:
      return std::string("Value (Type <Boolean>): ") +
             (m_data.as_boolean ? "true" : "false");

    case ValueType::Number:
      return std::string("Value (Type <Number>): ") +
             std::to_string(m_data.as_number);

    case ValueType::String:
      return std::string("Value (Type <String>): ") +
             std::string(m_data.as_string.str, m_data.as_string.len);

    case ValueType::Object:
      return std::string("Value (Type <Object>): TODO");
  }

  ASSERT_UNREACHABLE();
  return "";
}

bool operator==(const Value& lhs, const Value& rhs) {
  auto lhs_type = lhs.type();
  auto rhs_type = rhs.type();

  if (lhs_type != rhs_type) {
    return false;
  }

  switch (lhs_type) {
    case ValueType::Undefined:
    case ValueType::Null:
      return true;

    case ValueType::Boolean:
      return lhs.as_boolean() == rhs.as_boolean();

    case ValueType::Number:
      return lhs.as_number() == rhs.as_number();

    case ValueType::Object:
      return lhs.as_object() == rhs.as_object();

    case ValueType::String:
      return lhs.as_string() == rhs.as_string();
  }

  return false;
}

bool operator!=(const Value& lhs, const Value& rhs) { return !(lhs == rhs); }

}  // namespace gluon::lang

std::ostream& operator<<(std::ostream& stream,
                         const gluon::lang::Value& value) {
  return stream << value.to_string();
}
