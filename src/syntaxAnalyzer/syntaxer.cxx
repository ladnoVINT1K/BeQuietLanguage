#include "syntaxer.h"
#include <iostream>

Syntaxer::Syntaxer(Lexer& lexer) : lexer_(lexer), curr_(lexer.get_lexem()),
tid_(), tf_(), stack_(), depth_(0) {
}

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
	return true;
}

void Syntaxer::Prog() {
	if (!matchType(Types::END)) {
		Declarations();
	}
}

void Syntaxer::Declarations() {
	while (!matchType(Types::END)) {
		if (match("def")) {
			Func();
		} else {
			Var();
		}
		expect(Types::Punctuation, ";");
	}
}

void Syntaxer::Var() {
	Type();
	id_ = curr_.value;
	if (tid_.check_exist(id_)) throw std::runtime_error(id_ + " was already initialized");
	expectType(Types::Identificator);
	if (match(";") or match(",") or match(")")) {
		if (type_ == "let") throw std::runtime_error("let must be initialized");
		tid_.push_id(id_, info(to_idtype(type_), depth_));
		depth_ = 0;
		return;
	}
	expect(Types::Operation, "=");
	if (match("{")) {
		init_list();
		tid_.push_id(id_, info(to_idtype(type_), depth_));
		depth_ = 0;
	} else {
		Expr();
		infoStack buff = stack_.pop_stack();
		check_init(type_, buff.t_);
		tid_.push_id(id_, info(to_idtype(buff.t_), depth_, buff.v_));
	}
}

void Syntaxer::Type() {
	if (match("massive")) {
		++depth_;
		NewToken();
		expect(Types::Punctuation, "<");
		Type();
		expect(Types::Punctuation, ">");
	} else if (match("int") or match("float")
		or match("let") or match("string")) {
		type_ = curr_.value;
		NewToken();
	} else {
		throw curr_;
	}
}

void Syntaxer::Func() {
	expect(Types::Keyword, "def");
	TypeF();
	func_.unic_name = curr_.value;
	func_.name = curr_.value;
	func_.res.d_ = depth_;
	depth_ = 0;
	func_.res.t_ = to_ftype(type_);
	expectType(Types::Identificator);
	expect(Types::Punctuation, "(");
	tid_.create_tid();
	Params();
	expect(Types::Punctuation, ")");
	tf_.new_func(func_);
	func_.unic_name = "";
	func_.params.clear();
	expect(Types::Punctuation, "{");
	FuncR();
	ReturnState();
	expect(Types::Punctuation, ";");
	expect(Types::Punctuation, "}");
	tid_.delete_tid();
}

void Syntaxer::TypeF() {
	if (match("void")) {
		type_ = curr_.value;
		NewToken();
	} else {
		Type();
	}
}

