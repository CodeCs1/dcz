#include <RunAST.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

bool loopBlock=false;

/*
    The most bloated way to Visit AST. (using a lot of std lib -> slow compilation + not optimizied)
*/

#ifdef _WIN32
#define realpath(N,R) _fullpath((R),(N),_MAX_PATH)
#endif

RunAST::RunAST(bool VerboseEnabled,std::string filename, std::list<SExpr> pars) : envi(filename)
{
    this->filename = filename;
    this->pars = pars;
    this->VerboseEnabled = VerboseEnabled;
    this->globals = Environment(filename);
    this->envi = globals;
}
RunAST::RunAST(bool VerboseEnabled,std::list<SExpr> pars) :envi("stdin") {
    this->pars = pars;
    this->VerboseEnabled = VerboseEnabled;
    this->globals = Environment(filename);
    this->envi = globals;
}

Result RunAST::ExecuteBlock(std::list<SExpr> node, Environment envi)
{
    Environment prev = this->envi;
    this->envi = envi;
    for (auto x : node) {
        Result r = Visit(x);
        if (x->Type() == RETURN || IsDone) { 
            this->envi = prev; 
            IsDone = 0;
            return r; 
        }
    }
    if (!loopBlock) this->envi = prev;
    return Result();
}

void RunAST::UpdateGlobal() {
    this->globals = this->envi;
}


SExpr RunAST::CurrentExpr()
{
    auto l_expr = this->pars.begin();
    std::advance(l_expr, count);
    return *l_expr;
}
Result RunAST::VisitLiteral(SExpr Node) {
    Literal& lit = *(Literal*)Node.get();
    Result res;
    res.value = lit.str.literal;
    res.type = lit.str.type;
    res.line = lit.str.line;
    return res;
}

Result RunAST::VisitGroupping(SExpr Node) {
    Grouping& gr = *(Grouping*)Node.get();
    return Visit(gr.expr);
}

Result RunAST::VisitBinary(SExpr Node)
{
    Binary& bin = *(Binary*)Node.get();
    std::string ret = "";
    auto vl = Visit(bin.left);
    auto vr = Visit(bin.right);
    float l,r;
    if (vl.type == NUMBER && vr.type == NUMBER) {
        l = std::stof(std::get<std::string>(vl.value.value()));
        r = std::stof(std::get<std::string>(vr.value.value()));
    }

    switch(bin.data.type) {
        case PLUS:
            ret = std::to_string(l+r);
            break;
        case MINUS:
            ret = std::to_string(l-r);
            break;
        case STAR:
            ret = std::to_string(l*r);
            break;
        case SLASH:
            if (r == 0) { ErrorHandler::pError("Divided by 0", filename, 1, count);exit(1); }
            ret = std::to_string(l/r);
            break;
        case SHIFT_LEFT:
            ret = std::to_string((int)l << (int)r);
            break;
        case SHIFT_RIGHT:
            ret = std::to_string((int)l >> (int)r);
            break;       
        case AND:
            ret = std::to_string((int)l & (int)r);
            break;
        case XOR:
            ret = std::to_string((int)l ^ (int)r);
            break;
        case OR:
            ret = std::to_string((int)l | (int)r);
            break;
        case LESS:
            ret = std::to_string(l < r);
            break;
        case GREATER:
            ret = std::to_string(l > r);
            break;
        case LESS_EQUAL:
            ret = std::to_string(l <= r);
            break;
        case GREATER_EQUAL:
            ret = std::to_string(l >= r);
            break;
        case NOT_EQUAL:
            ret = std::to_string(l != r);
            break;
        case EQUAL_EQUAL:
            ret = std::to_string(l == r);
            break;
    }
    Result data;
    data.value = ret;
    data.pType = LITERAL;
    return data;
}

std::string dirnameOf(const std::string& fname)
{
     size_t pos = fname.find_last_of("\\/");
     return (std::string::npos == pos)
         ? ""
         : fname.substr(0, pos+1);
}

