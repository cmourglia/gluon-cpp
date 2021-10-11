#include <Gluon/VM/Value.h>

#include <stdio.h>

namespace VM
{
const Value Value::Undefined = {};

std::string Value::to_string() const
{
	switch (m_value_type)
	{
		case ValueType::Undefined:
			return "Value (type <Undefined>): undefined";
			break;

		case ValueType::Null:
			return "Value (type <Null>): null";
			break;

		case ValueType::Boolean:
			return std::string("Value (type <Boolean>): ") +
			       (m_data.as_boolean ? "true" : "false");
			break;

		case ValueType::Number:
			return std::string("Value (type <Number>): ") +
			       std::to_string(m_data.as_number);
			break;

		case ValueType::String:
			return std::string("Value (type <String>): ") +
			       std::string(m_data.as_string.string,
			                   m_data.as_string.length);
			break;

		case ValueType::Object:
			printf("Value (type <Object>): TODO");
			break;
	}

	ASSERT_UNREACHABLE();
	return "";
}
}