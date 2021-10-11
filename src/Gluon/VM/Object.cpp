#include <Gluon/VM/Object.h>

namespace VM
{

Value Object::get(const std::string& name) const
{
	return m_values.get_value_or(name, Value::Undefined);
}

void Object::add(const std::string& propertyName, Value value)
{
	m_values[propertyName] = value;
}

void Object::visit_graph(VisitorCallback callback) // NOLINT
{
	Cell::visit_graph(callback);

	for (auto&& value : m_values)
	{
		if (value.second.is_object())
		{
			value.second.as_object()->visit_graph(callback);
		}
	}
}
}