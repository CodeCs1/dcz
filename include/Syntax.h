#pragma once

#include <Token.h>
#include <string>
#include <vector>
#include <list>
#include <memory>

#include <config.h>
#ifdef COMPILER
#include <codegen/Result_MC.h>
#endif

class Result_MC;
class Function_MC;

enum ParserType {
    EXPR=-1,
    LITERAL=1,
    GROUPING=2,
    BINARY=3,
    UNARY=4,
    STATEMENT=5,
    VARIABLE=6,
    VAR=7,
    ASSIGN=8,
    SCOPE=9,
    IFSTATEMENT=10,
    WHILELOOP=11,
    CALL=12,
    FUNC=13,
    RETURN=14,
    LIST=15,
    LISTINDEX=16
};

#define SExpr std::shared_ptr<Expr>

#define SPtr(c) std::shared_ptr<c>

class Expr {
    public:
        Expr& operator=(const Expr& expr) {
            return *this;
        }    
        virtual std::string toString() const { return "not implemented!";}
        virtual ParserType Type() const {return EXPR;}
        virtual ~Expr() = default;
        #ifdef COMPILER
        virtual Result_MC* codegen() = 0;
        #endif
};

class Binary : public Expr {
    public:
        std::shared_ptr<Expr> left;
        TokenData data;
        std::shared_ptr<Expr> right;
        Binary(std::shared_ptr<Expr> left, TokenData op, std::shared_ptr<Expr> right);
        std::string toString() const override {
            return left->toString() + " " + this->data.token + " " + right->toString();
        }
        ParserType Type() const override { return BINARY; }
        #ifdef COMPILER
        Result_MC* codegen() override;
        #endif

};

class Literal : public Expr {
    public:
        TokenData str;
        Literal(TokenData str);
        std::string toString() const override {
            return str.token;
        }
        ParserType Type() const override { return LITERAL; }
        #ifdef COMPILER
        Result_MC* codegen() override;
        #endif
};

class Grouping : public Expr {
    public:
        std::shared_ptr<Expr> expr;
        Grouping(std::shared_ptr<Expr> expr);
        std::string toString() const override {
            return "( " + expr->toString()+ " )"; 
        }
        ParserType Type() const override { return GROUPING; }
        #ifdef COMPILER
        Result_MC* codegen() override {return NULL;}
        #endif
};

class Unary : public Expr {
    public:
        TokenData op;
        std::shared_ptr<Expr> expr;
        Unary(std::shared_ptr<Expr> expr, TokenData data);
        std::string toString() const override {
            return expr->toString();
        }
        ParserType Type() const override { return UNARY; }
        #ifdef COMPILER
        Result_MC* codegen() override {return nullptr;}
        #endif
};

class Statement : public Expr {
    public:
        TokenData name;
        std::vector<std::shared_ptr<Expr>> args;
        Statement(TokenData name, std::vector<std::shared_ptr<Expr>> args) {
            this->name = name;
            this->args = args;
        }
        std::string toString() const override {
            return this->name.token;
        }
        ParserType Type() const override {
            return STATEMENT;
        }
        #ifdef COMPILER
        Result_MC* codegen() override;
        #endif
};

class Variable : public Expr {
    public:
        TokenData type;
        TokenData name;
        SExpr expr;
        Variable(TokenData type, TokenData name, SExpr expr) {
            this->type = type;
            this->name = name;
            this->expr = expr;
        }
        std::string toString() const override {
            return this->name.token;
        }
        ParserType Type() const override {
            return VARIABLE;
        }
        #ifdef COMPILER
        Result_MC* codegen() override;
        #endif
};

class Var : public Expr {

    public:
        TokenData name;
        Var(TokenData name) {
            this->name = name;
        }
        std::string toString() const override {
            return this->name.token;
        }
        ParserType Type() const override {
            return VAR;
        }
        #ifdef COMPILER
        Result_MC* codegen() override;
        #endif
};

class Assign : public Expr {
    public:
        TokenData name;
        SExpr expr;
        Assign(TokenData name, SExpr expr) {
            this->name = name;
            this->expr = expr;
        }
        std::string toString() const override {
            return this->name.token;
        }
        ParserType Type() const override {
            return ASSIGN;
        }
        #ifdef COMPILER
        Result_MC* codegen() override {return nullptr;}
        #endif
};

class Block : public Expr {
    public:
        std::list<SExpr> expr;
        Block(std::list<SExpr> expr) {
            this->expr = expr;
        }
        
