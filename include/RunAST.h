#pragma once

#include <Syntax.h>
#include <algorithm>
#include <ErrorHandler.h>
#include <cmath>
#include <cstring>
#include <Caller.h>

#include <variant>
#include <iostream>
#include <optional>

class Func;

typedef struct Result {
    TokenType type;
    ParserType pType;
    std::optional<std::variant< std::string, std::vector<SExpr>, int, float >> value;
    int line;
}Result_t;

struct EnvironmentVariable{
    std::string datatype;
    std::string name;
    Result value;
    std::shared_ptr<Func> func;
};


extern bool IsStrNum(std::string str);
extern bool IsStrInt(std::string str);


class Environment {
    private:
        std::vector<EnvironmentVariable> vars;
        std::string filename="stdin";
        std::shared_ptr<Environment> enclosing;
    public:
        Environment(std::string filename) {
            vars.clear();
            this->filename = filename;
        }
        Environment(std::string filename, std::shared_ptr<Environment> enclosing) {
            this->filename = filename;
            this->enclosing = enclosing;
            vars.clear();
        }
        
        Environment(std::shared_ptr<Environment> enclosing) {
            this->enclosing = enclosing;
            vars.clear();
        }
        Environment() {
            vars.clear();
        }
        void defineFunc(std::string func_name, std::shared_ptr<Func> def) {
            vars.push_back({std::string("func"), func_name, {IDENTIFIER, VAR, func_name, 1}, def});
        }

        void append(std::shared_ptr<Environment> var) {
            for (auto x : var->vars) {
                vars.push_back(x);
            }
        }

        void define(std::string datatype,std::string name, Result value) {
            vars.push_back({datatype, name, value,nullptr});
        }
        void assign(TokenData name, Result value) {
            int i=0;
            if (std::find_if(vars.begin(), vars.end(), [&](EnvironmentVariable envi) {
                i++; return (envi.name == name.token);
            })!=vars.end()) {
                EnvironmentVariable var;
                var.datatype = vars[i-1].datatype;
                var.name = name.token;
                var.value = value;
                var.func=nullptr;
                vars[i-1] = var;
                return;
            }
            if (enclosing != nullptr) enclosing->assign(name, value);
            ErrorHandler::pError("Undefined variable '"+name.token+"'",
                filename, name.line, 1);
            exit(1);
        }
        EnvironmentVariable get(std::string name) {
            int i=0;
            if (std::find_if(vars.begin(), vars.end(), [&](EnvironmentVariable envi) {
                i++; 
                return (envi.name == name);
            })!=vars.end()) {
                
                return vars[i-1];
            }
            
            if (enclosing != nullptr) return enclosing->get(name);

            ErrorHandler::pError("Undefined variable '"+name+"'",
                filename,1, 1);
            exit(1);
        }

        size_t Size() {
            return vars.size();
        }
};



class RunAST {
    protected:
        std::list<SExpr> pars;
        bool VerboseEnabled=false;
        std::string filename = "stdin";
        void RunSingleOpcode();
        int count=0;
        SExpr CurrentExpr();
        Result Visit(SExpr Node);
        Result VisitLiteral(SExpr Node);
        Result VisitBinary(SExpr Node);
        Result VisitGroupping(SExpr Node);
        Result VisitStatement(SExpr Node);
        Result VisitVariable(SExpr Node);
        Result VisitVar(SExpr Node);
        Result VisitBlock(SExpr Node);
        Result VisitIf(SExpr Node);
        Result VisitWhile(SExpr Node);
        Result VisitAssignment(SExpr Node);
        Result VisitReturn(SExpr Node);
        Result VisitUnary(SExpr Node);
        Result VisitCall(SExpr Node);
        Result VisitFunc(SExpr Node);
        Result VisitList(SExpr Node);
        Result VisitListIndex(SExpr Node);
        Environment envi=globals;
    public:
        Environment globals;
        void UpdateGlobal();
        uint8_t IsDone=0;
        RunAST(bool VerboseEnabled,std::string filename,std::list<SExpr> pars);
        RunAST(bool VerboseEnabled,std::list<SExpr> pars);
        Result ExecuteBlock(std::list<SExpr> node, Environment envi);
        void Run();
};