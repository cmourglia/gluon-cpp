#pragma once

#include <gluon/lang/object.h>

struct ZScopeNode;

class ZFunction : public ZObject
{
public:
	ZFunction(std::string Name, ZScopeNode* body)
	    : m_Name{std::move(Name)}
	    , m_Body{body}
	{
	}

	const char* ToString() const override
	{
		return "Function";
	}

	std::string Name() const
	{
		return m_Name;
	}

	ZScopeNode* Body() const
	{
		return m_Body;
	}

	bool IsFunction() const override
	{
		return true;
	}

private:
	std::string m_Name;
	ZScopeNode* m_Body;
};
