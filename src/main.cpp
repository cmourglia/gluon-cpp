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
	auto program = Make<VM::Program>();

	auto* function = program->Push<VM::FunctionDeclaration>("foo");

	auto* block1 = function->MakeBody<VM::BlockStatement>();

	auto binaryExpression = Make<
	    VM::BinaryExpression>(VM::BinaryOp::Addition,
	                          Make<VM::BinaryExpression>(VM::BinaryOp::Addition,
	                                                     Make<VM::Literal>(
	                                                         VM::Value{1}),
	                                                     Make<VM::Literal>(
	                                                         VM::Value{2})),
	                          Make<VM::Literal>(VM::Value{3}));

	auto* returnStmt = block1->Push<VM::ReturnStatement>(binaryExpression);
	UNUSED(returnStmt);

	auto* expressionStmt = program->Push<VM::ExpressionStatement>(
	    Make<VM::CallExpression>("foo"));
	UNUSED(expressionStmt);

	program->Dump(0);

	VM::Interpreter interpreter;
	auto            result = interpreter.Run(program.get());

	printf("Interpreter returned: ");
	result.Dump();
	printf("\n");

	interpreter.GetHeap()->Allocate<VM::Object>();
	interpreter.GetGlobalObject()
	    ->Add("foo", VM::Value{interpreter.GetHeap()->Allocate<VM::Object>()});

	interpreter.GetHeap()->Garbage();

	return 0;
}

#endif