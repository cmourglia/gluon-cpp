#pragma once

#include <Beard/Macros.h>

#include <string>

class ZObject;

enum class EValueType
{
	Undefined,
	Null,
	Boolean,
	Number,
	String,
	ZObject,
};

struct ZString
{
	char* String;
	i32   Length;
};

class ZValue
{
public:
	ZValue() = default;

	explicit ZValue(std::nullptr_t)
	    : m_ValueType{EValueType::Null}
	    , m_Data{}
	{
	}

	explicit ZValue(f64 value)
	    : m_ValueType{EValueType::Number}
	    , m_Data{}
	{
		m_Data.AsNumber = value;
	}

	explicit ZValue(i32 value)
	    : m_ValueType{EValueType::Number}
	    , m_Data{}
	{
		m_Data.AsNumber = value;
	}

	explicit ZValue(bool value)
	    : m_ValueType{EValueType::Boolean}
	    , m_Data{}
	{
		m_Data.AsBoolean = value;
	}

	explicit ZValue(ZObject* object)
	    : m_ValueType(EValueType ::ZObject)
	    , m_Data{}
	{
		m_Data.AsObject = object;
	}

	// clang-format off
	bool IsUndefined() const { return m_ValueType == EValueType::Undefined; }
	bool IsNull()      const { return m_ValueType == EValueType::Null; }
	bool IsBoolean()   const { return m_ValueType == EValueType::Boolean; }
	bool IsNumber()    const { return m_ValueType == EValueType::Number; }
	bool IsString()    const { return m_ValueType == EValueType::String; }
	bool IsObject()    const { return m_ValueType == EValueType::ZObject; }
	// clang-format on

	// clang-format off
	bool AsBoolean() const { return m_Data.AsBoolean; }
	f64 AsNumber() const { return m_Data.AsNumber; }
	ZObject* AsObject() const { return m_Data.AsObject; }
	// clang-format on
	std::string AsString() const
	{
		return std::string{m_Data.AsString.String, m_Data.AsString.String + m_Data.AsString.Length};
	}

	EValueType Type() const
	{
		return m_ValueType;
	}

	std::string ToString() const;

	static const ZValue Undefined;
	static const ZValue Null;

private:
	EValueType m_ValueType = EValueType::Undefined;

	union
	{
		bool     AsBoolean;
		f64      AsNumber;
		ZString  AsString;
		ZObject* AsObject;
	} m_Data;
};

bool operator==(const ZValue& lhs, const ZValue& rhs);
bool operator!=(const ZValue& lhs, const ZValue& rhs);
