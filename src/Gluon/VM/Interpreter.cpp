#include <Gluon/VM/Interpreter.h>

#include <Gluon/VM/AST.h>
#include <Gluon/VM/Object.h>

namespace VM
{

Interpreter::Interpreter()
{
	m_heap          = std::make_unique<Heap>(this);
	m_global_object = m_heap->allocate<Object>();
}

Interpreter::~Interpreter()
{
	m_global_object = nullptr;
	m_heap->garbage();
}

Value Interpreter::run(ScopeNode* node)
{
	push_scope(node);

	Value last_value = Value::Undefined;

	for (const auto& c : node->children())
	{
		last_value = c->execute(this);
	}

	pop_scope(node);

	return last_value;
}

void Interpreter::push_scope(ScopeNode* node) { m_stack.add({.node = node}); }

void Interpreter::pop_scope(ScopeNode* node)
{
	// FIXME: Check this
	ASSERT(m_stack.last().node == node, "Stack mismatch");

	m_stack.pop();
}
}