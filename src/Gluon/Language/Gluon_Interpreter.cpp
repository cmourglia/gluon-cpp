#include <Gluon/Language/Gluon_Interpreter.h>

#include <Gluon/Language/Gluon_AST.h>
#include <Gluon/Language/Gluon_Object.h>

ZInterpreter::ZInterpreter()
{
	m_Heap         = std::make_unique<ZHeap>(this);
	m_GlobalObject = m_Heap->Allocate<ZObject>();
}

ZInterpreter::~ZInterpreter()
{
	m_GlobalObject = nullptr;
	m_Heap->Garbage();
}

ZValue ZInterpreter::Run(ZScopeNode* Node)
{
	PushScope(Node);

	ZValue last_value = ZValue::Undefined;

	for (const auto& c : Node->Children())
	{
		last_value = c->Execute(this);
	}

	PopScope(Node);

	return last_value;
}

void ZInterpreter::PushScope(ZScopeNode* Node)
{
	m_Stack.Add({.Node = Node});
}

void ZInterpreter::PopScope(ZScopeNode* Node)
{
	// FIXME: Check this
	ASSERT(m_Stack.Last().Node == Node, "Stack mismatch");

	m_Stack.PopAndDiscard();
}

void ZInterpreter::DeclareVariable(const char* Name)
{
	ZScopeFrame& frame    = m_Stack.Last();
	frame.Variables[Name] = ZValue::Null;
}

void ZInterpreter::SetVariable(const char* Name, ZValue value)
{
	for (i32 i = m_Stack.ElementCount() - 1; i >= 0; --i)
	{
		ZScopeFrame& frame = m_Stack[i];
		if (auto it = frame.Variables.Find(Name); it != frame.Variables.end())
		{
			it->second = value;
			return;
		}
	}

	m_GlobalObject->Add(Name, value);
}

ZValue ZInterpreter::GetVariable(const char* Name)
{
	for (i32 i = m_Stack.ElementCount() - 1; i >= 0; --i)
	{
		ZScopeFrame& frame = m_Stack[i];
		if (auto it = frame.Variables.Find(Name); it != frame.Variables.end())
		{
			return it->second;
		}
	}

	// Look in the global object
	return m_GlobalObject->Get(Name);
}
