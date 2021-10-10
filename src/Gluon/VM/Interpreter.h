#pragma once

#include <Gluon/VM/Value.h>

#include <Gluon/Core/Containers/DynArray.h>

namespace VM
{

struct ASTNode;
struct ScopeNode;
class Object;

struct ScopeFrame
{
	ScopeNode* node;
};

class Interpreter
{
public:
	Interpreter();
	~Interpreter();

	NONCOPYABLE(Interpreter);
	NONMOVEABLE(Interpreter);

	Value Run(ScopeNode* node);

	Object* GlobalObject() const { return m_globalObject; }

private:
	void PushScope(ScopeNode* node);
	void PopScope(ScopeNode* node);

	DynArray<ScopeFrame> m_stack;

	Object* m_globalObject = nullptr;
};

}
