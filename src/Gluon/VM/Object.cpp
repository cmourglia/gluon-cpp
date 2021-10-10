#include <Gluon/VM/Object.h>

namespace VM
{

Value Object::Get(const std::string& name) const
{
	return m_values.GetValueOr(name, Value::Undefined);
}

void Object::Add(const std::string& propertyName, Value value)
{
	m_values[propertyName] = value;
}

void Object::VisitGraph(VisitorCallback callback) // NOLINT
{
	Cell::VisitGraph(callback);

	for (auto&& value : m_values)
	{
		if (value.second.IsObject())
		{
			value.second.AsObject()->VisitGraph(callback);
		}
	}
}
}