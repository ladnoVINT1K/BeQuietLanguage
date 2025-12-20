#pragma once
#include "../semanticAnalyzer/tid.cpp"
#include "poliz.h"
#include "../semanticAnalyzer/tf.cpp"
#include <stdexcept>
#include <exception>
#include <stack>
#include "../semanticAnalyzer/stack.cpp"

using std::to_string;

struct Call {
	tree_tid tid_;
	typefunc t_;
	int point;
};

class Interpreter {
public:
	Interpreter(Poliz& p, tf& tf);
	void runtime();
private:
	tf& tf_;
	Poliz& poliz_;
	int gpt_;
	string name_init;
	tree_tid global_tid;
	std::stack <Call> stack_call;
	std::stack <std::string> opers_;
	void uno_oper();
	void bin_oper();
	void do_oper(string x1, string x2, string op);
	string val_id(string name);
	string ind_val(string name, int i);
	void ind_change(string name, int i, string value);
	void change_val(string name, string val);
	int size(string name);
	string list();
	TypesId to_idtype(typefunc type);
};