#include "../lexicalAnalyzer/lexer.h"
#include "../semanticAnalyzer/stack.h"
#include "../semanticAnalyzer/tf.h"
#include "../semanticAnalyzer/tid.h"

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
	tree_tid tid_;
	tf tf_;
	type_stack stack_;
	string id_, type_, unic_, expr_;
	info_func func_;
	param param_;
	int depth_;
	Lexem curr_;
	void NewToken();
	void Prog();
	void Declarations();
	void Var();
	void Type();
	void Func();
	void TypeF();
	void Params();
	void FuncR();
	void State();
	void MState();
	void IfState();
	void ForState();
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
	void Iden();
	void list();
	void cin();
	void cout();
	typefunc to_ftype(string type);
	typefunc to_ftype(TypesId type);
	typefunc to_ftype(typestack type);
	typestack to_sttype(string type);
	TypesId to_idtype(string type);
	TypesId to_idtype(typefunc type);
	TypesId to_idtype(typestack type);
	void check_init(string type1, typestack type2);
	bool check_return(typefunc type1, typestack type2);
};