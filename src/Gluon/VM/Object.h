#pragma once

#include <Gluon/VM/Cell.h>
#include <Gluon/VM/Value.h>

#include <Gluon/Core/Containers/HashMap.h>

#include <string>

class Object : public Cell
{
public:
	Value get(const std::string& name) const;
	void  add(const std::string& propertyName, Value value);

	virtual const char* to_string() const override { return "Object"; }

	virtual bool is_function() const { return false; }

	void visit_graph(VisitorCallback callback) override;

private:
	StringHashMap<Value> m_values;
};
