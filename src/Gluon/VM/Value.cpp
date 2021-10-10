#include <Gluon/VM/Value.h>

#include <stdio.h>

namespace VM
{
const Value Value::Undefined = {};

void Value::Dump() const
{
	switch (m_valueType)
	{
		case ValueType::Undefined:
			printf("Value (type <Undefined>): undefined");
			break;

		case ValueType::Null:
			printf("Value (type <Null>): null");
			break;

		case ValueType::Boolean:
			printf("Value (type <Boolean>): %s",
			       m_data.asBoolean ? "true" : "false");
			break;

		case ValueType::Number:
			printf("Value (type <Number>): %lf", m_data.asNumber);
			break;

		case ValueType::String:
			printf("Value (type <String>): %.*s",
			       m_data.asString.length,
			       m_data.asString.string);
			break;

		case ValueType::Object:
			printf("Value (type <Object>): TODO");
			break;
	}
}
}