#pragma once

#include <Gluon/Core/Defines.h>

namespace VM
{

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

	explicit Value(f64 value)
	    : m_valueType{ValueType::Number}
	    , m_data{}
	{
		m_data.asNumber = value;
	}

	explicit Value(i32 value)
	    : m_valueType{ValueType::Number}
	    , m_data{}
	{
		m_data.asNumber = value;
	}

	explicit Value(bool value)
	    : m_valueType{ValueType::Boolean}
	    , m_data{}
	{
		m_data.asBoolean = value;
	}

	explicit Value(Object* object)
	    : m_valueType(ValueType ::Object)
	    , m_data{}
	{
		m_data.asObject = object;
	}

	// clang-format off
	bool IsUndefined() const { return m_valueType == ValueType::Undefined; }
	bool IsNull()      const { return m_valueType == ValueType::Null; }
	bool IsBoolean()   const { return m_valueType == ValueType::Boolean; }
	bool IsNumber()    const { return m_valueType == ValueType::Number; }
	bool IsString()    const { return m_valueType == ValueType::String; }
	bool IsObject()    const { return m_valueType == ValueType::Object; }
	// clang-format on

	// clang-format off
	bool AsBoolean() const { return m_data.asBoolean; }
	f64 AsNumber() const { return m_data.asNumber; }
	String AsString() const { return m_data.asString; }
	Object* AsObject() const { return m_data.asObject; }
	// clang-format on

	ValueType GetType() const { return m_valueType; }

	void Dump() const;

	static const Value Undefined;

private:
	ValueType m_valueType = ValueType::Undefined;

	union
	{
		bool    asBoolean;
		f64     asNumber;
		String  asString;
		Object* asObject;
	} m_data;
};

}