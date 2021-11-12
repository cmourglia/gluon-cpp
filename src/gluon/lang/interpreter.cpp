#include <gluon/lang/interpreter.h>

#include <gluon/lang/ast.h>
#include <gluon/lang/object.h>

Interpreter::Interpreter()
{
    m_heap          = std::make_unique<Heap>(this);
    m_global_object = m_heap->Allocate<Object>();
}

Interpreter::~Interpreter()
{
    m_global_object = nullptr;
    m_heap->Garbage();
}

Value Interpreter::Run(ScopeNode* node)
{
    PushScope(node);

    Value last_value = Value::Undefined;

    // for (const auto& c : Node->Children())
    //{
    //	last_value = c->Execute(this);
    //}

    PopScope(node);

    return last_value;
}

void Interpreter::PushScope(ScopeNode* node)
{
    m_stack.add({.node = node});
}

void Interpreter::PopScope(ScopeNode* node)
{
    // FIXME: Check this
    ASSERT(m_stack.last().node == node, "Stack mismatch");

    m_stack.pop_and_discard();
}

void Interpreter::DeclareVariable(const char* name)
{
    ScopeFrame& frame     = m_stack.last();
    frame.variables[name] = Value::Null;
}

void Interpreter::SetVariable(const char* name, Value value)
{
    for (i32 i = m_stack.element_count() - 1; i >= 0; --i)
    {
        ScopeFrame& frame = m_stack[i];
        if (auto it = frame.variables.find(name); it != frame.variables.end())
        {
            it->second = value;
            return;
        }
    }

    m_global_object->Add(name, value);
}

Value Interpreter::GetVariable(const char* name)
{
    for (i32 i = m_stack.element_count() - 1; i >= 0; --i)
    {
        ScopeFrame& frame = m_stack[i];
        if (auto it = frame.variables.find(name); it != frame.variables.end())
        {
            return it->second;
        }
    }

    // Look in the global object
    return m_global_object->Get(name);
}
