#pragma once

#include <gluon/lang/heap.h>
#include <gluon/lang/value.h>

#include <beard/containers/array.h>

#include <beard/containers/hash_map.h>
#include <memory>

struct AstNode;
struct ScopeNode;
class Object;

struct ScopeFrame
{
    ScopeNode* node;

    beard::string_hash_map<Value> variables;
};

class Interpreter
{
public:
    Interpreter();
    ~Interpreter();

    NONCOPYABLE(Interpreter);
    NONMOVEABLE(Interpreter);

    Value Run(ScopeNode* node);

    Object* global_object() const
    {
        return m_global_object;
    }

    Heap* heap() const
    {
        return m_heap.get();
    }

    void  DeclareVariable(const char* name);
    void  SetVariable(const char* name, Value value);
    Value GetVariable(const char* name);

private:
    void PushScope(ScopeNode* node);
    void PopScope(ScopeNode* node);

    beard::array<ScopeFrame> m_stack;

    std::unique_ptr<Heap> m_heap          = nullptr;
    Object*               m_global_object = nullptr;
};