        std::string toString() const override {
            return "list not implemented";
        }
        ParserType Type() const override {
            return SCOPE;
        }
        #ifdef COMPILER
        Result_MC* codegen() override {return nullptr;}
        #endif
};

class IfStatement : public Expr {
    public:
        SExpr condition, then , elseblock;
        IfStatement(SExpr condition, SExpr then, SExpr elseblock) {
            this->condition = condition;
            this->then = then;
            this->elseblock = elseblock;
        }
        std::string toString() const override {
            return "If not implemented";
        }
        ParserType Type() const override { return IFSTATEMENT; }
        #ifdef COMPILER
        Result_MC* codegen() override {return nullptr;}
        #endif
};

class WhileLoop : public Expr {
    public:
        SExpr condition,block;
        WhileLoop(SExpr condition, SExpr block) {
            this->condition = condition;
            this->block = block;
        }
        std::string toString() const override {
            return "while loop not implemented";
        }
        ParserType Type() const override { return WHILELOOP; }
        #ifdef COMPILER
        Result_MC* codegen() override {return nullptr;}
        #endif
};


class FuncCall : public Expr {
    public:
        SExpr caller;
        std::list<std::shared_ptr<Expr>> args;
        FuncCall(SExpr caller, std::list<std::shared_ptr<Expr>> args) {
            this->caller = caller;
            this->args = args;
        }
        std::string toString() const override {
            return "caller not implemented!";
        }
        ParserType Type() const override {
            return CALL;
        }
        #ifdef COMPILER
        Result_MC* codegen() override {return nullptr;}
        #endif
};


struct ArgsPars {
    TokenData datatype;
    TokenData pars;
};

class FuncDef : public Expr {
    public:
        TokenData name;
        std::list<ArgsPars> pars;
        std::list<SExpr> body;
        FuncDef(TokenData name, std::list<ArgsPars> pars, std::list<SExpr> body) {
            this->name = name;
            this->pars = pars;
            this->body = body;
        }
        std::string toString() const override {
            return "FuncDef not implemented!";
        }
        ParserType Type() const override {
            return FUNC;
        }
        #ifdef COMPILER
        Result_MC* codegen() override {return nullptr;}
        #endif
};

class ReturnStmt : public Expr {
    public:
        TokenData keyw;
        SExpr value;
        ReturnStmt(TokenData keyw, SExpr value) {
            this->keyw = keyw;
            this->value = value;
        }
        ParserType Type() const override {
            return RETURN;
        }
        #ifdef COMPILER
        Result_MC* codegen() override {return nullptr;}
        #endif
};

class ListStmt : public Expr {
    public:
        std::vector<SExpr>expr;
        ListStmt(std::vector<SExpr> expr) {
            this->expr = expr;
        }
        ParserType Type() const override {
            return ParserType::LIST;
        }
        #ifdef COMPILER
        Result_MC* codegen() override {return nullptr;}
        #endif
};

class ListIndexStmt : public Expr {
    public:
        SExpr expr, index;
        ListIndexStmt(SExpr expr, SExpr index) {
            this->expr = expr;
            this->index = index;
        }
        ParserType Type() const override {
            return LISTINDEX;
        }
        #ifdef COMPILER
        Result_MC* codegen() override {return nullptr;}
        #endif
};


class Parser {
    protected:
        std::string filename="stdin";
        std::list<SExpr> pars;
        std::vector<TokenData> tok;
        SExpr expr();
        SExpr Shifting();
        SExpr Logical();
        SExpr Equality();
        SExpr Compare();
        SExpr term();
        SExpr factor();
        SExpr unary();
        SExpr Primary();
        SExpr statement();
        SExpr Declaration();
        std::list<SExpr> block();
        SExpr Assignment();
        SExpr Ifstatement();
        SExpr Whileloop();
        SExpr Forloop();
        SExpr funcDef();
        SExpr Call();
        SExpr listStmt();
        SExpr listIndexStmt();
        SExpr returnStatement();
        SExpr finishCall(SExpr caller);
        bool IsEOF();
        bool IsMatch(std::list<TokenType> t);
        bool CheckType(TokenType t);
        TokenData nextToken();
        TokenData CurrToken();
        TokenData Previous();
        TokenData NextTokenNoChange();
        bool CheckNextType(TokenType t);
        TokenData CheckSyntax(TokenType type, std::string message);
    public:
        Parser(std::vector<TokenData> token);
        Parser(std::string filename, std::vector<TokenData> token);
        std::list<SExpr> ReturnExpr();
        
};