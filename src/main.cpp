#if 0
#	include <Gluon/App/GluonApp.h>

int main(int argc, char** argv)
{
	GluonApp app(argc, argv);
	return app.Run();
}
#else

#	include <Gluon/VM/AST.h>
#	include <Gluon/VM/Interpreter.h>
#	include <Gluon/VM/Object.h>

void test1(Program* program);
void test2(Program* program);

int main()
{
	auto program = make<Program>();

	// test1(program.get());
	test2(program.get());

	program->dump(0);

	Interpreter interpreter;
	auto        result = interpreter.run(program.get());

	printf("Interpreter returned: %s\n", result.to_string().c_str());

	interpreter.heap()->allocate<Object>();
	interpreter.global_object()->add("foo", Value{interpreter.heap()->allocate<Object>()});

	interpreter.heap()->garbage();

	return 0;
}

void test1(Program* program)
{
	auto* function = program->add<FunctionDeclaration>("foo");

	function->body()->add<ReturnStatement>(
	    make<BinaryExpression>(BinaryOp::Addition,
	                           make<BinaryExpression>(BinaryOp::Addition,
	                                                  make<Literal>(Value{1}),
	                                                  make<Literal>(Value{2})),
	                           make<Literal>(Value{3})));

	program->add<ExpressionStatement>(make<CallExpression>("foo"));
}

void test2(Program* program)
{
	program->add<ExpressionStatement>(make<AssignmentExpression>(AssignmentOperator::Assign,
	                                                             make<Identifier>("x"),
	                                                             make<Literal>(Value{42})));

	auto* function = program->add<FunctionDeclaration>("bar");

	function->body()->add<VariableDeclaration>(make<Identifier>("a"), make<Literal>(Value{2}));

	function->body()->add<VariableDeclaration>(make<Identifier>("b"));

	function->body()->add<ExpressionStatement>(
	    make<AssignmentExpression>(AssignmentOperator::Assign,
	                               make<Identifier>("b"),
	                               make<Literal>(Value{3})));

	function->body()->add<ReturnStatement>(
	    make<BinaryExpression>(BinaryOp::Addition,
	                           make<BinaryExpression>(BinaryOp::Addition,
	                                                  make<Identifier>("a"),
	                                                  make<Identifier>("b")),
	                           make<Identifier>("x")));

	program->add<ExpressionStatement>(make<CallExpression>("bar"));
}
#endif
