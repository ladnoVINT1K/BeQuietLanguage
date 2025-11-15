#include "../lexicalAnalyzer/lexer.h"

class Syntaxer {
public:
	Syntaxer(Lexer& lexer);
	bool syntax();
	bool match(const std::string& value);
	void expect(Types type, const std::string& value);
	void expectType(Types type);
	bool matchType(Types type);
private:
	Lexer& lexer_;
	Lexem curr_;
	void NewToken();
	void Prog();
	void Declarations();
	void Declaration();
	void Var();
	void Type();
	void Init();
	void Func();
	void TypeF();
	void Params();
	void Param();
	void FuncR();
	void State();
	void MState();
	void IfState();
	void IfStateTail();
	void ForState();
	void ForStateTail();
	void A();
	void WhileState();
	void ReturnState();
	void Expr();
	void E1();
	void E2();
	void E3();
	void E4();
	void E5();
	void E6();
	void E7();
	void E8();
	void CI();
	void Call();
	void CallTail();
	void Args();
	void Iden();
	void list();
	void cin();
	void cout();
};