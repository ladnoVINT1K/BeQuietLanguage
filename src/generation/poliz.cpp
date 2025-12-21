#pragma once
#include "poliz.h"

void Poliz::push_poliz(poliz_elem lex) {
	p_.push_back(lex);
	++gpt_;
}

int Poliz::blank() {
	p_.push_back({PolizType::LITERAL, ""});
	return gpt_++;
}

int Poliz::get_gpt() {
	return gpt_;
}

void Poliz::push_address(int p) {
	p_[p].n_ = std::to_string(gpt_);
}

string Poliz::get_value(int p) {
	return p_[p].n_;
}

PolizType Poliz::get_type(int p) {
	return p_[p].t_;
}

void Poliz::print_poliz() {
	for (int i = 0; i < p_.size(); ++i) {
		std::cout << p_[i].n_ << " ";
	}
	std::cout << '\n';
}

int Poliz::size() {
	return p_.size();
}