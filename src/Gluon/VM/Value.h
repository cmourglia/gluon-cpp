#pragma once

#include <Beard/Macros.h>

#include <string>

class Object;

enum class ValueType
{
	Undefined,
	Null,
	Boolean,
	Number,
	String,
	Object,
};

struct String
{
	char* string;
	i32   length;
};

class Value
{
public:
	Value() = default;

	explicit Value(std::nullptr_t)
	    : m_value_type{ValueType::Null}
	    , m_data{}
	{
	}

	explicit Value(f64 value)
	    : m_value_type{ValueType::Number}
	    , m_data{}
	{
		m_data.as_number = value;
	}

	explicit Value(i32 value)
	    : m_value_type{ValueType::Number}
	    , m_data{}
	{
		m_data.as_number = value;
	}

	explicit Value(bool value)
	    : m_value_type{ValueType::Boolean}
	    , m_data{}
	{
		m_data.as_boolean = value;
	}

	explicit Value(Object* object)
	    : m_value_type(ValueType ::Object)
	    , m_data{}
	{
		m_data.as_object = object;
	}

	// clang-format off
	bool is_undefined() const { return m_value_type == ValueType::Undefined; }
	bool is_null()      const { return m_value_type == ValueType::Null; }
	bool is_boolean()   const { return m_value_type == ValueType::Boolean; }
	bool is_number()    const { return m_value_type == ValueType::Number; }
	bool is_string()    const { return m_value_type == ValueType::String; }
	bool is_object()    const { return m_value_type == ValueType::Object; }
	// clang-format on

	// clang-format off
	bool as_boolean() const { return m_data.as_boolean; }
	f64 as_number() const { return m_data.as_number; }
	Object* as_object() const { return m_data.as_object; }
	// clang-format on
	std::string as_string() const
	{
		return std::string{m_data.as_string.string,
		                   m_data.as_string.string + m_data.as_string.length};
	}

	ValueType type() const { return m_value_type; }

	std::string to_string() const;

	static const Value Undefined;
	static const Value Null;

private:
	ValueType m_value_type = ValueType::Undefined;

	union
	{
		bool    as_boolean;
		f64     as_number;
		String  as_string;
		Object* as_object;
	} m_data;
};

bool operator==(const Value& lhs, const Value& rhs);
bool operator!=(const Value& lhs, const Value& rhs);
