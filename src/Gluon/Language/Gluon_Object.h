#pragma once

#include <Gluon/Language/Gluon_Cell.h>
#include <Gluon/Language/Gluon_Value.h>

#include <Beard/HashMap.h>

#include <string>

class ZObject : public ZCell
{
public:
	ZValue Get(const std::string& Name) const;
	void   Add(const std::string& PropertyName, ZValue Value);

	virtual const char* ToString() const override
	{
		return "Object";
	}

	virtual bool IsFunction() const
	{
		return false;
	}

	void VisitGraph(FVisitorCallback Callback) override;

private:
	Beard::StringHashMap<ZValue> m_Values;
};
