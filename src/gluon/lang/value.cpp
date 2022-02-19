#include <gluon/lang/value.h>

#include <stdio.h>

const Value Value::Undefined = {};
const Value Value::Null = Value{nullptr};

std::string Value::ToString() const {
  switch (m_ValueType) {
    case ValueType::kUndefined:
      return "Value (Type <Undefined>): undefined";

    case ValueType::kNull:
      return "Value (Type <Null>): null";

    case ValueType::kBoolean:
      return std::string("Value (Type <Boolean>): ") +
             (m_Data.AsBoolean ? "true" : "false");

    case ValueType::kNumber:
      return std::string("Value (Type <Number>): ") +
             std::to_string(m_Data.AsNumber);

    case ValueType::kString:
      return std::string("Value (Type <String>): ") +
             std::string(m_Data.AsString.String, m_Data.AsString.Length);

    case ValueType::kObject:
      return std::string("Value (Type <ZObject>): TODO");
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
    case ValueType::kUndefined:
    case ValueType::kNull:
      return true;

    case ValueType::kBoolean:
      return lhs.AsBoolean() == rhs.AsBoolean();

    case ValueType::kNumber:
      return lhs.AsNumber() == rhs.AsNumber();

    case ValueType::kObject:
      return lhs.AsObject() == rhs.AsObject();

    case ValueType::kString:
      return lhs.AsString() == rhs.AsString();
  }

  return false;
}

bool operator!=(const Value& lhs, const Value& rhs) {
  return !(lhs == rhs);
}

std::ostream& operator<<(std::ostream& stream, const Value& value) {
  return stream << value.ToString();
}