#pragma once
#include "interpreter.h"

Interpreter::Interpreter(Poliz& p, tf& tf) : tf_(tf), poliz_(p), gpt_(0) {}

void Interpreter::runtime() {
	for (; gpt_ < poliz_.size(); ++gpt_) {
		if (poliz_.get_value(gpt_) == "") return;
		if (poliz_.get_type(gpt_) == PolizType::LITERAL or poliz_.get_type(gpt_) == PolizType::ID) {
			opers_.push(poliz_.get_value(gpt_));
		} else if (poliz_.get_type(gpt_) == PolizType::COMMAND) {
			if (poliz_.get_value(gpt_) == "CREATE_TID") {
				if (stack_call.empty()) {
					stack_call.push({});
				}
				stack_call.top().tid_.create_tid();
			} else if (poliz_.get_value(gpt_) == "DELETE_TID") {
				stack_call.top().tid_.delete_tid();
			} else if (poliz_.get_value(gpt_) == "!") {
				gpt_ = (std::stoi(opers_.top())) - 1;
				opers_.pop();
			} else if (poliz_.get_value(gpt_) == "F!") {
				int ind = std::stoi(opers_.top());
				opers_.pop();
				if (opers_.top() == "false") {
					gpt_ = ind - 1;
				}
				opers_.pop();
			} else if (poliz_.get_value(gpt_) == "return") {
				string op = opers_.top();
				if (!((op[0] >= '0' and op[0] <= '9') or op[0] == '\'' or op[0] == '-')) {
					opers_.pop();
					if (op.find("[") != op.npos) {
						string name1 = op.substr(0, op.find("["));
						int j = std::stoi(op.substr(op.find("[") + 1, op.find("]") - op.find("[") - 1));
						op = ind_val(name1, j);
					} else {
						op = val_id(op);
					}
					opers_.push(op);
				}
				gpt_ = stack_call.top().point - 1;
				stack_call.pop();
				if (stack_call.empty()) return;
			} else if (poliz_.get_value(gpt_) == "CALL") {
				string name = opers_.top();
				opers_.pop();
				auto func = tf_.unic_call(name);
				vector <string> oper(func.params.size());
				for (int i = oper.size() - 1; i >= 0 ; --i) {
					string op = opers_.top();
					opers_.pop();
					if (op.find("[") != op.npos) {
						string name1 = op.substr(0, op.find("["));
						int j = std::stoi(op.substr(op.find("[") + 1, op.find("]") - op.find("[") - 1));
						oper[i] = ind_val(name, j);
					} else if ((op[0] >= 0 and op[0] <= '9') or op[0] == '\'') {
						oper[i] = op;
					} else {
						oper[i] = val_id(op);
					}
				}
				stack_call.push({});
				stack_call.top().point = gpt_ + 1;
				gpt_ = func.address - 1;
				stack_call.top().t_ = func.res.t_;
				stack_call.top().tid_.create_tid();
				int j = 0;
				for (auto i : func.params) {
					stack_call.top().tid_.push_id(i.name, info(to_idtype(i.t_), i.d_, oper[j]));
					++j;
				}
				stack_call.top().t_ = func.res.t_;
			} else if (poliz_.get_value(gpt_) == ";") {
				opers_.pop();
			} else if (poliz_.get_value(gpt_) == "INIT") {
				name_init = poliz_.get_value(++gpt_);
				if (poliz_.get_value(++gpt_)[0] == 'I') {
					TypesId type;
					string v;
					int d = 0;
					if (poliz_.get_value(gpt_)[5] == 'i') {
						type = TypesId::Int;
						v = "0";
						d = std::stoi(poliz_.get_value(gpt_).substr(9));
					} else if (poliz_.get_value(gpt_)[5] == 'c') {
						type = TypesId::Char;
						v = '\0';
						d = std::stoi(poliz_.get_value(gpt_).substr(10));
					} else {
						type = TypesId::Float;
						v = "0.0";
						d = std::stoi(poliz_.get_value(gpt_).substr(11));
					}
					if (stack_call.empty()) {
						global_tid.push_id(name_init, info(type, d, v) );
					} else {
						stack_call.top().tid_.push_id(name_init, info(type, d, v));
					}
				} else {
					--gpt_;
					opers_.push(name_init);
				}
			} else if (poliz_.get_value(gpt_)[0] == 'I') {
				TypesId type;
				string v = opers_.top(); opers_.pop();
				if (!((v[0] >= '0' and v[0] <= '9') or v[0] == '\'' or v[0] == '-')) {
					
					if (v.find("[") != v.npos) {
						string name1 = v.substr(0, v.find("["));
						int j = std::stoi(v.substr(v.find("[") + 1, v.find("]") - v.find("[") - 1));
						v = ind_val(name1, j);
					} else {
						v = val_id(v);
						
					}
				}
				name_init = opers_.top(); opers_.pop();
				int d = 0;
				if (poliz_.get_value(gpt_)[5] == 'i') {
					type = TypesId::Int;
					if (v.find(".") != v.npos) v = v.substr(0, v.find("."));
					else if (v[0] == '\'') {
						int c = v[1];
						v = to_string(c);
					}
					d = std::stoi(poliz_.get_value(gpt_).substr(9));
				} else if (poliz_.get_value(gpt_)[5] == 'c') {
					type = TypesId::Char;
					if (v[0] >= '0' and v[0] <= '9') {
						char c = stoi(v);
						v = to_string(c);
					}
					d = std::stoi(poliz_.get_value(gpt_).substr(10));
				} else {
					type = TypesId::Float;
					d = std::stoi(poliz_.get_value(gpt_).substr(11));
					if (v.find(".") == v.npos) v += ".0";
				}
				if (stack_call.empty()) {
					global_tid.push_id(name_init, info(type, d, v));
				} else {
					stack_call.top().tid_.push_id(name_init, info(type, d, v));
				}
			} else if (poliz_.get_value(gpt_) == "BEGIN_LIST") {
				++gpt_;
				opers_.push(list());
			}
		} else if (poliz_.get_type(gpt_) == PolizType::UNO_OPER) {
			uno_oper();
		} else {
			bin_oper();
		}
	}
	return;
}

