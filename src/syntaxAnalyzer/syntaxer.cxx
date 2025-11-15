#include "syntaxer.h"
#include <iostream>

Syntaxer::Syntaxer(Lexer& lexer) : lexer_(lexer), curr_(lexer.get_lexem()) {}

void Syntaxer::NewToken() {
	curr_ = lexer_.get_lexem();
}

bool Syntaxer::match(const string& value) {
	return (value.empty() or curr_.value == value);
}

bool Syntaxer::matchType(Types type) {
	return (curr_.type == type);
}

void Syntaxer::expect(Types type, const string& value) {
	if (!value.empty() and curr_.value != value) {
		std::pair<Types, string> need = { type, value };
		throw std::make_pair(curr_, need);
	}
	NewToken();
}

void Syntaxer::expectType(Types type) {
	if (curr_.type != type) {
		throw std::make_pair(curr_, type);
	}
	NewToken();
}

bool Syntaxer::syntax() {
	Prog();
	std::cout << "successful";
	return true;
}

void Syntaxer::Prog() {
	if (!matchType(Types::END)) {
		Declarations();
	}
}

void Syntaxer::Declarations() {
	while (!matchType(Types::END)) {
		Declaration();
		expect(Types::Punctuation, ";");
	}
}

void Syntaxer::Declaration() {
	if (match("def")) {
		Func();
	} else {
		Var();
	}
}

void Syntaxer::Var() {
	Type();
	expectType(Types::Identificator);
	Init();
}

void Syntaxer::Type() {
	if (match("massive")) {
		NewToken();
		expect(Types::Punctuation, "<");
		Type();
		expect(Types::Punctuation, ">");
	} else if (match("char") or match("int")
		or match("float") or match("let")) {
		NewToken();
	} else {
		throw curr_;
	}
}

void Syntaxer::Init() {
	if (match(";") or match(",") or match(")")) return;
	expect(Types::Operation, "=");
	if (match("{")) {
		list();
	} else {
		Expr();
	}
}

void Syntaxer::Func() {
	expect(Types::Keyword, "def");
	TypeF();
	expectType(Types::Identificator);
	expect(Types::Punctuation, "(");
	Params();
	expect(Types::Punctuation, ")");
	expect(Types::Punctuation, "{");
	FuncR();
	ReturnState();
	expect(Types::Punctuation, ";");
	expect(Types::Punctuation, "}");
}

void Syntaxer::TypeF() {
	if (match("void")) {
		NewToken();
	} else {
		Type();
	}
}

void Syntaxer::Params() {
	if (match("void")) {
		NewToken();
	} else {
		Param();
	}
}

void Syntaxer::Param() {
	Var();
	while (match(",")) {
		NewToken();
		Var();
	}
}

void Syntaxer::FuncR() {
	if (!match("return")) {
		State();
		FuncR();
	}
}

void Syntaxer::State() {
	if (match("if")) {
		IfState();
	} else if (match("for")) {
		ForState();
	} else if (match("while")) {
		WhileState();
	} else if (match("input")) {
		cin();
	} else if (match("return")) {
		ReturnState();
	} else if (match("output")) {
		cout();
	} else if (match("char") or match("int")
		or match("float") or match("let") or match("massive")) {
		Var();
	} else {
		Expr();
	}
	expect(Types::Punctuation, ";");
}

void Syntaxer::MState() {
	while (!match("}")) {
		State();
	}
}

void Syntaxer::IfState() {
	expect(Types::Keyword, "if");
	expect(Types::Punctuation, "(");
	Expr();
	expect(Types::Punctuation, ")");
	expect(Types::Punctuation, "{");
	State();
	MState();
	expect(Types::Punctuation, "}");
	IfStateTail();
}

void Syntaxer::IfStateTail() {
	if (!match(";")) {
		expect(Types::Keyword, "else");
		expect(Types::Punctuation, "{");
		State();
		MState();
		expect(Types::Punctuation, "}");
	}
}

void Syntaxer::ForState() {
	expect(Types::Keyword, "for");
	expect(Types::Punctuation, "(");
	ForStateTail();
}

void Syntaxer::ForStateTail() {
	if (match(";")) {
		NewToken();
	} else {
		A();
		expect(Types::Punctuation, ";");
	}
	Expr();
	expect(Types::Punctuation, ";");
	Expr();
	expect(Types::Punctuation, ")");
	expect(Types::Punctuation, "{");
	State();
	MState();
	expect(Types::Punctuation, "}");
}

void Syntaxer::A() {
	if (match("char") or match("int")
		or match("float") or match("let")
		or match("massive")) {
		Var();
	} else {
		Expr();
	}
}

void Syntaxer::WhileState() {
	expect(Types::Keyword, "while");
	expect(Types::Punctuation, "(");
	Expr();
	expect(Types::Punctuation, ")");
	expect(Types::Punctuation, "{");
	State();
	MState();
	expect(Types::Punctuation, "}");
}

void Syntaxer::ReturnState() {
	expect(Types::Keyword, "return");
	if (!match(";")) {
		Expr();
	}
}

void Syntaxer::Expr() {
	E1();
	while (match(",")) {
		NewToken();
		E1();
	}
}

void Syntaxer::E1() {
	E2();
	while (match("+=") or match("-=") or match("*=")
		or match("/=") or match("%=") or match("=")) {
		NewToken();
		E2();
	}
}

void Syntaxer::E2() {
	E3();
	while (match("or") or match("|")) {
		NewToken();
		E3();
	}
}

void Syntaxer::E3() {
	E4();
	while (match("and") or match("&")) {
		NewToken();
		E4();
	}
}

void Syntaxer::E4() {
	E5();
	while (match("<") or match(">") or match("==")
		or match("!=") or match("<=") or match(">=")) {
		NewToken();
		E5();
	}
}

void Syntaxer::E5() {
	E6();
	while (match("+") or match("-")) {
		NewToken();
		E6();
	}
}

void Syntaxer::E6() {
	E7();
	while (match("*") or match("/") or match("%")) {
		NewToken();
		E7();
	}
}

void Syntaxer::E7() {
	if (match("!") or match("not") or match("-")) {
		NewToken();
		E7();
	} else {
		E8();
	}
}

void Syntaxer::E8() {
	if (match("(")) {
		NewToken();
		Expr();
		expect(Types::Punctuation, ")");
	} else if (matchType(Types::Identificator)) {
		NewToken();
		CI();
	} else {
		expectType(Types::Literal);
	}
}

void Syntaxer::CI() {
	if (match("(")) {
		Call();
	} else {
		Iden();
	}
}

void Syntaxer::Call() {
	expect(Types::Punctuation, "(");
	CallTail();
}

void Syntaxer::CallTail() {
	if (!match(")")) {
		Args();
	}
	expect(Types::Punctuation, ")");
}

void Syntaxer::Args() {
	Expr();
	while (match(",")) {
		NewToken();
		Expr();
	}
}

void Syntaxer::Iden() {
	while (match("[")) {
		NewToken();
		Expr();
		expect(Types::Operation, "]");
	}
}

void Syntaxer::list() {
	expect(Types::Punctuation, "{");
	while (!match("}")) {
		Expr();
	}
	expect(Types::Punctuation, "}");
}

void Syntaxer::cin() {
	expect(Types::Keyword, "input");
	expect(Types::Punctuation, "(");
	expectType(Types::Identificator);
	Iden();
	expect(Types::Punctuation, ")");
}

void Syntaxer::cout() {
	expect(Types::Keyword, "output");
	expect(Types::Punctuation, "(");
	Expr();
	expect(Types::Punctuation, ")");
}