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

}