#pragma once

#include <gluon/lang/heap.h>
#include <gluon/lang/value.h>

#include <beard/containers/array.h>

#include <beard/containers/hash_map.h>
#include <memory>

struct ZASTNode;
struct ZScopeNode;
class ZObject;

struct ZScopeFrame
{
	ZScopeNode* Node;

	beard::string_hash_map<ZValue> Variables;
};

class ZInterpreter
{
public:
	ZInterpreter();
	~ZInterpreter();

	NONCOPYABLE(ZInterpreter);
	NONMOVEABLE(ZInterpreter);

	ZValue Run(ZScopeNode* Node);

	ZObject* GlobalObject() const
	{
		return m_GlobalObject;
	}

	ZHeap* Heap() const
	{
		return m_Heap.get();
	}

	void   DeclareVariable(const char* Name);
	void   SetVariable(const char* Name, ZValue value);
	ZValue GetVariable(const char* Name);

private:
	void PushScope(ZScopeNode* Node);
	void PopScope(ZScopeNode* Node);

	beard::array<ZScopeFrame> m_Stack;

	std::unique_ptr<ZHeap> m_Heap         = nullptr;
	ZObject*               m_GlobalObject = nullptr;
};