void Interpreter::uno_oper() {
	string x = opers_.top();
	opers_.pop();
	if (poliz_.get_value(gpt_) == "size") {
		opers_.push(std::to_string(size(x)));
	} else if (poliz_.get_value(gpt_) == "input") {
		string input;
		std::cin >> input;
		if (x.find("[") != x.npos) {
			string name = x.substr(0, x.find("["));
			int i = std::stoi(x.substr(x.find("[") + 1, x.find("]") - x.find("[") - 1));
			ind_change(name, i, input);
		} else {
			change_val(x, input);
		}
	} else if (poliz_.get_value(gpt_) == "output") {
		if (x[0] == '\'') cout << x[1];
		else if (x[0] >= '0' and x[0] <= '9') cout << x;
		else if (x == "true" or x == "false") cout << x;
		else {
			if (x.find("[") != x.npos) {
				string name = x.substr(0, x.find("["));
				int i = std::stoi(x.substr(x.find("[") + 1, x.find("]") - x.find("[") - 1));
				cout << ind_val(name, i);
			} else {
				cout << val_id(x);
			}
		}
	} else if (poliz_.get_value(gpt_) == "!") {
		if (x == "true") {
			opers_.push("false");
		} else {
			opers_.push("true");
		}
	} else if (poliz_.get_value(gpt_) == "-") {
		if (x.find("[") != x.npos) {
			string name = x.substr(0, x.find("["));
			int i = std::stoi(x.substr(x.find("[") + 1, x.find("]") - x.find("[") - 1));
			x = ind_val(x, i);
		} else if ((x[0] >= 'a' and x[0] <= 'z') or x[0] == '_'){
			x = val_id(x);
		}
		if (x[0] >= '0' and x[0] <= '9') {
			x = '-' + x;
		} else {
			x = x.substr(1);
		}
		opers_.push(x);
	}
	return;
}

