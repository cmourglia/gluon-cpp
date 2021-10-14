#include <Gluon/VM/Value.h>

#include <stdio.h>

const Value Value::Undefined = {};
const Value Value::Null      = Value{nullptr};

std::string Value::to_string() const
{
	switch (m_value_type)
	{
		case ValueType::Undefined:
			return "Value (type <Undefined>): undefined";

		case ValueType::Null:
			return "Value (type <Null>): null";

		case ValueType::Boolean:
			return std::string("Value (type <Boolean>): ") + (m_data.as_boolean ? "true" : "false");

		case ValueType::Number:
			return std::string("Value (type <Number>): ") + std::to_string(m_data.as_number);

		case ValueType::String:
			return std::string("Value (type <String>): ") +
			       std::string(m_data.as_string.string, m_data.as_string.length);

		case ValueType::Object:
			return std::string("Value (type <Object>): TODO");
	}

	ASSERT_UNREACHABLE();
	return "";
}

bool operator==(const Value& lhs, const Value& rhs)
{
	auto lhs_type = lhs.type();
	auto rhs_type = rhs.type();

	if (lhs_type != rhs_type)
	{
		return false;
	}

	switch (lhs_type)
	{
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