Result RunAST::VisitStatement(SExpr Node) {
    Statement& state = *(Statement*)Node.get();
    std::string func_name = state.name.token;
    std::vector<Result> args;
    for (auto x : state.args) {
        args.push_back(Visit(x));
    }

    if (func_name == "Lprint") {
        for (auto x : args) 
            std::cout << std::get<std::string>(x.value.value());
        std::cout << std::endl;
    }
    else if (func_name == "import") {
        if (args.size() == 0) {
            ErrorHandler::pError("Expect file name - Import Statement", filename,
            state.name.line,1);
            exit(1);
        }
        std::string r_p = std::string(realpath(filename.c_str(),NULL));
        for (auto x : args) {
            //check if file exist
            std::string filename_i = std::get<std::string>(x.value.value());
            std::ifstream f(dirnameOf(r_p)+filename_i+".dcz");
            if (f.fail()) {
                ErrorHandler::pWarning("File " + dirnameOf(r_p)+filename_i + ".dcz does not exist, skipping",
                filename, state.name.line, 1);
                continue;
            }
            
            std::string s; {
                std::stringstream str;
                str <<f.rdbuf();
                s = str.str();
            }

            Token t_i(filename_i, s);
            t_i.ParseLexer();
            Parser pars(filename_i, t_i.ReturnLexer());
            auto x_i = pars.ReturnExpr();

            auto rast = RunAST(VerboseEnabled, x_i);
            rast.Run();
            envi.append(std::make_shared<Environment>(rast.envi));
        }
    }
    return Result();
}


Result RunAST::VisitBlock(SExpr Node) {
    Block& bl = *(Block*)Node.get();
    ExecuteBlock(bl.expr, Environment(this->envi));
    return Result();
}

Result RunAST::VisitIf(SExpr Node)
{
    IfStatement& ist = *(IfStatement*)Node.get();
    int ret;
    auto v = Visit(ist.condition);
    if (v.type == NUMBER)
        ret = std::get<int>(Visit(ist.condition).value.value());
    else ret = 1; // const always equal to 1
    if (ret) { return  Visit(ist.then); }
    else {
        if (ist.elseblock != nullptr) return Visit(ist.elseblock);
    }
    return Result();
}

Result RunAST::VisitWhile(SExpr Node)
{
    WhileLoop& wl = *(WhileLoop*)Node.get();
    loopBlock=true;
    while (std::get<int>(Visit(wl.condition).value.value())) Visit(wl.block);
    loopBlock=false;
    return Result();
}

Result RunAST::VisitUnary(SExpr Node) {
    Unary& un = *(Unary*)Node.get();
    Result v = Visit(un.expr);
    std::string datastr;
    switch(un.op.type) {
        case NOT:
            if (v.type != NUMBER) exit(1);
            datastr = std::to_string(~std::stoi(std::get<std::string>(v.value.value())));
            break;
        case MINUS:
            if (v.type != NUMBER) { ErrorHandler::pError("Not a real number",
            filename, 1, 1); exit(1); }
            datastr = std::to_string(-std::stoi(std::get<std::string>(v.value.value())));
            break;
        case EXCLA_MARK:
            if (v.type != NUMBER) { ErrorHandler::pError("Not a real number",
            filename, 1, 1); exit(1); }
            datastr = std::to_string(!std::stoi(std::get<std::string>(v.value.value())));
            break;
    }
    Result data;
    data.value =datastr;
    data.line = un.op.line;
    data.type = NUMBER; 
    data.pType = UNARY;
    return data;
}

