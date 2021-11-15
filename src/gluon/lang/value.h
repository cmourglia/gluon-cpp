#pragma once

#include <beard/core/macros.h>

#include <iostream>
#include <string>

class Object;

struct ValueType
{
    enum Enum
    {
        kUndefined,
        kNull,
        kBoolean,
        kNumber,
        kString,
        kObject,
    };
};

struct ZString
{
    char* String;
    i32   Length;
};

class Value
{
public:
    Value() = default;

    explicit Value(std::nullptr_t)
        : m_ValueType{ValueType::kNull}
        , m_Data{}
    {
    }

    explicit Value(f64 Value)
        : m_ValueType{ValueType::kNumber}
        , m_Data{}
    {
        m_Data.AsNumber = Value;
    }

    explicit Value(i32 Value)
        : m_ValueType{ValueType::kNumber}
        , m_Data{}
    {
        m_Data.AsNumber = Value;
    }

    explicit Value(bool Value)
        : m_ValueType{ValueType::kBoolean}
        , m_Data{}
    {
        m_Data.AsBoolean = Value;
    }

    explicit Value(Object* Object)
        : m_ValueType{ValueType::kObject}
        , m_Data{}
    {
        m_Data.AsObject = Object;
    }

    explicit Value(std::string String)
        : m_ValueType{ValueType::kString}
        , m_Data{}
    {
        // FIXME: This will just leak for now
        m_Data.AsString.Length = static_cast<i32>(String.size());
        m_Data.AsString.String = static_cast<char*>(malloc(m_Data.AsString.Length));
        memcpy(m_Data.AsString.String, String.c_str(), m_Data.AsString.Length);
    }

    // clang-format off
	bool IsUndefined() const { return m_ValueType == ValueType::kUndefined; }
	bool IsNull()      const { return m_ValueType == ValueType::kNull; }
	bool IsBoolean()   const { return m_ValueType == ValueType::kBoolean; }
	bool IsNumber()    const { return m_ValueType == ValueType::kNumber; }
	bool IsString()    const { return m_ValueType == ValueType::kString; }
	bool IsObject()    const { return m_ValueType == ValueType::kObject; }
    // clang-format on

    // clang-format off
	bool AsBoolean() const { return m_Data.AsBoolean; }
	f64 AsNumber() const { return m_Data.AsNumber; }
	Object* AsObject() const { return m_Data.AsObject; }
    // clang-format on
    std::string AsString() const
    {
        return std::string{m_Data.AsString.String, m_Data.AsString.String + m_Data.AsString.Length};
    }

    ValueType::Enum type() const { return m_ValueType; }

    std::string ToString() const;

    static const Value Undefined;
    static const Value Null;

private:
    ValueType::Enum m_ValueType = ValueType::kUndefined;

    union
    {
        bool    AsBoolean;
        f64     AsNumber;
        ZString AsString;
        Object* AsObject;
    } m_Data;
};

bool operator==(const Value& lhs, const Value& rhs);
bool operator!=(const Value& lhs, const Value& rhs);

std::ostream& operator<<(std::ostream& stream, const Value& value);