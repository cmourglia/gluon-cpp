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

int main()
{
	auto program = make<VM::Program>();

	auto* function = program->add<VM::FunctionDeclaration>("foo");

	auto* block1 = function->make_body<VM::BlockStatement>();

	auto binaryExpression = make<
	    VM::BinaryExpression>(VM::BinaryOp::Addition,
	                          make<VM::BinaryExpression>(VM::BinaryOp::Addition,
	                                                     make<VM::Literal>(
	                                                         VM::Value{1}),
	                                                     make<VM::Literal>(
	                                                         VM::Value{2})),
	                          make<VM::Literal>(VM::Value{3}));

	auto* returnStmt = block1->add<VM::ReturnStatement>(binaryExpression);
	UNUSED(returnStmt);

	auto* expressionStmt = program->add<VM::ExpressionStatement>(
	    make<VM::CallExpression>("foo"));
	UNUSED(expressionStmt);

	program->dump(0);

	VM::Interpreter interpreter;
	auto            result = interpreter.run(program.get());

	printf("Interpreter returned: %s\n", result.to_string().c_str());

	interpreter.heap()->allocate<VM::Object>();
	interpreter.global_object()
	    ->add("foo", VM::Value{interpreter.heap()->allocate<VM::Object>()});

	interpreter.heap()->garbage();

	return 0;
}

#endif