Result RunAST::VisitVariable(SExpr Node) {
    Variable& var = *(Variable*)Node.get();
    if (var.expr == nullptr && var.type.token == "suu" && VerboseEnabled) {
        printf("[Suu]: I still haven't singled out a number to date! >~<\n");
    }
    Result value;
    if (var.expr != nullptr) {
        value = Visit(var.expr);
    }

    if (value.value.has_value() && var.type.token == "suu") {
        if (value.type != NUMBER) {
            if (VerboseEnabled) printf("[Suu]: Get that trash thing outta my face\n");
            ErrorHandler::pError("Can't convert variable '"+ var.name.token+"' from string to suu data type.",
                filename,var.type.line ,1);
            exit(1);
        }
        if (VerboseEnabled) printf("[Suu]: I'm in love with number %s ^^\n", std::get<float>(value.value.value()));
    }
    envi.define(var.type.token, var.name.token, value);
    return Result();
}
Result RunAST::VisitAssignment(SExpr Node) {
    Assign& assign = *(Assign*)Node.get();
    Result value = Visit(assign.expr);
    if (VerboseEnabled) {
        EnvironmentVariable old_value = envi.get(assign.name.token);
        if (old_value.value.value == value.value) {
            if (old_value.datatype == "suu")
                printf("[Suu]: Nah they're same.\n");
        } else {
            if (old_value.datatype == "suu")
            /*value.value.value.value.value.(...)*/
                printf("[Suu]: I can't choose between %d and %d...!\n", 
                    std::get<int>(old_value.value.value.value()),
                std::get<int>(value.value.value()));
        }
    }
    envi.assign(assign.name, value);
    return value;
}

Result RunAST::VisitReturn(SExpr Node)
{
    ReturnStmt& ret = *(ReturnStmt*)Node.get();
    Result value = ret.value != nullptr ? Visit(ret.value) : Result();
    IsDone = 1;
    return value;
}

Result RunAST::VisitVar(SExpr Node) {
    Var& v = *(Var*)Node.get();
    Result data;
    auto x = envi.get(v.name.token).value;
    data = x;
    return data;
}

Result RunAST::VisitCall(SExpr Node) {
    FuncCall& caller = *(FuncCall*)Node.get();
    Result call = Visit(caller.caller);
    std::list<Result> args;
    for (auto x : caller.args) args.push_back(Visit(x));

    auto func_envi = envi.get(std::get<std::string>(call.value.value()));

    return func_envi.func->call(this, args);
}

Result RunAST::VisitFunc(SExpr Node)
{
    FuncDef& def = *(FuncDef*)Node.get();
    std::shared_ptr<Func> func = std::make_shared<Func>(Func((def)));
    envi.defineFunc(def.name.token, func);
    return Result();
}

Result RunAST::VisitList(SExpr Node)
{
    ListStmt& liststmt = *(ListStmt*)Node.get();
    Result res;
    res.value = liststmt.expr;
    res.pType=LIST;
    return res;
}

Result RunAST::VisitListIndex(SExpr Node)
{
    ListIndexStmt& lis = *(ListIndexStmt*)Node.get();

    size_t index = std::stoi(std::get<std::string>(Visit(lis.index).value.value())); // always int
    auto expr = Visit(lis.expr);
    if (expr.pType != LIST) {
        ErrorHandler::pError("Invaild list type", filename, expr.line, 1);
        exit(1);
    }
    auto x = std::get<std::vector<SExpr>>(expr.value.value());
    if (index >= x.size()) {
        ErrorHandler::pError("Index out of bound", filename, expr.line, 1);
        exit(1);
    }
    Result res;
    res = Visit(x[index]);

    return res;
}

Result RunAST::Visit(SExpr Node)
{
    switch(Node->Type()) {
        case LITERAL:
            return VisitLiteral(Node);
        case BINARY:
            return VisitBinary(Node);
        case GROUPING:
            return VisitGroupping(Node);
        case STATEMENT:
            return VisitStatement(Node);
        case VARIABLE:
            return VisitVariable(Node);
        case VAR:
            return VisitVar(Node);
        case ASSIGN:
            return VisitAssignment(Node);
        case UNARY:
            return VisitUnary(Node);
        case SCOPE:
            return VisitBlock(Node);
        case IFSTATEMENT:
            return VisitIf(Node);
        case WHILELOOP:
            return VisitWhile(Node);
        case CALL:
            return VisitCall(Node);
        case FUNC:
            return VisitFunc(Node);
        case RETURN:
            return VisitReturn(Node);
        case LIST:
            return VisitList(Node);
        case LISTINDEX:
            return VisitListIndex(Node);
        default:
            std::cout << "Invaild parser type: " << (int)Node->Type() <<  std::endl;
            return Result();
    }
}

void RunAST::Run()
{
    while(count < pars.size()) { 
        Visit(CurrentExpr());
        count++;
    }
}
