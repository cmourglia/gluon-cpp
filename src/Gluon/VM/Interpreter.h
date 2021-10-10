#pragma once

#include <Gluon/VM/Value.h>
#include <Gluon/VM/Heap.h>

#include <Gluon/Core/Containers/DynArray.h>

#include <memory>

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

	Object* GetGlobalObject() const { return m_globalObject; }

	Heap* GetHeap() const { return m_heap.get(); }

private:
	void PushScope(ScopeNode* node);
	void PopScope(ScopeNode* node);

	DynArray<ScopeFrame> m_stack;

	std::unique_ptr<Heap> m_heap         = nullptr;
	Object*               m_globalObject = nullptr;
};

}
