#pragma once

#include <Gluon/VM/Value.h>

#include <Gluon/Core/Containers/HashMap.h>

#include <string>

namespace VM
{

class Object
{
public:
	Value Get(const std::string& name) const;
	void  Add(const std::string& propertyName, Value value);

	virtual bool IsFunction() const { return false; }

private:
	StringHashMap<Value> m_values;
};
}