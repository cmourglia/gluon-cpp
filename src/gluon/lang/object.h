#pragma once

#include <gluon/lang/cell.h>
#include <gluon/lang/value.h>

#include <beard/containers/hash_map.h>

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
	beard::string_hash_map<ZValue> m_Values;
};
