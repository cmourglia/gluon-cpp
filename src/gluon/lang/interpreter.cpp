#include <gluon/lang/interpreter.h>

#include <gluon/lang/ast.h>
#include <gluon/lang/object.h>

#include <iostream>

Interpreter::Interpreter() : m_scope_stack{this} {
  m_heap = Make<Heap>(this);
  m_global_object = m_heap->Allocate<Object>();
}

Interpreter::~Interpreter() {
  m_global_object = nullptr;
  m_heap->Garbage();
}

void Interpreter::Run(ExprPtr expr) {
  m_scope_stack.PushScope();

  Value last_value = expr->Accept(*this);

  m_scope_stack.PopScope();

  std::cout << last_value << std::endl;
}

void Interpreter::VisitExpr(ExprStmt& expr) {
  UNUSED(expr);
  TODO;
}
void Interpreter::VisitPrint(PrintStmt& print) {
  UNUSED(print);
  TODO;
}
void Interpreter::VisitVarDecl(VarDeclStmt& var_decl) {
  UNUSED(var_decl);
  TODO;
}
void Interpreter::VisitBlock(BlockStmt& block) {
  UNUSED(block);
  TODO;
}
void Interpreter::VisitIf(IfStmt& if_stmt) {
  UNUSED(if_stmt);
  TODO;
}
void Interpreter::VisitWhile(WhileStmt& while_stmt) {
  UNUSED(while_stmt);
  TODO;
}

Value Interpreter::VisitBinary(BinaryExpr& binary) {
  UNUSED(binary);
  TODO;
}

Value Interpreter::VisitGrouping(GroupingExpr& grouping) {
  UNUSED(grouping);
  TODO;
}

Value Interpreter::VisitLiteral(LiteralExpr& literal) {
  UNUSED(literal);
  TODO;
}

Value Interpreter::VisitUnary(UnaryExpr& unary) {
  UNUSED(unary);
  TODO;
}

Value Interpreter::VisitVariable(VariableExpr& variable) {
  UNUSED(variable);
  TODO;
}

Value Interpreter::VisitAssign(AssignExpr& assign) {
  UNUSED(assign);
  TODO;
}

ScopeStack::ScopeStack(Interpreter* interpreter) : m_interpreter{interpreter} {}

void ScopeStack::PushScope() {
  m_stack.add({});
}

void ScopeStack::PopScope() {
  m_stack.pop_and_discard();
}

// FIXME: Not sure about this.
void ScopeStack::DeclareVariable(const char* name) {
  ScopeFrame& frame = m_stack.last();
  frame.variables[name] = Value::Null;
}

void ScopeStack::SetVariable(const char* name, Value value) {
  for (i32 i = m_stack.element_count() - 1; i >= 0; --i) {
    ScopeFrame& frame = m_stack[i];
    if (auto it = frame.variables.find(name); it != frame.variables.end()) {
      if (it->second.type() != ValueType::kNull &&
          value.type() != it->second.type()) {
        throw std::exception("Cannot mutate variable types");
      }

      it->second = value;
      return;
    }
  }
}

Value ScopeStack::GetVariable(const char* name) {
  for (i32 i = m_stack.element_count() - 1; i >= 0; --i) {
    ScopeFrame& frame = m_stack[i];
    if (auto it = frame.variables.find(name); it != frame.variables.end()) {
      return it->second;
    }
  }

  throw std::exception("Cannot find variable");
}
