#pragma once

#include <Gluon/VM/Object.h>

namespace VM
{

struct ScopeNode;

class Function : public Object
{
public:
	Function(std::string name, ScopeNode* body)
	    : m_name{std::move(name)}
	    , m_body{body}
	{
	}

	const char* TypeName() const override { return "Function"; }

	std::string GetName() const { return m_name; }

	ScopeNode* GetBody() const { return m_body; }

	bool IsFunction() const override { return true; }

private:
	std::string m_name;
	ScopeNode*  m_body;
};

}