#pragma once
#include "stack.h"
#include <iostream>

type_stack::type_stack() {
	Operators_ = {};
	Operations_ = {};
}

void type_stack::push_stack(string lex, int d) {
	if (find(simple_oper.begin(), simple_oper.end(), lex) != simple_oper.end() or
		find(compound_oper.begin(), compound_oper.end(), lex) != compound_oper.end() or lex == "and" or lex == "or") {
		Operations_.push_front(lex);
	} else {
		typestack t;
		string v = lex;
		std::regex n("^\\d+(\\.\\d+)?$");
		if (std::regex_match(lex, n) and lex.find(".") != string::npos) {
			t = typestack::Float;
		} else if (std::regex_match(lex, n)) {
			t = typestack::Int;
		} else {
			if (lex == "true" or lex == "false") {
				t = typestack::Bool;
			} else {
				t = typestack::Char;
				Operators_.push_front(infoStack(t, Types::Literal, 1, v));
				return;
			}
		}
		Operators_.push_front(infoStack(t, Types::Literal, d, v));
		return;
	}
}

void type_stack::push_stack(info i) {
	Types cat = Types::Identificator;
	if (i.t_ == TypesId::Float) {
		Operators_.push_front(infoStack(typestack::Float, cat, i.d_, i.v_));
	} else if (i.t_ == TypesId::Int) {
		Operators_.push_front(infoStack(typestack::Int, cat, i.d_, i.v_));
	} else {
		Operators_.push_front(infoStack(typestack::Char, cat, i.d_, i.v_));
	}
	return;
}

void type_stack::push_stack(info_func i) {
	Types cat = Types::Literal;
	if (i.res.t_ == typefunc::Float) {
		Operators_.push_front(infoStack(typestack::Float, cat, i.res.d_));
	} else if (i.res.t_ == typefunc::Int) {
		Operators_.push_front(infoStack(typestack::Int, cat, i.res.d_));
	} else if (i.res.t_ == typefunc::Char) {
		Operators_.push_front(infoStack(typestack::Char, cat, i.res.d_));
	} else if (i.res.t_ == typefunc::Void) {
		Operators_.push_front(infoStack(typestack::Void, cat, i.res.d_));
	}
}

void type_stack::push_stack(infoStack i) {
	Operators_.push_front(i);
	return;
}

void type_stack::check_uno() {
	infoStack c = Operators_.front(); Operators_.pop_front();
	string op = Operations_.front(); Operations_.pop_front();

	if (c.d_ != 0) throw std::runtime_error("can't use uno oper for massive");

	if (op == "-" and (c.t_ == typestack::Int or c.t_ == typestack::Float)) {
		push_stack(c);
		return;
	} else if (op == "!" and c.t_ == typestack::Bool) {
		push_stack(c);
		return;
	} else {
		throw std::runtime_error("error in uno operation");
	}
}

void type_stack::check_bin() {
	infoStack c1 = Operators_.front(); Operators_.pop_front();
	infoStack c2 = Operators_.front(); Operators_.pop_front();
	string op = Operations_.front(); Operations_.pop_front();
	if (op == "=" and c2.t_ != typestack::Int and c2.t_ != typestack::Float) {
		if (c2.l_ == Types::Identificator && c1.d_ == c2.d_ and c1.t_ == c2.t_) c2 = c1;
		else throw std::runtime_error("error in assignment");
		push_stack(c2);
		return;
	}
	if (c1.d_ != 0 or c2.d_ != 0) throw std::runtime_error("can't use bin oper for massive");
	if ((op == "and" || op == "or") && c1.t_ == typestack::Bool && c2.t_ == typestack::Bool) {
		infoStack res(typestack::Bool, Types::Literal, 0);
		push_stack(res);
		return;
	} else if ((op == "==" || op == "!=") and ((c1.t_ == typestack::Bool && c2.t_ == typestack::Bool) or
		((c1.t_ == typestack::Int or c1.t_ == typestack::Char) && (c2.t_ == typestack::Char or c2.t_ == typestack::Int)) or
		((c1.t_ == typestack::Int or c1.t_ == typestack::Float) && (c2.t_ == typestack::Int or c2.t_ == typestack::Float)))) {
		infoStack res(typestack::Bool, Types::Literal, 0);
		push_stack(res);
		return;
	} else if ((c1.t_ == typestack::Int or c1.t_ == typestack::Char) && (c2.t_ == typestack::Char or c2.t_ == typestack::Int)
		&& (op == "+" || op == "-")) {
		infoStack res(typestack::Int);
		push_stack(res);
		return;
	} else if ((c1.t_ == typestack::Int or c1.t_ == typestack::Char) &&
		(c2.t_ == typestack::Int or c2.t_ == typestack::Char) &&
		(op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=")) {
		infoStack res(typestack::Bool, Types::Literal, 0);
		push_stack(res);
		return;
	} else if ((c1.t_ == typestack::Int or c1.t_ == typestack::Float) &&
		(c2.t_ == typestack::Int or c2.t_ == typestack::Float) &&
		(op == "==" || op == "!=" || op == "<" || op == ">" || op == "<=" || op == ">=")) {
		infoStack res(typestack::Bool, Types::Literal, 0);
		push_stack(res);
		return;
	} else if (c1.t_ == typestack::Int && c2.t_ == typestack::Int &&
		(op == "+" || op == "-" || op == "*" || op == "/" || op == "%")) {
		infoStack res(typestack::Int, Types::Literal, 0);
		push_stack(res);
		return;
	} else if ((c1.t_ == typestack::Int or c1.t_ == typestack::Float) &&
		(c2.t_ == typestack::Int or c2.t_ == typestack::Float) &&
		(op == "+" || op == "-" || op == "*" || op == "/" || op == "%")) {
		infoStack res(typestack::Float, Types::Literal, 0);
		push_stack(res);
		return;
	} else if ((c1.t_ == typestack::Int or c1.t_ == typestack::Float) &&
		(c2.t_ == typestack::Int or c2.t_ == typestack::Float)
		&& c2.l_ == Types::Identificator && (op == "+=" || op == "-=" ||
			op == "*=" || op == "/=" || op == "%=" || op == "=")) {
		infoStack res(c2.t_, Types::Identificator, 0);
		push_stack(res);
	} else if ((c1.t_ == typestack::Int or c1.t_ == typestack::Char) &&
		(c2.t_ == typestack::Int or c2.t_ == typestack::Char)
		&& c2.l_ == Types::Identificator && (op == "+=" || op == "-=")) {
		infoStack res(c2.t_, Types::Identificator, 0);
		push_stack(res);
	} else if (c1.t_ != c2.t_) throw std::runtime_error("can't complicit two different types");
	else throw std::runtime_error("error in check_bin!");
}

bool type_stack::check_if() {
	infoStack c = Operators_.front(); Operators_.pop_front();
	if ((c.t_ == typestack::Bool or c.t_ == typestack::Int or c.t_ == typestack::Float) and c.d_ == 0) return true;
	return false;
}

infoStack type_stack::pop_stack() {
	infoStack c = Operators_.front(); Operators_.pop_front();
	return c;
}

typestack type_stack::to_stackt(TypesId type) {
	if (type == TypesId::Int) return typestack::Int;
	if (type == TypesId::Float) return typestack::Float;
	if (type == TypesId::Char) return typestack::Char;
}