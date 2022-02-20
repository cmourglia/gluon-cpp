#include "gluon/lang/ast_printer.h"
#include "gluon/lang/interpreter.h"
#include "gluon/lang/lexer.h"
#include "gluon/lang/parser.h"

using namespace gluon::lang;

int main() {
  auto test_prg = R"(
// Comment
print("Hello");

print("String with \"nested quotes\" and an escaped \\");

let a = 42;
let b = 0.25;
let c = 22;
  )";

  auto lexer = Lexer{test_prg};
  auto program = parse(lexer);

  Interpreter interpreter;
  interpreter.run(program);

  return 0;
}
