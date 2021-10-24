#if 1
#	include <Gluon/App/Gluon_App.h>

int main(int argc, char** argv)
{
	GluonApp app(argc, argv);
	return app.Run();
}
#else

#	include <Gluon/VM/Gluon_AST.h>
#	include <Gluon/VM/Gluon_Interpreter.h>
#	include <Gluon/VM/Gluon_Object.h>

void test1(ZProgram* program);
void test2(ZProgram* program);

int main()
{
	auto program = Make<ZProgram>();

	// test1(program.get());
	test2(program.get());

	program->Dump(0);

	ZInterpreter interpreter;
	auto         Result = interpreter.Run(program.get());

	printf("Interpreter returned: %s\n", Result.ToString().c_str());

	interpreter.Heap()->Allocate<ZObject>();
	interpreter.GlobalObject()->Add("foo", ZValue{interpreter.Heap()->Allocate<ZObject>()});

	interpreter.Heap()->Garbage();

	return 0;
}

void test1(ZProgram* program)
{
	auto* function = program->add<ZFunctionDeclaration>("foo");

	function->Body()->add<ZReturnStatement>(Make<ZBinaryExpression>(EBinaryOp::Addition,
	                                                                Make<ZBinaryExpression>(EBinaryOp::Addition,
	                                                                                        Make<ZLiteral>(ZValue{1}),
	                                                                                        Make<ZLiteral>(ZValue{2})),
	                                                                Make<ZLiteral>(ZValue{3})));

	program->add<ExpressionStatement>(Make<ZCallExpression>("foo"));
}

void test2(ZProgram* program)
{
	program->add<ExpressionStatement>(
	    Make<AssignmentExpression>(EAssignmentOperator::Assign, Make<ZIdentifier>("x"), Make<ZLiteral>(ZValue{42})));

	auto* function = program->add<ZFunctionDeclaration>("bar");

	function->Body()->add<ZVariableDeclaration>(Make<ZIdentifier>("a"), Make<ZLiteral>(ZValue{2}));

	function->Body()->add<ZVariableDeclaration>(Make<ZIdentifier>("b"));

	function->Body()->add<ExpressionStatement>(
	    Make<AssignmentExpression>(EAssignmentOperator::Assign, Make<ZIdentifier>("b"), Make<ZLiteral>(ZValue{3})));

	function->Body()->add<ZReturnStatement>(Make<ZBinaryExpression>(EBinaryOp::Addition,
	                                                                Make<ZBinaryExpression>(EBinaryOp::Addition,
	                                                                                        Make<ZIdentifier>("a"),
	                                                                                        Make<ZIdentifier>("b")),
	                                                                Make<ZIdentifier>("x")));

	program->add<ExpressionStatement>(Make<ZCallExpression>("bar"));
}
#endif
