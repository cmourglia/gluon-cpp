#pragma once

#include <Gluon/VM/Cell.h>
#include <Gluon/VM/Value.h>

#include <Gluon/Core/Containers/HashMap.h>

#include <string>

namespace VM
{

class Object : public Cell
{
public:
	Value Get(const std::string& name) const;
	void  Add(const std::string& propertyName, Value value);

	const char* GetTypename() const override { return "Object"; }

	virtual bool IsFunction() const { return false; }

	void VisitGraph(VisitorCallback callback) override;

private:
	StringHashMap<Value> m_values;
};
}