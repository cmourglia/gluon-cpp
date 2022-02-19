#pragma once

#include <gluon/lang/grammar.h>
#include <gluon/lang/heap.h>
#include <gluon/lang/value.h>

#include <beard/containers/array.h>

#include <beard/containers/hash_map.h>
#include <memory>

class Object;

struct ScopeFrame {
  beard::string_hash_map<Value> variables;
};

class Interpreter;
class ScopeStack {
 public:
  explicit ScopeStack(Interpreter* interpreter);

  void DeclareVariable(const char* name);
  void SetVariable(const char* name, Value value);
  Value GetVariable(const char* name);

  void PushScope();
  void PopScope();

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

  void Run(ExprPtr expr);

  [[nodiscard]] Object* global_object() const { return m_global_object; }
  [[nodiscard]] Heap* heap() const { return m_heap.get(); }

 private:
  void VisitExpr(ExprStmt& expr) override;
  void VisitPrint(PrintStmt& print) override;
  void VisitVarDecl(VarDeclStmt& var_decl) override;
  void VisitBlock(BlockStmt& block) override;
  void VisitIf(IfStmt& if_stmt) override;
  void VisitWhile(WhileStmt& while_stmt) override;

  Value VisitBinary(BinaryExpr& binary) override;
  Value VisitGrouping(GroupingExpr& grouping) override;
  Value VisitLiteral(LiteralExpr& literal) override;
  Value VisitUnary(UnaryExpr& unary) override;
  Value VisitVariable(VariableExpr& variable) override;
  Value VisitAssign(AssignExpr& assign) override;

  ExprPtr m_program;
  ScopeStack m_scope_stack;
  std::unique_ptr<Heap> m_heap = nullptr;
  Object* m_global_object = nullptr;
};
