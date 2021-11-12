#include <gluon/lang/value.h>

#include <stdio.h>

const ZValue ZValue::Undefined = {};
const ZValue ZValue::Null      = ZValue{nullptr};

std::string ZValue::ToString() const
{
	switch (m_ValueType)
	{
		case EValueType::Undefined:
			return "ZValue (Type <Undefined>): undefined";

		case EValueType::Null:
			return "ZValue (Type <Null>): null";

		case EValueType::Boolean:
			return std::string("ZValue (Type <Boolean>): ") + (m_Data.AsBoolean ? "true" : "false");

		case EValueType::Number:
			return std::string("ZValue (Type <Number>): ") + std::to_string(m_Data.AsNumber);

		case EValueType::String:
			return std::string("ZValue (Type <String>): ") +
			       std::string(m_Data.AsString.String, m_Data.AsString.Length);

		case EValueType::ZObject:
			return std::string("ZValue (Type <ZObject>): TODO");
	}

	ASSERT_UNREACHABLE();
	return "";
}

bool operator==(const ZValue& lhs, const ZValue& rhs)
{
	auto lhs_type = lhs.Type();
	auto rhs_type = rhs.Type();

	if (lhs_type != rhs_type)
	{
		return false;
	}

	switch (lhs_type)
	{
		case EValueType::Undefined:
		case EValueType::Null:
			return true;

		case EValueType::Boolean:
			return lhs.AsBoolean() == rhs.AsBoolean();

		case EValueType::Number:
			return lhs.AsNumber() == rhs.AsNumber();

		case EValueType::ZObject:
			return lhs.AsObject() == rhs.AsObject();

		case EValueType::String:
			return lhs.AsString() == rhs.AsString();
	}

	return false;
}

bool operator!=(const ZValue& lhs, const ZValue& rhs)
{
	return !(lhs == rhs);
}
