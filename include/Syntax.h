#pragma once

#include <Token.h>
#include <string>
#include <vector>
#include <list>
#include <memory>

#include <codegen/Result_MC.h>

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
        virtual Result_MC* codegen() = 0;
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

        Result_MC* codegen() override;

};

class Literal : public Expr {
    public:
        TokenData str;
        Literal(TokenData str);
        std::string toString() const override {
            return str.token;
        }
        ParserType Type() const override { return LITERAL; }
        Result_MC* codegen() override;
};

class Grouping : public Expr {
    public:
        std::shared_ptr<Expr> expr;
        Grouping(std::shared_ptr<Expr> expr);
        std::string toString() const override {
            return "( " + expr->toString()+ " )"; 
        }
        ParserType Type() const override { return GROUPING; }
        Result_MC* codegen() override {return NULL;}
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
        Result_MC* codegen() override {return nullptr;}
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
        Result_MC* codegen() override {return NULL;}
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
        Result_MC* codegen() override;
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
        Result_MC* codegen() override;
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
        Result_MC* codegen() override {return nullptr;}
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
        Result_MC* codegen() override {return nullptr;}
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
        Result_MC* codegen() override {return nullptr;}
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
        Result_MC* codegen() override {return nullptr;}
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
        Result_MC* codegen() override {return nullptr;}
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
        Result_MC* codegen() override {return nullptr;}
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
        Result_MC* codegen() override {return nullptr;}
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
        Result_MC* codegen() override {return nullptr;}
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
        Result_MC* codegen() override {return nullptr;}
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