void Interpreter::bin_oper() {
	string x2 = opers_.top(); opers_.pop();
	string x1 = opers_.top(); opers_.pop();
	if (poliz_.get_value(gpt_) == ",") {
		opers_.push(x2);
		return;
	} else if (poliz_.get_value(gpt_) == "and") {
		if ((x1 == "true" and x2 == "true")) {
			opers_.push("true");
		} else {
			opers_.push("false");
		}
	} else if (poliz_.get_value(gpt_) == "or") {
		if ((x1 == "false" and x2 == "false")) {
			opers_.push("false");
		} else {
			opers_.push("true");
		}
	} else if (poliz_.get_value(gpt_) == "[]") {
		if (x2[0] == '-') throw std::runtime_error("index < 0");
		if (x2[0] >= '0' and x2[0] <= '9') opers_.push(x1 + "[" + x2 + "]");
		else if (x2.find("[") != x2.npos) {
			string name = x2.substr(0, x2.find("["));
			int i = std::stoi(x2.substr(x2.find("[") + 1, x2.find("]") - x2.find("[") - 1));
			opers_.push(x1 + "[" + ind_val(name, i) + "]");
		} else {
			opers_.push(x1 + "[" + val_id(x2) + "]");
		}
	} else {
		do_oper(x1, x2, poliz_.get_value(gpt_));
	}
	return;
}

string Interpreter::list() {
	string res = "";
	while (poliz_.get_value(gpt_) != "END_LIST") {
		if (poliz_.get_type(gpt_) == PolizType::LITERAL or poliz_.get_type(gpt_) == PolizType::ID) {
			opers_.push(poliz_.get_value(gpt_));
		} else if (poliz_.get_type(gpt_) == PolizType::UNO_OPER) {
			uno_oper();
		} else if (poliz_.get_value(gpt_) == ",") {
			string op = opers_.top();
			if (op[0] == '\'' or (op[0] >= '0' and op[0] <= '9')) {
				res += op + ",";
			} else if (op.find("[") != op.npos) {
				string name = op.substr(0, op.find("["));
				int i = std::stoi(op.substr(op.find("[") + 1, op.find("]") - op.find("[") - 1));
				res += ind_val(name, i);
			} else {
				res += val_id(op) + ",";
			}
			opers_.pop();
		} else if (poliz_.get_type(gpt_) == PolizType::UNO_OPER){
			bin_oper();
		} 
		++gpt_;
		if (poliz_.get_value(gpt_) == "END_LIST") {
			res += opers_.top();
			opers_.pop();
			break;
		}
	}
	return res;
}

string Interpreter::val_id(string name) {
	if (stack_call.empty()) {
		auto x2 = global_tid.check_exist_expr(name);
		return x2.value().v_;
	}
	auto x1 = stack_call.top().tid_.check_exist_expr(name);
	string res = "";
	if (!x1) {
		auto x2 = global_tid.check_exist_expr(name);
		return x2.value().v_;
	} else { 
		return x1.value().v_;
	}
}

string Interpreter::ind_val(string name, int i) {
	if (i < 0) throw std::runtime_error("index < 0");
	string res = "";
	if (stack_call.empty()) {
		int cnt = i, j = 0;
		auto s = global_tid.check_exist_expr(name);
		while (j < 9 and cnt > 0) {
			if (s.value().v_[j] == ',') {
				--cnt;
			}
			++j;
		}
		if (cnt > 0) throw std::runtime_error("index out of range");
		while (j < s.value().v_.size() and s.value().v_[j] != ',') {
			res += s.value().v_[j];
			++j;
		}
		return res;
	}
	auto x1 = stack_call.top().tid_.check_exist_expr(name);
	if (!x1) {
		int cnt = i, j = 0;
		auto s = global_tid.check_exist_expr(name);
		while (j < s.value().v_.size() and cnt > 0) {
			if (s.value().v_[j] == ',') {
				--cnt;
			}
			++j;
		}
		if (cnt > 0) throw std::runtime_error("index out of range");
		while (j < s.value().v_.size() and s.value().v_[j] != ',') {
			res += s.value().v_[j];
			++j;
		}
	} else {
		int cnt = i, j = 0;
		while (j < x1.value().v_.size() and cnt > 0) {
			if (x1.value().v_[j] == ',') {
				--cnt;
			}
			++j;
		}
		if (cnt > 0) throw std::runtime_error("index out of range");
		while (j < x1.value().v_.size() and x1.value().v_[j] != ',') {
			res += x1.value().v_[j];
			++j;
		}
	}
	return res;
}

