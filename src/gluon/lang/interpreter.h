#pragma once

#include <beard/containers/array.h>
#include <beard/containers/hash_map.h>

#include <memory>

#include "gluon/lang/grammar.h"
#include "gluon/lang/heap.h"
#include "gluon/lang/value.h"

namespace gluon::lang {

class Object;

struct ScopeFrame {
  beard::string_hash_map<Value> variables;
};

class Interpreter;
class ScopeStack {
 public:
  explicit ScopeStack(Interpreter* interpreter);

  void declare_variable(const char* name);
  void set_variable(const char* name, Value value);
  Value get_variable(const char* name);

  void push_scope();
  void pop_scope();

 private:
  Interpreter* m_interpreter = nullptr;
  beard::array<ScopeFrame> m_stack;
};

class Interpreter : public StmtVisitor, public ExprVisitor {
 public:
  Interpreter();
  ~Interpreter();

  NONCOPYABLE(Interpreter);
  NONMOVEABLE(Interpreter);

  void run(ExprPtr expr);

  [[nodiscard]] Object* global_object() const { return m_global_object; }
  [[nodiscard]] Heap* heap() const { return m_heap.get(); }

 private:
  void visit_expr(ExprStmt& expr) override;
  void visit_print(PrintStmt& print) override;
  void visit_var_decl(VarDeclStmt& var_decl) override;
  void visit_block(BlockStmt& block) override;
  void visit_if(IfStmt& if_stmt) override;
  void visit_while(WhileStmt& while_stmt) override;

  Value visit_binary(BinaryExpr& binary) override;
  Value visit_grouping(GroupingExpr& grouping) override;
  Value visit_literal(LiteralExpr& literal) override;
  Value visit_unary(UnaryExpr& unary) override;
  Value visit_variable(VariableExpr& variable) override;
  Value visit_assign(AssignExpr& assign) override;

  ExprPtr m_program;
  ScopeStack m_scope_stack;
  std::unique_ptr<Heap> m_heap = nullptr;
  Object* m_global_object = nullptr;
};

}  // namespace gluon::lang
