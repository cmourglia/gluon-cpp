#pragma once

#include <Gluon/VM/Value.h>
#include <Gluon/VM/Heap.h>

#include <Gluon/Core/Containers/Array.h>

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

	Value run(ScopeNode* node);

	Object* global_object() const { return m_global_object; }

	Heap* heap() const { return m_heap.get(); }

private:
	void push_scope(ScopeNode* node);
	void pop_scope(ScopeNode* node);

	Array<ScopeFrame> m_stack;

	std::unique_ptr<Heap> m_heap          = nullptr;
	Object*               m_global_object = nullptr;
};

}
