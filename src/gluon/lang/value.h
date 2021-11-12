#pragma once

#include <beard/core/macros.h>

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

    explicit ZValue(f64 Value)
        : m_ValueType{EValueType::Number}
        , m_Data{}
    {
        m_Data.AsNumber = Value;
    }

    explicit ZValue(i32 Value)
        : m_ValueType{EValueType::Number}
        , m_Data{}
    {
        m_Data.AsNumber = Value;
    }

    explicit ZValue(bool Value)
        : m_ValueType{EValueType::Boolean}
        , m_Data{}
    {
        m_Data.AsBoolean = Value;
    }

    explicit ZValue(ZObject* Object)
        : m_ValueType{EValueType::ZObject}
        , m_Data{}
    {
        m_Data.AsObject = Object;
    }

    explicit ZValue(std::string String)
        : m_ValueType{EValueType::String}
        , m_Data{}
    {
        // FIXME: This will just leak for now
        m_Data.AsString.Length = static_cast<i32>(String.size());
        m_Data.AsString.String = static_cast<char*>(malloc(m_Data.AsString.Length));
        memcpy(m_Data.AsString.String, String.c_str(), m_Data.AsString.Length);
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
