#include <gluon/lang/interpreter.h>

#include <gluon/lang/ast.h>
#include <gluon/lang/object.h>

#include <iostream>

Interpreter::Interpreter() : m_scope_stack{this} {
  m_heap = make<Heap>(this);
  m_global_object = m_heap->Allocate<Object>();
}

Interpreter::~Interpreter() {
  m_global_object = nullptr;
  m_heap->Garbage();
}

void Interpreter::Run(ExprPtr expr) {
  m_scope_stack.PushScope();

  Value last_value = expr->accept(*this);

  m_scope_stack.PopScope();

  std::cout << last_value << std::endl;
}

void Interpreter::visit_expr(ExprStmt& expr) {
  UNUSED(expr);
  TODO;
}
void Interpreter::visit_print(PrintStmt& print) {
  UNUSED(print);
  TODO;
}
void Interpreter::visit_var_decl(VarDeclStmt& var_decl) {
  UNUSED(var_decl);
  TODO;
}
void Interpreter::visit_block(BlockStmt& block) {
  UNUSED(block);
  TODO;
}
void Interpreter::visit_if(IfStmt& if_stmt) {
  UNUSED(if_stmt);
  TODO;
}
void Interpreter::visit_while(WhileStmt& while_stmt) {
  UNUSED(while_stmt);
  TODO;
}

Value Interpreter::visit_binary(BinaryExpr& binary) {
  UNUSED(binary);
  TODO;
}

Value Interpreter::visit_grouping(GroupingExpr& grouping) {
  UNUSED(grouping);
  TODO;
}

Value Interpreter::visit_literal(LiteralExpr& literal) {
  UNUSED(literal);
  TODO;
}

Value Interpreter::visit_unary(UnaryExpr& unary) {
  UNUSED(unary);
  TODO;
}

Value Interpreter::visit_variable(VariableExpr& variable) {
  UNUSED(variable);
  TODO;
}

Value Interpreter::visit_assign(AssignExpr& assign) {
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
  frame.variables[name] = Value::kNull;
}

void ScopeStack::SetVariable(const char* name, Value value) {
  for (i32 i = m_stack.element_count() - 1; i >= 0; --i) {
    ScopeFrame& frame = m_stack[i];
    if (auto it = frame.variables.find(name); it != frame.variables.end()) {
      if (it->second.type() != ValueType::Null &&
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