void Interpreter::ind_change(string name, int i, string value) {
	if (i < 0) throw std::runtime_error("index < 0");
	if (stack_call.empty()) {
		int cnt = i, j = 0;
		auto s = global_tid.check_exist_expr(name);
		while (j < s.value().v_.size() and cnt > 0) {
			if (s.value().v_[j] == ',') {
				--cnt;
			}
			++j;
		}
		if (cnt > 0) throw std::runtime_error("index out of range");
		int k = j;
		while (k < s.value().v_.size() and s.value().v_[k] != ',') {
			++k;
		}
		string res = s.value().v_.substr(0, j) + value + s.value().v_.substr(0);
		global_tid.change_val(name, res);
		return;
	}
	auto x1 = stack_call.top().tid_.check_exist_expr(name);
	if (!x1) {
		int cnt = i, j = 0;
		auto s = global_tid.check_exist_expr(name);
		while (j < s.value().v_.size() and cnt > 0) {
			if (s.value().v_[j] == ',') {
				--cnt;
			}
			++j;
		}
		if (cnt > 0) throw std::runtime_error("index out of range");
		int k = j;
		while (k < s.value().v_.size() and s.value().v_[k] != ',') {
			++k;
		}
		string res = s.value().v_.substr(0, j) + value + s.value().v_.substr(0);
		global_tid.change_val(name, res);
		return;
	} else {
		int cnt = i, j = 0;
		while (j < x1.value().v_.size() and cnt > 0) {
			if (x1.value().v_[j] == ',') {
				--cnt;
			}
			++j;
		}
		if (cnt > 0) throw std::runtime_error("index out of range");
		int k = j;
		while (k < x1.value().v_.size() and x1.value().v_[k] != ',') {
			++k;
		}
		string res = x1.value().v_.substr(0, j) + value + x1.value().v_.substr(k);
		stack_call.top().tid_.change_val(name, res);
		return;
	}
}

void Interpreter::change_val(string name, string value) {
	if (stack_call.empty()) {
		global_tid.change_val(name, value);
		return;
	}
	auto x = stack_call.top().tid_.check_exist_expr(name);
	if (!x) {
		global_tid.change_val(name, value);
		return;
	} else {
		stack_call.top().tid_.change_val(name, value);
	}
}

int Interpreter::size(string name) {
	if (stack_call.empty()) {
		string s = global_tid.check_exist_expr(name).value().v_;
		int cnt = 0;
		for (int i = 0; i < s.size(); ++i) {
			if (s[i] == ',') ++cnt;
		}
		return cnt + 1;
	}
	auto x = stack_call.top().tid_.check_exist_expr(name);
	if (!x) {
		string s = global_tid.check_exist_expr(name).value().v_;
		int cnt = 0;
		for (int i = 0; i < s.size(); ++i) {
			if (s[i] == ',') ++cnt;
		}
		return cnt + 1;
	} else {
		string s = x.value().v_;
		int cnt = 0;
		for (int i = 0; i < s.size(); ++i) {
			if (s[i] == ',') ++cnt;
		}
		return cnt + 1;
	}
}

