#include "gluon/lang/interpreter.h"

#include <iostream>

#include "gluon/lang/fwd.h"
#include "gluon/lang/object.h"

namespace gluon::lang {

Interpreter::Interpreter() : m_scope_stack{this} {
  m_heap = make<Heap>(this);
  m_global_object = m_heap->Allocate<Object>();
}

Interpreter::~Interpreter() {
  m_global_object = nullptr;
  m_heap->Garbage();
}

void Interpreter::run(ExprPtr expr) {
  m_scope_stack.push_scope();

  Value last_value = expr->accept(*this);

  m_scope_stack.pop_scope();

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
  Value left = binary.left()->accept(*this);
  Value right = binary.right()->accept(*this);
  switch (binary.op().type) {
    case TokenType::Plus:
      return left + right;
    case TokenType::Minus:
      return left - right;
    case TokenType::Star:
      return left * right;
    case TokenType::Slash:
      return left / right;
    case TokenType::Percent:
      return left % right;
  }

  throw std::runtime_error{"Invalid binary operator"};
}

Value Interpreter::visit_grouping(GroupingExpr& grouping) {
  return grouping.expr()->accept(*this);
}

Value Interpreter::visit_literal(LiteralExpr& literal) {
  return literal.value();
}

Value Interpreter::visit_unary(UnaryExpr& unary) {
  auto value = unary.right()->accept(*this);
  switch (unary.op().type) {
    case TokenType::Minus:
      return -value;
    case TokenType::Not:
      return !value;
  }

  throw std::runtime_error{"Invalid unary operator"};
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

void ScopeStack::push_scope() {
  m_stack.add({});
}

void ScopeStack::pop_scope() {
  m_stack.pop_and_discard();
}

// FIXME: Not sure about this.
void ScopeStack::declare_variable(const char* name) {
  ScopeFrame& frame = m_stack.last();
  frame.variables[name] = Value::kNull;
}

void ScopeStack::set_variable(const char* name, Value value) {
  for (i32 i = m_stack.element_count() - 1; i >= 0; --i) {
    ScopeFrame& frame = m_stack[i];
    if (auto it = frame.variables.find(name); it != frame.variables.end()) {
      if (it->second.type() != ValueType::Null &&
          value.type() != it->second.type()) {
        throw std::runtime_error{"Cannot mutate variable types"};
      }

      it->second = value;
      return;
    }
  }
}

Value ScopeStack::get_variable(const char* name) {
  for (i32 i = m_stack.element_count() - 1; i >= 0; --i) {
    ScopeFrame& frame = m_stack[i];
    if (auto it = frame.variables.find(name); it != frame.variables.end()) {
      return it->second;
    }
  }

  throw std::runtime_error{"Cannot find variable"};
}

}  // namespace gluon::lang