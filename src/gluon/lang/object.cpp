#include <gluon/lang/object.h>

ZValue ZObject::Get(const std::string& Name) const
{
	return m_Values.get_value_or(Name, ZValue::Undefined);
}

void ZObject::Add(const std::string& propertyName, ZValue value)
{
	m_Values[propertyName] = value;
}

void ZObject::VisitGraph(FVisitorCallback callback) // NOLINT
{
	ZCell::VisitGraph(callback);

	for (auto&& value : m_Values)
	{
		if (value.second.IsObject())
		{
			value.second.AsObject()->VisitGraph(callback);
		}
	}
}