void Syntaxer::Params() {
	if (!match(")")) {
		Type();
		if (type_ == "let") throw std::runtime_error("param can't be let");
		param_.d_ = depth_;
		func_.unic_name += type_;
		param_.t_ = to_ftype(type_);
		id_ = curr_.value;
		expectType(Types::Identificator);
		if (tid_.check_exist(id_)) {
			throw std::runtime_error("you’ve already declared a variable named " + id_);
		}
		func_.params.push_back(param_);
		tid_.push_id(id_, info(to_idtype(type_), depth_));
		depth_ = 0;
		while (match(",")) {
			NewToken();
			Type();
			if (type_ == "let") throw std::runtime_error("param can't be let");
			param_.d_ = depth_;
			func_.unic_name += type_;
			param_.t_ = to_ftype(type_);
			id_ = curr_.value;
			expectType(Types::Identificator);
			if (tid_.check_exist(id_)) {
				throw std::runtime_error("you’ve already declared a variable named " + id_);
			}
			func_.params.push_back(param_);
			tid_.push_id(id_, info(to_idtype(type_), depth_));
			depth_ = 0;
		}
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
	} else if (match("string") or match("int")
		or match("float") or match("let") or match("massive")) {
		Var();
	} else {
		Expr();
		stack_.pop_stack();
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
	if (!stack_.check_if()) throw std::runtime_error("expression must be bool type in if");
	expect(Types::Punctuation, "{");
	tid_.create_tid();
	State();
	MState();
	expect(Types::Punctuation, "}");
	tid_.delete_tid();
	if (!match(";")) {
		expect(Types::Keyword, "else");
		expect(Types::Punctuation, "{");
		tid_.create_tid();
		State();
		MState();
		expect(Types::Punctuation, "}");
		tid_.delete_tid();
	}
}

void Syntaxer::ForState() {
	expect(Types::Keyword, "for");
	expect(Types::Punctuation, "(");
	tid_.create_tid();
	if (match(";")) {
		NewToken();
	} else {
		if (match("int") or match("float")
			or match("let") or match("massive")) {
			Var();
		} else {
			Expr();
			stack_.pop_stack();
		}
		expect(Types::Punctuation, ";");
	}
	Expr();
	if (!stack_.check_if()) throw std::runtime_error("expression must be bool type in for");
	expect(Types::Punctuation, ";");
	Expr();
	expect(Types::Punctuation, ")");
	stack_.pop_stack();
	expect(Types::Punctuation, "{");
	State();
	MState();
	expect(Types::Punctuation, "}");
	tid_.delete_tid();
}

void Syntaxer::WhileState() {
	expect(Types::Keyword, "while");
	expect(Types::Punctuation, "(");
	Expr();
	expect(Types::Punctuation, ")");
	if (!stack_.check_if()) throw std::runtime_error("expression must be bool type in while");
	expect(Types::Punctuation, "{");
	tid_.create_tid();
	State();
	MState();
	expect(Types::Punctuation, "}");
	tid_.delete_tid();
}

void Syntaxer::ReturnState() {
	info_func buff = tf_.v_[tf_.v_.size() - 1];
	expect(Types::Keyword, "return");
	if (!match(";")) {
		Expr();
		infoStack buff_st = stack_.pop_stack();
		if (check_return(buff.res.t_, buff_st.t_)) {
			return;
		} else if (buff.res.t_ == typefunc::Let) {
			tf_.v_[tf_.v_.size() - 1].res.t_ = to_ftype(buff_st.t_);
		} else throw std::runtime_error("Mismatched return type in " + buff.name);
	} else if (buff.res.t_ == typefunc::Let) {
		tf_.v_[tf_.v_.size() - 1].res.t_ = typefunc::Void;
	} else if (buff.res.t_ != typefunc::Void) throw std::runtime_error("func " + buff.name + " must return anything");
}

void Syntaxer::Expr() {
	E1();
	while (match(",")) {
		stack_.pop_stack();
		NewToken();
		E1();
	}
}

void Syntaxer::E1() {
	E2();
	while (match("+=") or match("-=") or match("*=")
		or match("/=") or match("%=") or match("=")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E2();
		stack_.check_bin();
	}
}

void Syntaxer::E2() {
	E3();
	while (match("or") or match("|")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E3();
		stack_.check_bin();
	}
}

void Syntaxer::E3() {
	E4();
	while (match("and") or match("&")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E4();
		stack_.check_bin();
	}
}

void Syntaxer::E4() {
	E5();
	while (match("<") or match(">") or match("==")
		or match("!=") or match("<=") or match(">=")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E5();
		stack_.check_bin();
	}
}

void Syntaxer::E5() {
	E6();
	while (match("+") or match("-")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E6();
		stack_.check_bin();
	}
}

void Syntaxer::E6() {
	E7();
	while (match("*") or match("/") or match("%")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E7();
		stack_.check_bin();
	}
}

void Syntaxer::E7() {
	if (match("!") or match("not") or match("-")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E7();
		stack_.check_uno();
	} else {
		E8();
	}
}

void Syntaxer::E8() {
	if (match("(")) {
		NewToken();
		Expr();
		expect(Types::Punctuation, ")");
	} else if (match("{")) {
		expr_list();
		first = 1;
		stack_.push_stack(infoStack(listt, Types::Literal, depth_));
		depth_ = 0;
	} else if (matchType(Types::Identificator)) {
		string call_n = curr_.value;
		NewToken();
		if (match("(")) {
			expect(Types::Punctuation, "(");
			vector<param> call_;
			if (!match(")")) {
				E1();
				auto buff = stack_.pop_stack();
				call_.push_back({ to_ftype(buff.t_), buff.d_ });
				while (match(",")) {
					NewToken();
					E1();
					buff = stack_.pop_stack();
					call_.push_back({ to_ftype(buff.t_), buff.d_ });
				}
			}
			expect(Types::Punctuation, ")");
			if (!tf_.check_call(call_n, call_)) throw std::runtime_error("don't find func " + call_n + " with this params");
			auto res = tf_.call_res(call_n, call_);
			stack_.push_stack(res);
		} else {
			auto buff = tid_.check_exist(call_n);
			if (!buff) throw std::runtime_error(call_n + " not exist");
			int dpth = 0;
			while (match("[")) {
				++dpth;
				NewToken();
				Expr();
				if (stack_.pop_stack().t_ != typestack::Int) throw std::runtime_error("in [] must be int");
				expect(Types::Operation, "]");
			}
			if (dpth > buff.value().d_) throw std::runtime_error("wrong massive depth");
			stack_.push_stack(info(buff.value().t_, buff.value().d_ - dpth));
		}
	} else {
		expr_ = curr_.value;
		expectType(Types::Literal);
		stack_.push_stack(expr_);
	}
}

void Syntaxer::expr_list() {
	list_d.push("{");
	expect(Types::Punctuation, "{");
	while (!match("}")) {
		if (match("{")) {
			expr_list();
			while (match(",")) {
				NewToken();
				expr_list();
			}
		} else {
			if (first) {
				depth_ = list_d.size();
				Expr();
				auto buff = stack_.pop_stack();
				listt = buff.t_;
				first = false;
			} else {
				Expr();
				auto buff = stack_.pop_stack();
				if (buff.t_ != to_sttype(type_)) throw std::runtime_error("different type in massive");
				if (list_d.size() != depth_) throw std::runtime_error("different massive depth");
			}

		}
	}
	expect(Types::Punctuation, "}");
	list_d.pop();
}

void Syntaxer::init_list() {
	list_d.push("{");
	expect(Types::Punctuation, "{");
	while (!match("}")) {
		if (match("{")) {
			init_list();
			while (match(",")) {
				NewToken();
				init_list();
			}
		} else {
			Expr();
			auto buff = stack_.pop_stack();
			if (type_ == "let") {
				if (buff.t_ == typestack::Int) type_ = "int";
				else if (buff.t_ == typestack::Float) type_ = "float";
				else if (buff.t_ == typestack::Str) type_ = "string";
				else throw std::runtime_error("wrong type in massive");
				depth_ = list_d.size();
			}
			if (buff.t_ != to_sttype(type_)) throw std::runtime_error("wrong type in massive");
			if (list_d.size() != depth_) throw std::runtime_error("wrong massive depth");
		}
	}
	expect(Types::Punctuation, "}");
	list_d.pop();
}

void Syntaxer::cin() {
	expect(Types::Keyword, "input");
	expect(Types::Punctuation, "(");
	id_ = curr_.value;
	expectType(Types::Identificator);
	auto buff = tid_.check_exist(id_);
	if (!buff) throw std::runtime_error(id_ + " not exist");
	int dpth = 0;
	while (match("[")) {
		++dpth;
		NewToken();
		Expr();
		if (stack_.pop_stack().t_ != typestack::Int) throw std::runtime_error("in [] must be num");
		expect(Types::Operation, "]");
	}
	if (dpth > buff.value().d_) throw std::runtime_error("wrong massive depth");
	if (dpth < buff.value().d_) throw std::runtime_error("can't input massive");
	expect(Types::Punctuation, ")");
}

void Syntaxer::cout() {
	expect(Types::Keyword, "output");
	expect(Types::Punctuation, "(");
	Expr();
	infoStack buff = stack_.pop_stack();
	if (buff.d_ > 0) throw std::runtime_error("can't output massive");
	expect(Types::Punctuation, ")");
}

typefunc Syntaxer::to_ftype(string type) {
	if (type == "int") return typefunc::Int;
	if (type == "float") return typefunc::Float;
	if (type == "string") return typefunc::String;
	if (type == "void") return typefunc::Void;
	return typefunc::Let;
}

TypesId Syntaxer::to_idtype(string type) {
	if (type == "int") return TypesId::Int;
	if (type == "float") return TypesId::Float;
	if (type == "string") return TypesId::Str;
}

typefunc Syntaxer::to_ftype(TypesId type) {
	if (type == TypesId::Int) return typefunc::Int;
	if (type == TypesId::Float) return typefunc::Float;
	if (type == TypesId::Str) return typefunc::String;
	return typefunc::Let;
}

TypesId Syntaxer::to_idtype(typefunc type) {
	if (type == typefunc::Int) return TypesId::Int;
	if (type == typefunc::Float) return TypesId::Float;
	if (type == typefunc::String) return TypesId::Str;
}

void Syntaxer::check_init(string type1, typestack type2) {
	if (((type1 == "int" or type1 == "float") and (type2 == typestack::Int or type2 == typestack::Float)) or
		(type_ == "string" and type2 == typestack::Str) or type_ == "let") {
		return;
	} else {
		throw std::runtime_error("types don't match");
	}
}

TypesId Syntaxer::to_idtype(typestack type) {
	if (type == typestack::Int) return TypesId::Int;
	if (type == typestack::Float) return TypesId::Float;
	if (type == typestack::Str) return TypesId::Str;
}

bool Syntaxer::check_return(typefunc type1, typestack type2) {
	if (((type2 == typestack::Int or type2 == typestack::Float) and
		(type1 == typefunc::Int or type1 == typefunc::Float)) or
		(type2 == typestack::Str and type1 == typefunc::String)) return 1;
	else return 0;
}

typefunc Syntaxer::to_ftype(typestack type) {
	if (type == typestack::Int) return typefunc::Int;
	if (type == typestack::Float) return typefunc::Float;
	if (type == typestack::Void) return typefunc::Void;
	if (type == typestack::Str) return typefunc::String;
}

typestack Syntaxer::to_sttype(string type) {
	if (type == "int") return typestack::Int;
	if (type == "float") return typestack::Float;
	if (type == "string") return typestack::Str;
}