void Interpreter::do_oper(string x1, string x2, string op) {
	string res;
	if ((x1 == "true" and x2 == "true") or (x1 == "false" and x2 == "false")) {
		if (op == "==") res = "true";
		else if (op == "!=") res = "false";
	} else if ((x1 == "true" and x2 == "false") or (x1 == "false" and x2 == "true")) {
		if (op == "==") res = "false";
		else if (op == "!=") res = "true";
	} else if (((x1[0] >= '0' and x1[0] <= '9') or x1[0] == '-') and x1.find(".") != x1.npos) {
		double op1 = stod(x1);
		if (((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') and x2.find(".") != x2.npos) {
			double op2 = stod(x2);
			if (op == "+") {
				res = std::to_string(op2 + op1);
			} else if (op == "-") {
				res = std::to_string(op1 - op2);
			} else if (op == "*") {
				res = std::to_string(op1 * op2);
			} else if (op == "/") {
				if (op2 == 0) throw std::runtime_error("do not divide by 0, please");
				res = std::to_string(op1 / op2);
			} else if (op == ">") {
				if (op1 > op2) res = "true";
				else res = "false";
			} else if (op == "<") {
				if (op1 < op2) res = "true";
				else res = "false";
			} else if (op == "!=") {
				if (op1 != op2) res = "true";
				else res = "false";
			} else if (op == "==") {
				if (op1 == op2) res = "true";
				else res = "false";
			} else if (op == "<=") {
				if (op1 <= op2) res = "true";
				else res = "false";
			} else if (op == ">=") {
				if (op1 >= op2) res = "true";
				else res = "false";
			}
		} else if ((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-' ) {
			int op2 = stoi(x2);
			if (op == "+") {
				res = std::to_string(op2 + op1);
			} else if (op == "-") {
				res = std::to_string(op1 - op2);
			} else if (op == "*") {
				res = std::to_string(op1 * op2);
			} else if (op == "/") {
				if (op2 == 0) throw std::runtime_error("do not divide by 0, please");
				res = std::to_string(op1 / op2);
			} else if (op == ">") {
				if (op1 > op2) res = "true";
				else res = "false";
			} else if (op == "<") {
				if (op1 < op2) res = "true";
				else res = "false";
			} else if (op == "!=") {
				if (op1 != op2) res = "true";
				else res = "false";
			} else if (op == "==") {
				if (op1 == op2) res = "true";
				else res = "false";
			} else if (op == "<=") {
				if (op1 <= op2) res = "true";
				else res = "false";
			} else if (op == ">=") {
				if (op1 >= op2) res = "true";
				else res = "false";
			}
		} else if (x2.find("[") != x2.npos) {
			string name1 = x2.substr(0, x2.find("["));
			int i = std::stoi(x2.substr(x2.find("[") + 1, x2.find("]") - x2.find("[") - 1));
			x2 = ind_val(name1, i);
			do_oper(x1, x2, op);
			return;
		} else {
			x2 = val_id(x2);
			do_oper(x1, x2, op);
			return;
		}
	} else if ((x1[0] >= '0' and x1[0] <= '9') or x1[0] == '-') {
		int op1 = stoi(x1);
		if (((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') and x2.find(".") != x2.npos) {
			double op2 = stod(x2);
			if (op == "+") {
				res = std::to_string(op2 + op1);
			} else if (op == "-") {
				res = std::to_string(op1 - op2);
			} else if (op == "*") {
				res = std::to_string(op1 * op2);
			} else if (op == "/") {
				if (op2 == 0) throw std::runtime_error("do not divide by 0, please");
				res = std::to_string(op1 / op2);
			} else if (op == ">") {
				if (op1 > op2) res = "true";
				else res = "false";
			} else if (op == "<") {
				if (op1 < op2) res = "true";
				else res = "false";
			} else if (op == "!=") {
				if (op1 != op2) res = "true";
				else res = "false";
			} else if (op == "==") {
				if (op1 == op2) res = "true";
				else res = "false";
			} else if (op == "<=") {
				if (op1 <= op2) res = "true";
				else res = "false";
			} else if (op == ">=") {
				if (op1 >= op2) res = "true";
				else res = "false";
			}
		} else if ((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') {
			int op2 = stoi(x2);
			if (op == "+") {
				res = std::to_string(op2 + op1);
			} else if (op == "-") {
				res = std::to_string(op1 - op2);
			} else if (op == "*") {
				res = std::to_string(op1 * op2);
			} else if (op == "/") {
				if (op2 == 0) throw std::runtime_error("do not divide by 0, please");
				res = std::to_string(op1 / op2);
			} else if (op == ">") {
				if (op1 > op2) res = "true";
				else res = "false";
			} else if (op == "<") {
				if (op1 < op2) res = "true";
				else res = "false";
			} else if (op == "!=") {
				if (op1 != op2) res = "true";
				else res = "false";
			} else if (op == "==") {
				if (op1 == op2) res = "true";
				else res = "false";
			} else if (op == "<=") {
				if (op1 <= op2) res = "true";
				else res = "false";
			} else if (op == ">=") {
				if (op1 >= op2) res = "true";
				else res = "false";
			} else if (op == "%") {
				res = std::to_string(op1 % op2);
			}
		} else if (x2[0] == '\'') {
			char op2 = x2[1];
			if (op == "+") {
				res = std::to_string(op2 + op1);
			} else if (op == "-") {
				res = std::to_string(op1 - op2);
			} else if (op == ">") {
				if (op1 > op2) res = "true";
				else res = "false";
			} else if (op == "<") {
				if (op1 < op2) res = "true";
				else res = "false";
			} else if (op == "!=") {
				if (op1 != op2) res = "true";
				else res = "false";
			} else if (op == "==") {
				if (op1 == op2) res = "true";
				else res = "false";
			} else if (op == "<=") {
				if (op1 <= op2) res = "true";
				else res = "false";
			} else if (op == ">=") {
				if (op1 >= op2) res = "true";
				else res = "false";
			}
		} else if (x2.find("[") != x2.npos) {
			string name1 = x2.substr(0, x2.find("["));
			int i = std::stoi(x2.substr(x2.find("[") + 1, x2.find("]") - x2.find("[") - 1));
			x2 = ind_val(name1, i);
			do_oper(x1, x2, op);
			return;
		} else {
			x2 = val_id(x2);
			do_oper(x1, x2, op);
			return;
		}
	} else if (x1[0] == '\'') {
		char op1 = x1[1];
		if ((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') {
			int op2 = stoi(x2);
			if (op == "+") {
				res = std::to_string(op2 + op1);
			} else if (op == "-") {
				res = std::to_string(op1 - op2);
			} else if (op == ">") {
				if (op1 > op2) res = "true";
				else res = "false";
			} else if (op == "<") {
				if (op1 < op2) res = "true";
				else res = "false";
			} else if (op == "!=") {
				if (op1 != op2) res = "true";
				else res = "false";
			} else if (op == "==") {
				if (op1 == op2) res = "true";
				else res = "false";
			} else if (op == "<=") {
				if (op1 <= op2) res = "true";
				else res = "false";
			} else if (op == ">=") {
				if (op1 >= op2) res = "true";
				else res = "false";
			}
		} else if (x2[0] == '\'') {
			char op2 = x2[1];
			if (op == "+") {
				res = std::to_string(op2 + op1);
			} else if (op == "-") {
				res = std::to_string(op1 - op2);
			} else if (op == ">") {
				if (op1 > op2) res = "true";
				else res = "false";
			} else if (op == "<") {
				if (op1 < op2) res = "true";
				else res = "false";
			} else if (op == "!=") {
				if (op1 != op2) res = "true";
				else res = "false";
			} else if (op == "==") {
				if (op1 == op2) res = "true";
				else res = "false";
			} else if (op == "<=") {
				if (op1 <= op2) res = "true";
				else res = "false";
			} else if (op == ">=") {
				if (op1 >= op2) res = "true";
				else res = "false";
			}
		} else if (x2.find("[") != x2.npos) {
			string name1 = x2.substr(0, x2.find("["));
			int i = std::stoi(x2.substr(x2.find("[") + 1, x2.find("]") - x2.find("[") - 1));
			x2 = ind_val(name1, i);
			do_oper(x1, x2, op);
			return;
		} else {
			x2 = val_id(x2);
			do_oper(x1, x2, op);
			return;
		}
	} else if (x1.find("[") != x1.npos) {
		string ref = x1;
		string name = x1.substr(0, x1.find("["));
		int i = std::stoi(x1.substr(x1.find("[") + 1, x1.find("]") - x1.find("[") - 1));
		x1 = ind_val(name, i);
		if ((((x1[0] >= '0' and x1[0] <= '9') or x1[0] == '-') and x1.find(".") != x1.npos)) {
			double op1 = stod(x1);
			if (((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') and x2.find(".") != x2.npos) {
				double op2 = stod(x2);
				if (op == "=") {
					ind_change(name, i, x2);
				} else if (op == "+=") {
					ind_change(name, i, to_string(op1 + op2));
				} else if (op == "-=") {
					ind_change(name, i, to_string(op1 - op2));
				} else if (op == "*=") {
					ind_change(name, i, to_string(op1 * op2));
				} else if (op == "/=") {
					ind_change(name, i, to_string(op1 / op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if ((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') {
				int op2 = stoi(x2);
				if (op == "=") {
					ind_change(name, i, x2 + ".0");
				} else if (op == "+=") {
					ind_change(name, i, to_string(op1 + op2));
				} else if (op == "-=") {
					ind_change(name, i, to_string(op1 - op2));
				} else if (op == "*=") {
					ind_change(name, i, to_string(op1 * op2));
				} else if (op == "/=") {
					ind_change(name, i, to_string(op1 / op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if (x2.find("[") != x2.npos) {
				string name1 = x2.substr(0, x2.find("["));
				int i = std::stoi(x2.substr(x2.find("[") + 1, x2.find("]") - x2.find("[") - 1));
				x2 = ind_val(name1, i);
				do_oper(ref, x2, op);
				return;
			} else {
				x2 = val_id(x2);
				do_oper(ref, x2, op);
				return;
			}
		} else if ((x1[0] >= '0' and x1[0] <= '9') or x1[0] == '-') {
			int op1 = stoi(x1);
			if (((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') and x2.find(".") != x2.npos) {
				double op2 = stod(x2);
				if (op == "=") {
					ind_change(name, i, x2.substr(0, x2.find(".")));
				} else if (op == "+=") {
					string f = to_string(op1 + op2);
					ind_change(name, i, f.substr(0, f.find(".")));
				} else if (op == "-=") {
					string f = to_string(op1 - op2);
					ind_change(name, i, f.substr(0, f.find(".")));
				} else if (op == "*=") {
					string f = to_string(op1 * op2);
					ind_change(name, i, f.substr(0, f.find(".")));
				} else if (op == "/=") {
					string f = to_string(op1 / op2);
					ind_change(name, i, f.substr(0, f.find(".")));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if ((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') {
				int op2 = stoi(x2);
				if (op == "=") {
					ind_change(name, i, x2);
				} else if (op == "+=") {
					ind_change(name, i, to_string(op1 + op2));
				} else if (op == "-=") {
					ind_change(name, i, to_string(op1 - op2));
				} else if (op == "*=") {
					ind_change(name, i, to_string(op1 * op2));
				} else if (op == "/=") {
					ind_change(name, i, to_string(op1 / op2));
				} else if (op == "%=") {
					ind_change(name, i, to_string(op1 % op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if (x2[0] == '\'') {
				char op2 = x2[1];
				if (op == "=") {
					ind_change(name, i, x2);
				} else if (op == "+=") {
					ind_change(name, i, to_string(op1 + op2));
				} else if (op == "-=") {
					ind_change(name, i, to_string(op1 - op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if (x2.find("[") != x2.npos) {
				string name1 = x2.substr(0, x2.find("["));
				int i = std::stoi(x2.substr(x2.find("[") + 1, x2.find("]") - x2.find("[") - 1));
				x2 = ind_val(name1, i);
				do_oper(ref, x2, op);
				return;
			} else {
				x2 = val_id(x2);
				do_oper(ref, x2, op);
				return;
			}
		} else if (x1[0] == '\'') {
			char op1 = x1[1];
			if ((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-' ) {
				if (op == "=") {
					char op2 = stoi(x2);
					ind_change(name, i, x2);
				} else if (op == "+=") {
					char op2 = stoi(x2) + op1;
					ind_change(name, i, to_string(op2));
				} else if (op == "-=") {
					char op2 = op1 - stoi(x2);
					ind_change(name, i, to_string(op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if (x2[0] == '\'') {
				if (op == "=") {
					ind_change(name, i, x2);
				} else if (op == "+=") {
					char op2 = x2[1] + op1;
					ind_change(name, i, to_string(op2));
				} else if (op == "-=") {
					char op2 = op1 - x2[1];
					ind_change(name, i, to_string(op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if (x2.find("[") != x2.npos) {
				string name1 = x2.substr(0, x2.find("["));
				int i = std::stoi(x2.substr(x2.find("[") + 1, x2.find("]") - x2.find("[") - 1));
				x2 = ind_val(name1, i);
				do_oper(ref, x2, op);
				return;
			} else {
				x2 = val_id(x2);
				do_oper(ref, x2, op);
				return;
			}
		}
		res = ref;
	} else {
		string name = x1;
		x1 = val_id(x1);
		if ((((x1[0] >= '0' and x1[0] <= '9') or x1[0] == '-') and x1.find(".") != x1.npos)) {
			double op1 = stod(x1);
			if (((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') and x2.find(".") != x2.npos) {
				double op2 = stod(x2);
				if (op == "=") {
					change_val(name, x2);
				} else if (op == "+=") {
					change_val(name, to_string(op1 + op2));
				} else if (op == "-=") {
					change_val(name, to_string(op1 - op2));
				} else if (op == "*=") {
					change_val(name, to_string(op1 * op2));
				} else if (op == "/=") {
					change_val(name, to_string(op1 / op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if ((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') {
				int op2 = stoi(x2);
				if (op == "=") {
					change_val(name, x2 + ".0");
				} else if (op == "+=") {
					change_val(name, to_string(op1 + op2));
				} else if (op == "-=") {
					change_val(name, to_string(op1 - op2));
				} else if (op == "*=") {
					change_val(name, to_string(op1 * op2));
				} else if (op == "/=") {
					change_val(name, to_string(op1 / op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if (x2.find("[") != x2.npos) {
				string name = x2.substr(0, x2.find("["));
				int i = std::stoi(x2.substr(x2.find("[") + 1, x2.find("]") - x2.find("[") - 1));
				x2 = ind_val(name, i);
				do_oper(name, x2, op);
				return;
			} else {
				x2 = val_id(x2);
				do_oper(name, x2, op);
				return;
			}
		} else if ((x1[0] >= '0' and x1[0] <= '9') or x1[0] == '-') {
			int op1 = stoi(x1);
			if (((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') and x2.find(".") != x2.npos) {
				double op2 = stod(x2);
				if (op == "=") {
					change_val(name, x2.substr(0, x2.find(".")));
				} else if (op == "+=") {
					string f = to_string(op1 + op2);
					change_val(name, f.substr(0, f.find(".")));
				} else if (op == "-=") {
					string f = to_string(op1 - op2);
					change_val(name, f.substr(0, f.find(".")));
				} else if (op == "*=") {
					string f = to_string(op1 * op2);
					change_val(name, f.substr(0, f.find(".")));
				} else if (op == "/=") {
					string f = to_string(op1 / op2);
					change_val(name, f.substr(0, f.find(".")));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if ((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-') {
				int op2 = stoi(x2);
				if (op == "=") {
					change_val(name, x2);
				} else if (op == "+=") {
					change_val(name, to_string(op1 + op2));
				} else if (op == "-=") {
					change_val(name, to_string(op1 - op2));
				} else if (op == "*=") {
					change_val(name, to_string(op1 * op2));
				} else if (op == "/=") {
					change_val(name, to_string(op1 / op2));
				} else if (op == "%=") {
					change_val(name, to_string(op1 % op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if (x2[0] == '\'') {
				int op2 = x2[1];
				if (op == "=") {
					change_val(name, x2);
				} else if (op == "+=") {
					change_val(name, to_string(op1 + op2));
				} else if (op == "-=") {
					change_val(name, to_string(op1 - op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if (x2.find("[") != x2.npos) {
				string name1 = x2.substr(0, x2.find("["));
				int i = std::stoi(x2.substr(x2.find("[") + 1, x2.find("]") - x2.find("[") - 1));
				x2 = ind_val(name1, i);
				do_oper(name, x2, op);
				return;
			} else {
				x2 = val_id(x2);
				do_oper(name, x2, op);
				return;
			}
		} else if (x1[0] == '\'') {
			char op1 = x1[1];
			if ((x2[0] >= '0' and x2[0] <= '9') or x2[0] == '-' ) {
				if (op == "=") {
					char op2 = stoi(x2);
					change_val(name, x2);
				} else if (op == "+=") {
					char op2 = stoi(x2) + op1;
					change_val(name, to_string(op2));
				} else if (op == "-=") {
					char op2 = op1 - stoi(x2);
					change_val(name, to_string(op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if (x2[0] == '\'') {
				if (op == "=") {
					change_val(name, x2);
				} else if (op == "+=") {
					char op2 = x2[1] + op1;
					change_val(name, to_string(op2));
				} else if (op == "-=") {
					char op2 = op1 - x2[1];
					change_val(name, to_string(op2));
				} else {
					do_oper(x1, x2, op);
					return;
				}
			} else if (x2.find("[") != x2.npos) {
				string name1 = x2.substr(0, x2.find("["));
				int i = std::stoi(x2.substr(x2.find("[") + 1, x2.find("]") - x2.find("[") - 1));
				x2 = ind_val(name1, i);
				do_oper(name, x2, op);
				return;
			} else {
				x2 = val_id(x2);
				do_oper(name, x2, op);
				return;
			}
		}
		res = name;
	}
	opers_.push(res);
}

TypesId Interpreter::to_idtype(typefunc type) {
	if (type == typefunc::Int) return TypesId::Int;
	if (type == typefunc::Float) return TypesId::Float;
	if (type == typefunc::Char) return TypesId::Char;
}