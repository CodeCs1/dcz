#pragma once

#include <Syntax.h>
#include <optional>
#include <algorithm>
#include "ErrorHandler.h"

struct Environment_Variable_MC {
    size_t offset;
    std::string datatype;
    std::string name;
    std::string value;
};

class Environment_MC { // distinguished from Interpreter Environment
    private:
        std::string filename;
        std::vector<Environment_Variable_MC> vars;
        std::shared_ptr<Environment_MC> enclosing;
        size_t offset;
    public:
        Environment_MC(std::string filename) {
            this->filename = filename;
        }
        Environment_MC() {
            this->filename = "stdin";
        }
        void define(TokenData data_type, std::string name, std::string value, size_t offset) {
            vars.push_back({offset, data_type.token, name, value});
        }

        Environment_MC(std::shared_ptr<Environment_MC> enclosing) {
            this->enclosing = enclosing;
            vars.clear();
        }

        Environment_Variable_MC get(TokenData name) {
            int i=0;
            if (std::find_if(vars.begin(), vars.end(), [&](Environment_Variable_MC envi) {
                i++; 
                return (envi.name == name.token);
            })!=vars.end()) {
                
                return vars[i-1];
            }
            
            if (enclosing != nullptr) return enclosing->get(name);

            ErrorHandler::pError("Undefined variable '"+name.token+"'",
                filename,1, 1);
            exit(1);
        }
};

struct MachineCodeASM {
    std::optional<std::string> data;
    std::optional<uint64_t> opcode;
};

class AST2MC {
    private:
        std::string filename="stdin";
        std::string arch="8086";
        std::list<SExpr> expr;
        int memoryorigin;
        int count = 0;
        SExpr CurrentExpr();
        std::vector<MachineCodeASM> mca;
        void PushOpcode(uint64_t opcode);
        std::vector<int> Regstate;

        int getRegs();
        int AllocRegs();
        void freeRegs();
        int pointerRegs;

        size_t offset=0;
    public:
        AST2MC(std::string filename,std::list<SExpr> list,int memoryorigin);
        std::vector<MachineCodeASM> ReturnProg();
};