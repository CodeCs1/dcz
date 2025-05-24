#include "AST2MC.h"
#include <iostream>
#include <codegen/Builder_MC.h>
#include <map>
using namespace std;

inline static Builder_MC builder=Builder_MC();
inline static std::map<std::string, Result_MC*> ValueMap;
Result_MC* Literal::codegen() {
    return this->str.type == TokenType::STRING ? new Constant_MC(this->str.literal) : new Constant_MC(std::stod(this->str.literal));
}

Result_MC* Binary::codegen() {
    auto L = left->codegen();
    auto R = right->codegen();
    if (!L || !R) return nullptr;

    switch(this->data.type) {
        case PLUS:
            return builder.AddIntrBinary(L,R);
        case MINUS:
            return builder.SubIntrBinary(L,R);
        case STAR:
            return builder.MulIntrBinary(L,R);
        case SLASH:
            return builder.DivIntrBinary(L,R);
        default:
            return nullptr;
    }
}

Result_MC* Var::codegen() {
    auto v = ValueMap[this->name.token];
    if (!v) { ErrorHandler::pError("Undefined variable: " + this->name.token,"stdin",
    this->name.line,1); exit(1);}
    return v;
}

Result_MC* Variable::codegen() {
    if (ValueMap[this->name.token] != nullptr) {
        printf("Variable %s is already declared.\n", this->name.token.c_str());
        exit(1);
    }
    ValueMap[this->name.token] = this->expr->codegen();
    return nullptr;
}

Result_MC* Statement::codegen() {

    std::list<Result_MC*> arg1;
    for (auto arg: this->args) arg1.push_back(arg->codegen());
    if (this->name.token == "Lprint") {
        for (auto arg : arg1) {
            if (arg->v().str.has_value())
                printf("%s ",arg->v().str.value().c_str());
            if (arg->v().n.has_value())
                printf("%f ",arg->v().n.value());
        }
        printf("\n");
    }else if (this->name.token == "import") {
        //TODO
    }
    return nullptr;
}


AST2MC::AST2MC(std::string filename, std::list<SExpr> list,int memoryorigin)
{
    this->filename = filename;
    this->memoryorigin = memoryorigin;
    this->expr = list;
}

void AST2MC::PushOpcode(uint64_t opcode)
{
    this->mca.push_back({std::nullopt, opcode});
}

int AST2MC::getRegs()
{
    return pointerRegs;
}

int AST2MC::AllocRegs()
{
    if (pointerRegs >= 7) {
        printf("...\n");
        exit(1);
    }
    Regstate[pointerRegs]=1; // in use
    return (pointerRegs++)-1;
}

void AST2MC::freeRegs()
{
    if (pointerRegs < 0) pointerRegs = 0;
    Regstate[pointerRegs]=0;
    pointerRegs--;
}


SExpr AST2MC::CurrentExpr() {
    auto l_expr = this->expr.begin();
    std::advance(l_expr, count);
    return *l_expr;
}

std::vector<MachineCodeASM> AST2MC::ReturnProg()
{
    while(count < expr.size()) {       
        auto x = CurrentExpr()->codegen();
        if (x!=nullptr) x->dump(); 
        offset = count;
        count++;
    }
    return this->mca;
}
