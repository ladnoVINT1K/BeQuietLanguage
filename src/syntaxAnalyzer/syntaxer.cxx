#include "syntaxer.h"
#include <iostream>

Syntaxer::Syntaxer(Lexer& lexer, tf& tf) : lexer_(lexer), curr_(lexer.get_lexem()),
tid_(), tf_(tf), stack_(), depth_(0) {
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

Poliz Syntaxer::syntax() {
	Prog();
	return poliz_;
}

void Syntaxer::Prog() {
	if (!matchType(Types::END)) {
		Declarations();
	}
}

void Syntaxer::Declarations() {
	poliz_.blank();
	poliz_.push_poliz({ PolizType::COMMAND, "!" });
	while (!matchType(Types::END)) {
		if (match("def")) {
			Func();
		} else {
			int p1 = poliz_.get_gpt();
			Var();
			if (poliz_.get_value(main) == "") {
				poliz_.push_address(p1);
				main = poliz_.get_gpt();
				poliz_.blank();
				poliz_.push_poliz({ PolizType::COMMAND, "!" });
			}
		}
		expect(Types::Punctuation, ";");
	}
}

void Syntaxer::Var() {
	Type();
	id_ = curr_.value;
	poliz_.push_poliz({ PolizType::COMMAND, "INIT" });
	poliz_.push_poliz({ PolizType::ID,id_ });
	if (tid_.check_exist_init(id_)) throw std::logic_error(id_ + " was already initialized");
	expectType(Types::Identificator);
	if (match("=")) {
		expect(Types::Operation, "=");
		if (match("{")) {
			if (depth_ == 0) throw std::logic_error("wrong init type");
			poliz_.push_poliz({ PolizType::COMMAND, "BEGIN_LIST" });
			init_list();
			poliz_.push_poliz({ PolizType::COMMAND, "END_LIST" });
			tid_.push_id(id_, info(to_idtype(type_), depth_));
			poliz_.push_poliz({ PolizType::COMMAND, "Init_" + type_ + "_" + std::to_string(depth_) });
		} else {
			E1();
			infoStack buff = stack_.pop_stack();
			check_init(type_, buff.t_);
			tid_.push_id(id_, info(to_idtype(buff.t_), buff.d_, buff.v_));
			if (type_ == "let") poliz_.push_poliz({ PolizType::COMMAND, "Init_" + to_str(buff.t_) + "_" + std::to_string(depth_) });
			else poliz_.push_poliz({ PolizType::COMMAND, "Init_" + type_ + "_" + std::to_string(depth_) });
		}
	} else if (match(",") or match(";") or match(")")) {
		if (type_ == "let") throw std::logic_error("let must be initialized");
		tid_.push_id(id_, info(to_idtype(type_), depth_));
		poliz_.push_poliz({ PolizType::COMMAND, "Init_" + type_ + "_" + std::to_string(depth_) });
	}
	while (match(",")) {
		NewToken();
		id_ = curr_.value;
		poliz_.push_poliz({ PolizType::COMMAND, "INIT" });
		poliz_.push_poliz({ PolizType::ID, id_ });
		if (tid_.check_exist_init(id_)) throw std::logic_error(id_ + " was already initialized");
		expectType(Types::Identificator);
		if (match("=")) {
			expect(Types::Operation, "=");
			if (match("{")) {
				poliz_.push_poliz({ PolizType::COMMAND, "BEGIN_LIST" });
				init_list();
				poliz_.push_poliz({ PolizType::COMMAND, "END_LIST" });
				poliz_.push_poliz({ PolizType::COMMAND, "Init_" + type_ + "_" + std::to_string(depth_) });
				tid_.push_id(id_, info(to_idtype(type_), depth_));
			} else {
				E1();
				infoStack buff = stack_.pop_stack();
				check_init(type_, buff.t_);
				tid_.push_id(id_, info(to_idtype(buff.t_), buff.d_, buff.v_));
				tid_.push_id(id_, info(to_idtype(buff.t_), buff.d_, buff.v_));
				if (type_ == "let") poliz_.push_poliz({ PolizType::COMMAND, "Init_" + to_str(buff.t_) + "_" + std::to_string(depth_) });
				else poliz_.push_poliz({ PolizType::COMMAND, "Init_" + type_ + "_" + std::to_string(depth_) });
			}
		} else {
			if (type_ == "let") throw std::logic_error("let must be initialized");
			tid_.push_id(id_, info(to_idtype(type_), depth_));
			poliz_.push_poliz({ PolizType::COMMAND, "Init_" + type_ + "_" + std::to_string(depth_) });
		}
	}
	depth_ = 0;
	return;
}

void Syntaxer::Type() {
	if (match("massive") and depth_ == 0) {
		++depth_;
		NewToken();
		expect(Types::Operation, "<");
		Type();
		expect(Types::Operation, ">");
	} else if (match("int") or match("float")
		or match("let") or match("char")) {
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
	if (func_.name == "main") {
		poliz_.push_address(main);
	}
	func_.res.d_ = depth_;
	func_.address = poliz_.get_gpt();
	depth_ = 0;
	func_.res.t_ = to_ftype(type_);
	expectType(Types::Identificator);
	expect(Types::Punctuation, "(");
	poliz_.push_poliz({ PolizType::COMMAND, "CREATE_TID" });
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
	poliz_.push_poliz({ PolizType::COMMAND, "DELETE_TID" });
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
		if (type_ == "let") throw std::logic_error("param can't be let");
		param_.d_ = depth_;
		func_.unic_name += type_;
		param_.t_ = to_ftype(type_);
		id_ = curr_.value;
		param_.name = id_;
		expectType(Types::Identificator);
		if (tid_.check_exist_init(id_)) {
			throw std::logic_error("you’ve already declared a variable named " + id_);
		}
		func_.params.push_back(param_);
		tid_.push_id(id_, info(to_idtype(type_), depth_));
		depth_ = 0;
		while (match(",")) {
			NewToken();
			Type();
			if (type_ == "let") throw std::logic_error("param can't be let");
			param_.d_ = depth_;
			func_.unic_name += type_;
			param_.t_ = to_ftype(type_);
			id_ = curr_.value;
			param_.name = id_;
			expectType(Types::Identificator);
			if (tid_.check_exist_init(id_)) {
				throw std::logic_error("you’ve already declared a variable named " + id_);
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
	} else if (match("char") or match("int")
		or match("float") or match("let") or match("massive")) {
		Var();
	} else {
		Expr();
		poliz_.push_poliz({ PolizType::COMMAND, ";" });
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
	if (!stack_.check_if()) throw std::logic_error("expression must be bool type in if");
	int p1 = poliz_.get_gpt();
	poliz_.blank();
	poliz_.push_poliz({ PolizType::COMMAND, "F!" });
	expect(Types::Punctuation, "{");
	poliz_.push_poliz({ PolizType::COMMAND, "CREATE_TID" });
	tid_.create_tid();
	State();
	MState();
	expect(Types::Punctuation, "}");
	tid_.delete_tid();
	poliz_.push_poliz({ PolizType::COMMAND, "DELETE_TID" });
	int p2 = poliz_.get_gpt();
	poliz_.blank();
	poliz_.push_poliz({ PolizType::COMMAND, "!" });
	poliz_.push_address(p1);
	if (!match(";")) {
		expect(Types::Keyword, "else");
		expect(Types::Punctuation, "{");
		poliz_.push_poliz({ PolizType::COMMAND, "CREATE_TID" });
		tid_.create_tid();
		State();
		MState();
		expect(Types::Punctuation, "}");
		tid_.delete_tid();
		poliz_.push_poliz({ PolizType::COMMAND, "DELETE_TID" });
	}
	poliz_.push_address(p2);
}

void Syntaxer::ForState() {
	expect(Types::Keyword, "for");
	expect(Types::Punctuation, "(");
	poliz_.push_poliz({ PolizType::COMMAND, "CREATE_TID" });
	tid_.create_tid();
	if (match(";")) {
		NewToken();
	} else {
		if (match("int") or match("float")
			or match("let") or match("massive") or match("char")) {
			Var();
		} else {
			Expr();
			stack_.pop_stack();
			poliz_.push_poliz({ PolizType::COMMAND, ";" });
		}
		expect(Types::Punctuation, ";");
	}
	int p2 = poliz_.get_gpt();
	Expr();
	int p3 = poliz_.get_gpt();
	poliz_.blank();
	poliz_.push_poliz({ PolizType::COMMAND, "F!" });
	int p4 = poliz_.get_gpt();
	poliz_.blank();
	poliz_.push_poliz({ PolizType::COMMAND, "!" });
	if (!stack_.check_if()) throw std::logic_error("expression must be bool type in for");
	expect(Types::Punctuation, ";");
	int p5 = poliz_.get_gpt();
	Expr();
	poliz_.push_poliz({ PolizType::COMMAND, ";" });
	poliz_.push_poliz({ PolizType::LITERAL, std::to_string(p2) });
	poliz_.push_poliz({ PolizType::COMMAND, "!" });
	expect(Types::Punctuation, ")");
	stack_.pop_stack();
	expect(Types::Punctuation, "{");
	poliz_.push_address(p4);
	poliz_.push_poliz({ PolizType::COMMAND, "CREATE_TID" });
	State();
	MState();
	poliz_.push_poliz({ PolizType::COMMAND, "DELETE_TID" });
	poliz_.push_poliz({ PolizType::LITERAL, std::to_string(p5) });
	poliz_.push_poliz({ PolizType::COMMAND, "!" });
	expect(Types::Punctuation, "}");
	poliz_.push_poliz({ PolizType::COMMAND, "DELETE_TID" });
	poliz_.push_address(p3);
	tid_.delete_tid();
}

void Syntaxer::WhileState() {
	expect(Types::Keyword, "while");
	expect(Types::Punctuation, "(");
	int p1 = poliz_.get_gpt();
	Expr();
	expect(Types::Punctuation, ")");
	if (!stack_.check_if()) throw std::logic_error("expression must be bool type in while");
	int p2 = poliz_.get_gpt();
	poliz_.blank();
	poliz_.push_poliz({ PolizType::COMMAND, "F!" });
	poliz_.push_poliz({ PolizType::COMMAND, "CREATE_TID" });
	expect(Types::Punctuation, "{");
	tid_.create_tid();
	State();
	MState();

	poliz_.push_poliz({ PolizType::COMMAND, "DELETE_TID" });
	poliz_.push_poliz({ PolizType::LITERAL, std::to_string(p1) });
	poliz_.push_poliz({ PolizType::COMMAND, "!" });
	poliz_.push_address(p2);

	expect(Types::Punctuation, "}");
	tid_.delete_tid();
}

void Syntaxer::ReturnState() {
	info_func buff = tf_.v_[tf_.v_.size() - 1];
	expect(Types::Keyword, "return");
	if (!match(";")) {
		Expr();
		infoStack buff_st = stack_.pop_stack();
		if (check_return(buff.res.t_, buff_st.t_) and buff.res.d_ == buff_st.d_) {
			poliz_.push_poliz({ PolizType::COMMAND, "return" });
			return;
		} else if (buff.res.t_ == typefunc::Let) {
			tf_.v_[tf_.v_.size() - 1].res.t_ = to_ftype(buff_st.t_);
		} else throw std::logic_error("Mismatched return type in " + buff.name);
	} else if (buff.res.t_ == typefunc::Let) {
		tf_.v_[tf_.v_.size() - 1].res.t_ = typefunc::Void;
	} else if (buff.res.t_ != typefunc::Void) throw std::logic_error("func " + buff.name + " must return anything");
	poliz_.push_poliz({ PolizType::COMMAND, "return" });
}

void Syntaxer::Expr() {
	E1();
	while (match(",")) {
		stack_.pop_stack();
		NewToken();
		E1();
		poliz_.push_poliz({ PolizType::BIN_OPER, "," });
	}
}

void Syntaxer::E1() {
	E2();
	while (match("+=") or match("-=") or match("*=")
		or match("/=") or match("%=") or match("=")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E2();
		stack_.check_bin(poliz_);
	}
}

void Syntaxer::E2() {
	E3();
	while (match("or") or match("|")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E3();
		stack_.check_bin(poliz_);
	}
}

void Syntaxer::E3() {
	E4();
	while (match("and") or match("&")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E4();
		stack_.check_bin(poliz_);
	}
}

void Syntaxer::E4() {
	E5();
	while (match("<") or match(">") or match("==")
		or match("!=") or match("<=") or match(">=")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E5();
		stack_.check_bin(poliz_);
	}
}

void Syntaxer::E5() {
	E6();
	while (match("+") or match("-")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E6();
		stack_.check_bin(poliz_);
	}
}

void Syntaxer::E6() {
	E7();
	while (match("*") or match("/") or match("%")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E7();
		stack_.check_bin(poliz_);
	}
}

void Syntaxer::E7() {
	if (match("!") or match("not") or match("-")) {
		stack_.push_stack(curr_.value);
		NewToken();
		E7();
		stack_.check_uno(poliz_);
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
		poliz_.push_poliz({ PolizType::COMMAND, "BEGIN_LIST" });
		expr_list();
		poliz_.push_poliz({ PolizType::COMMAND, "END_LIST" });
		first = 1;
		stack_.push_stack(infoStack(listt, Types::Literal, depth_));
		depth_ = 0;
	} else if (match("size")) {
		Size();
	} else if (matchType(Types::Identificator)) {
		string call_n = curr_.value;
		NewToken();
		if (match("(")) {
			expect(Types::Punctuation, "(");
			vector<param> call_;
			if (!match(")")) {
				E1();
				auto buff = stack_.pop_stack();
				call_.push_back({ "", to_ftype(buff.t_), buff.d_ });
				while (match(",")) {
					NewToken();
					E1();
					buff = stack_.pop_stack();
					call_.push_back({ "", to_ftype(buff.t_), buff.d_ });
				}
			}
			expect(Types::Punctuation, ")");
			if (!tf_.check_call(call_n, call_)) throw std::logic_error("don't find func " + call_n + " with this params");
			auto res = tf_.call_res(call_n, call_);
			poliz_.push_poliz({ PolizType::ID, res.unic_name });
			poliz_.push_poliz({ PolizType::COMMAND, "CALL" });
			stack_.push_stack(res);
		} else {
			auto buff = tid_.check_exist_expr(call_n);
			poliz_.push_poliz({ PolizType::ID, call_n });
			if (!buff) throw std::logic_error(call_n + " not exist");
			int dpth = 0;
			while (match("[")) {
				++dpth;
				NewToken();
				Expr();
				if (stack_.pop_stack().t_ != typestack::Int) throw std::logic_error("in [] must be int");
				expect(Types::Operation, "]");
			}
			if (dpth > buff.value().d_) throw std::logic_error("[] for massive");
			for (int i = 0; i < dpth; ++i) {
				poliz_.push_poliz({ PolizType::BIN_OPER, "[]" });
			}
			stack_.push_stack(info(buff.value().t_, buff.value().d_ - dpth));
		}
	} else {
		expr_ = curr_.value;
		poliz_.push_poliz({ PolizType::LITERAL, expr_ });
		expectType(Types::Literal);
		stack_.push_stack(expr_);
	}
}

void Syntaxer::expr_list() {
	poliz_.push_poliz({ PolizType::BIN_OPER, "{" });
	expect(Types::Punctuation, "{");
	if (!match("}")) {
		depth_ = 1;
		E1();
		auto buff = stack_.pop_stack();
		listt = buff.t_;
		type_ = to_str(listt);
		while (match(",")) {
			NewToken();
			poliz_.push_poliz({ PolizType::BIN_OPER, "," });
			E1();
			auto buff = stack_.pop_stack();
			if (type_ == "int") {
				if (buff.t_ == typestack::Float) type_ = "float";
			}
			if (!(type_ == "float" and buff.t_ == typestack::Int) && buff.t_ != to_sttype(type_)) throw std::logic_error("wrong type in massive");
		}
	}
	expect(Types::Punctuation, "}");
}

void Syntaxer::init_list() {
	expect(Types::Punctuation, "{");
	if (!match("}")) {
		E1();
		auto buff = stack_.pop_stack();
		if (type_ == "let") {
			if (buff.t_ == typestack::Int) type_ = "int";
			else if (buff.t_ == typestack::Float) type_ = "float";
			else if (buff.t_ == typestack::Char) type_ = "char";
			else throw std::logic_error("wrong type in massive");
			depth_ = 1;
		}
		if (!(type_ == "float" and buff.t_ == typestack::Int) && buff.t_ != to_sttype(type_)) throw std::logic_error("wrong type in massive");
		while (match(",")) {
			NewToken();
			poliz_.push_poliz({ PolizType::BIN_OPER, "," });
			E1();
			auto buff = stack_.pop_stack();
			if (type_ == "int") {
				if (buff.t_ == typestack::Float) type_ = "float";
			}
			if (!(type_ == "float" and buff.t_ == typestack::Int) && buff.t_ != to_sttype(type_)) throw std::logic_error("wrong type in massive");
		}
	}
	expect(Types::Punctuation, "}");
}

void Syntaxer::cin() {
	expect(Types::Keyword, "input");
	expect(Types::Punctuation, "(");
	string name = curr_.value;
	poliz_.push_poliz({ PolizType::ID, name });
	expectType(Types::Identificator);
	auto buff = tid_.check_exist_expr(name);
	if (!buff) throw std::logic_error(name + " not exist");
	int dpth = 0;
	if (match("[")) {
		++dpth;
		NewToken();
		Expr();
		if (stack_.pop_stack().t_ != typestack::Int) throw std::logic_error("in [] must be num");
		expect(Types::Operation, "]");
	}
	if (dpth < buff.value().d_) throw std::logic_error("can't input massive");
	for (int i = 0; i < dpth; ++i) {
		poliz_.push_poliz({ PolizType::BIN_OPER, "[]" });
	}
	expect(Types::Punctuation, ")");
	poliz_.push_poliz({ PolizType::UNO_OPER, "input" });
}

void Syntaxer::cout() {
	expect(Types::Keyword, "output");
	expect(Types::Punctuation, "(");
	Expr();
	infoStack buff = stack_.pop_stack();
	if (buff.d_ > 0) throw std::logic_error("can't output massive");
	expect(Types::Punctuation, ")");
	poliz_.push_poliz({ PolizType::UNO_OPER, "output" });
}

void Syntaxer::Size() {
	expect(Types::Keyword, "size");
	expect(Types::Punctuation, "(");
	string name = curr_.value;
	poliz_.push_poliz({ PolizType::ID, name });
	expectType(Types::Identificator);
	auto buff = tid_.check_exist_expr(name);
	if (!buff) throw std::logic_error(name + " not exist");
	expect(Types::Punctuation, ")");
	stack_.push_stack(infoStack(typestack::Int, Types::Literal));
	poliz_.push_poliz({ PolizType::UNO_OPER, "size" });
}

typefunc Syntaxer::to_ftype(string type) {
	if (type == "int") return typefunc::Int;
	if (type == "float") return typefunc::Float;
	if (type == "char") return typefunc::Char;
	if (type == "void") return typefunc::Void;
	return typefunc::Let;
}

TypesId Syntaxer::to_idtype(string type) {
	if (type == "int") return TypesId::Int;
	if (type == "float") return TypesId::Float;
	if (type == "char") return TypesId::Char;
}

typefunc Syntaxer::to_ftype(TypesId type) {
	if (type == TypesId::Int) return typefunc::Int;
	if (type == TypesId::Float) return typefunc::Float;
	if (type == TypesId::Char) return typefunc::Char;
	return typefunc::Let;
}

TypesId Syntaxer::to_idtype(typefunc type) {
	if (type == typefunc::Int) return TypesId::Int;
	if (type == typefunc::Float) return TypesId::Float;
	if (type == typefunc::Char) return TypesId::Char;
}

void Syntaxer::check_init(string type1, typestack type2) {
	if (((type1 == "int" or type1 == "float") and (type2 == typestack::Int or type2 == typestack::Float)) or
		(type_ == "char" and type2 == typestack::Char) or type_ == "let" or
		((type1 == "int" or type1 == "char") and (type2 == typestack::Int or type2 == typestack::Char))) {
		return;
	} else {
		throw std::logic_error("types don't match");
	}
}

TypesId Syntaxer::to_idtype(typestack type) {
	if (type == typestack::Int) return TypesId::Int;
	if (type == typestack::Float) return TypesId::Float;
	if (type == typestack::Char) return TypesId::Char;
}

bool Syntaxer::check_return(typefunc type1, typestack type2) {
	if (((type2 == typestack::Int or type2 == typestack::Float) and
		(type1 == typefunc::Int or type1 == typefunc::Float)) or
		(type2 == typestack::Char and type1 == typefunc::Char) or
		((type2 == typestack::Int or type2 == typestack::Char) and
			(type1 == typefunc::Int or type1 == typefunc::Char))) return 1;
	else return 0;
}

typefunc Syntaxer::to_ftype(typestack type) {
	if (type == typestack::Int) return typefunc::Int;
	if (type == typestack::Float) return typefunc::Float;
	if (type == typestack::Void) return typefunc::Void;
	if (type == typestack::Char) return typefunc::Char;
}

typestack Syntaxer::to_sttype(string type) {
	if (type == "int") return typestack::Int;
	if (type == "float") return typestack::Float;
	if (type == "char") return typestack::Char;
}

string Syntaxer::to_str(typestack type) {
	if (type == typestack::Int) return "int";
	if (type == typestack::Float) return "float";
	if (type == typestack::Char) return "char";
}