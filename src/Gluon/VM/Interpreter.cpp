#include <Gluon/VM/Interpreter.h>

#include <Gluon/VM/AST.h>
#include <Gluon/VM/Object.h>

namespace VM
{

Interpreter::Interpreter()
    : m_globalObject{new Object{}}
{
}

Interpreter::~Interpreter()
{
	delete m_globalObject;
	m_globalObject = nullptr;
}

Value Interpreter::Run(ScopeNode* node)
{
	PushScope(node);

	Value lastValue = Value::Undefined;

	for (const auto& c : node->GetChildren())
	{
		lastValue = c->Execute(this);
	}

	PopScope(node);

	return lastValue;
}

void Interpreter::PushScope(ScopeNode* node) { m_stack.Add({.node = node}); }

void Interpreter::PopScope(ScopeNode* node)
{
	// FIXME: Check this
	Assert(m_stack.Last().node == node, "Stack mismatch");

	m_stack.Pop();
}

}