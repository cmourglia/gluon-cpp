#pragma once

#include <beard/core/macros.h>

#include <cstring>  // memcpy
#include <iostream>
#include <string>

namespace gluon::lang {

class Object;

enum class ValueType {
  Undefined,
  Null,
  Boolean,
  Number,
  String,
  Object,
};

struct String {
  char* str;
  usize len;
};

class Value {
 public:
  Value() = default;

  explicit Value(std::nullptr_t) : m_type{ValueType::Null}, m_data{} {}

  explicit Value(f64 value) : m_type{ValueType::Number}, m_data{} {
    m_data.as_number = value;
  }

  explicit Value(i32 value) : m_type{ValueType::Number}, m_data{} {
    m_data.as_number = value;
  }

  explicit Value(bool value) : m_type{ValueType::Boolean}, m_data{} {
    m_data.as_boolean = value;
  }

  explicit Value(Object* object) : m_type{ValueType::Object}, m_data{} {
    m_data.as_object = object;
  }

  explicit Value(std::string_view string)
      : m_type{ValueType::String}, m_data{} {
    // FIXME: This will just leak for now
    m_data.as_string.len = static_cast<i32>(string.size());
    m_data.as_string.str = static_cast<char*>(malloc(m_data.as_string.len));
    memcpy(m_data.as_string.str, string.data(), m_data.as_string.len);
  }

  // clang-format off
	bool is_undefined() const { return m_type == ValueType::Undefined; }
	bool is_null()      const { return m_type == ValueType::Null; }
	bool is_boolean()   const { return m_type == ValueType::Boolean; }
	bool is_number()    const { return m_type == ValueType::Number; }
	bool is_string()    const { return m_type == ValueType::String; }
	bool is_object()    const { return m_type == ValueType::Object; }
  // clang-format on

  // clang-format off
	bool as_boolean() const { return m_data.as_boolean; }
	f64 as_number() const { return m_data.as_number; }
	Object* as_object() const { return m_data.as_object; }
  // clang-format on
  std::string_view as_string() const {
    return std::string_view{m_data.as_string.str, m_data.as_string.len};
  }

  ValueType type() const { return m_type; }

  std::string to_string() const;

  static const Value kUndefined;
  static const Value kNull;

 private:
  ValueType m_type = ValueType::Undefined;

  union {
    bool as_boolean;
    f64 as_number;
    String as_string;
    Object* as_object;
  } m_data;
};

bool operator==(const Value& lhs, const Value& rhs);
bool operator!=(const Value& lhs, const Value& rhs);

// Unary
Value operator-(const Value& lhs);
Value operator!(const Value& lhs);

// Binary
Value operator+(const Value& lhs, const Value& rhs);
Value operator-(const Value& lhs, const Value& rhs);
Value operator*(const Value& lhs, const Value& rhs);
Value operator/(const Value& lhs, const Value& rhs);
Value operator%(const Value& lhs, const Value& rhs);

}  // namespace gluon::lang

std::ostream& operator<<(std::ostream& stream, const gluon::lang::Value& value);