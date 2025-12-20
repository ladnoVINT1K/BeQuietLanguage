#pragma once
#include <string>
#include <vector>
#include <iostream>

using std::vector;
using std::string;

enum class PolizType {
	COMMAND,
	ID,
	LITERAL,
	UNO_OPER,
	BIN_OPER
};

struct poliz_elem {
	PolizType t_;
	string n_;
};


class Poliz {
public:
	void push_poliz(poliz_elem lex);
	int blank();
	int get_gpt();
	void push_address(int p);
	string get_value(int p);
	PolizType get_type(int p);
	void print_poliz();
	int size();
private:
	vector<poliz_elem> p_;
	int gpt_ = 0;
};