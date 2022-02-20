#pragma once

#include "gluon/lang/object.h"

namespace gluon::lang {

struct ScopeNode;

class Function : public Object {
 public:
  Function(std::string name, ScopeNode* body)
      : m_name{std::move(name)}, m_body{body} {}

  const char* to_string() const override { return "Function"; }

  std::string name() const { return m_name; }

  ScopeNode* body() const { return m_body; }

  bool is_function() const override { return true; }

 private:
  std::string m_name;
  ScopeNode* m_body;
};

}  // namespace gluon::lang