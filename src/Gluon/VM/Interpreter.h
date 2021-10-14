#pragma once

#include <Gluon/VM/Value.h>
#include <Gluon/VM/Heap.h>

#include <Beard/Array.h>

#include <memory>
#include <Beard/HashMap.h>

struct ASTNode;
struct ScopeNode;
class Object;

struct ScopeFrame
{
	ScopeNode* node;

	Beard::StringHashMap<Value> variables;
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

	void  declare_variable(const char* name);
	void  set_variable(const char* name, Value value);
	Value get_variable(const char* name);

private:
	void push_scope(ScopeNode* node);
	void pop_scope(ScopeNode* node);

	Beard::Array<ScopeFrame> m_stack;

	std::unique_ptr<Heap> m_heap          = nullptr;
	Object*               m_global_object = nullptr;
};
