#include <Syntax.h>
#include <ErrorHandler.h>

Binary::Binary(std::shared_ptr<Expr> left, TokenData data, std::shared_ptr<Expr> right){
    this->left = left;
    this->data = data;
    this->right = right;
}

Literal::Literal(TokenData str) {
    this->str = str;
}

Grouping::Grouping(std::shared_ptr<Expr> expr) {
    this->expr = expr;
}

Unary::Unary(std::shared_ptr<Expr> expr, TokenData data)
{
    this->expr = expr;
    this->op = data;
}


int current,start,line,endcurrent;

Parser::Parser(std::vector<TokenData> token)
{
    current = start = endcurrent = 0;
    line = 1;
    this->tok = token;
}

Parser::Parser(std::string filename,std::vector<TokenData> token)
{
    current = start = endcurrent = 0;
    line = 1;
    this->tok = token;
    this->filename =filename;
}

SExpr Parser::finishCall(SExpr caller)
{
    std::list<SExpr> args;
    if (!CheckType(RIGHT_PAREN)) {
        do {
            if (args.size() >= args.max_size()) {
                ErrorHandler::pError("Out of maximum argument count", filename, 1,endcurrent);
                exit(1);
            }
            args.push_back(expr());
        }while(IsMatch({COMMA}));
    }

    CheckSyntax(RIGHT_PAREN, "Expect ')'");

    return std::make_shared<FuncCall>(FuncCall(caller, args));
}

bool Parser::IsEOF()
{
    return this->tok[current].type == eof;
}

bool Parser::IsMatch(std::list<TokenType> t)
{
    for (TokenType t_ : t) {
        if (CheckType(t_)) {
            nextToken();
            return true;
        }   
    }
    return false;
}

bool Parser::CheckType(TokenType t)
{
    return IsEOF() ? false : CurrToken().type == t;
}

bool Parser::CheckNextType(TokenType t)
{
    return IsEOF() ? false : NextTokenNoChange().type == t;
}

TokenData Parser::nextToken()
{
    if (!IsEOF()) { current++; endcurrent++;}
    return Previous();
}

TokenData Parser::CurrToken()
{
    return tok.at(current);
}

TokenData Parser::Previous()
{
    return tok.at(current-1);
}

TokenData Parser::NextTokenNoChange() {
    return IsEOF() ? Previous() : tok.at(current+1);
}

std::list<std::shared_ptr<Expr>> Parser::ReturnExpr()
{
    while (!IsEOF()) {
        pars.push_back(Parser::Declaration());
        line++;
        endcurrent=0;
    }
    return pars;
}


SExpr Parser::Declaration() {
    TokenData t = CurrToken();
    if (t.token == "char" || t.token == "short" ||
    t.token == "long" || t.token == "int" || t.token == "suu" ||
    t.token == "any" || t.token == "float") {
        nextToken();
        TokenData name = CheckSyntax(IDENTIFIER, "Expect a variable name.");

        if (t.token == "suu") {
            ErrorHandler::pWarning("variable '"+name.token+"' is initialized using suu data type, which is unstable. Consider to use float instead.",
            filename, name.line, endcurrent);
        }

        SExpr expr_init;
        if (IsMatch({EQUAL}))
            expr_init = expr();
        

        CheckSyntax(SEMICOLON, "Expect ';'");
        return std::make_shared<Variable>(Variable(t, name, expr_init));
    }
    return statement();
}

SExpr Parser::Ifstatement() {
    CheckSyntax(LEFT_PAREN, "Expect '('");
    SExpr condition = expr();
    CheckSyntax(RIGHT_PAREN, "Expect ')'");
    SExpr thenblock=statement();
    SExpr elseblock;
    if (!IsEOF()) {
        if (expr()->toString() == "else") {
            elseblock = statement();
        } else current--;
    }
    return std::make_shared<IfStatement>(IfStatement(condition, thenblock, elseblock));
}

SExpr Parser::Whileloop()
{
    CheckSyntax(LEFT_PAREN, "Expect '('");
    SExpr condition = expr();
    CheckSyntax(RIGHT_PAREN, "Expect ')'");
    SExpr body = statement();

    return std::make_shared<WhileLoop>(condition, body);
}

SExpr Parser::Forloop()
{
    CheckSyntax(LEFT_PAREN, "expect '('");
    SExpr init= IsMatch({SEMICOLON}) ? nullptr : Declaration();
    SExpr condition = nullptr;
    if (!CheckType(SEMICOLON)) condition = expr();
    CheckSyntax(SEMICOLON, "Expect ';'");
    SExpr incr = nullptr;
    if (!CheckType(RIGHT_PAREN)) incr = expr();
    CheckSyntax(RIGHT_PAREN, "expect ')'");
    SExpr body = statement();
    if (incr != nullptr) {
        body = std::make_shared<Block>(Block(
            {
                body,
                incr
            }
        ));
    }
    if (condition == nullptr) {
        TokenData data;
        data.token = data.literal = "1";
        data.line = line;
        condition = std::make_shared<Literal>(Literal(data));
    }
    body = std::make_shared<WhileLoop>(condition, body);
    if (init != nullptr) body = std::make_shared<Block>(Block({init, body}));
    return body;
}

SExpr Parser::Call()
{
    SExpr func_name = Primary();
    if (func_name->toString() == "if" || 
    func_name->toString() == "while") return func_name;
    while(1) {
        if (IsMatch({LEFT_PAREN})) {
            func_name = finishCall(func_name);
        } else break;
    }


    return func_name;
}

SExpr Parser::Assignment()
{
    SExpr expr = listIndexStmt();
    if (IsMatch({EQUAL, PLUS_EQUAL, MINUS_EQUAL, SLASH_EQUAL, STAR_EQUAL})) {
        TokenData equal = Previous();
        SExpr value = Assignment();

        if (expr->Type() == VAR) {
            TokenData name = (*(Var*)expr.get()).name;
            return std::make_shared<Assign>(Assign(name, value));
        }
        ErrorHandler::pError("Invaild assignment target: " + equal.token, filename,
        equal.line, endcurrent);
        exit(1);
    }
    return expr;
}

SExpr Parser::funcDef() {
    TokenData name = CheckSyntax(IDENTIFIER, "Expect function name");
    CheckSyntax(LEFT_PAREN, "Expect '(' - FuncDef");
    std::list<ArgsPars> pars;
    if (!CheckType(RIGHT_PAREN)) {
        do {
            if (pars.size() >= pars.max_size()) {
                ErrorHandler::pError("Out of maximum argument count", filename, 1,endcurrent);
            }
            pars.push_back(
                {
                    CheckSyntax(KEYWORDS, "Expect data type"),
                    CheckSyntax(IDENTIFIER, "Expect parameter name.")
                }
            );
        }while(IsMatch({COMMA}));
    }
    CheckSyntax(RIGHT_PAREN, "Expect ')' - FuncDef");

    CheckSyntax(LEFT_CR_BRACKET, "Expect '{' - FuncDef");
    std::list<SExpr> body = block();

    return std::make_shared<FuncDef>(FuncDef(name, pars, body));
}

SExpr Parser::returnStatement() {
    TokenData keyw = Previous();
    SExpr value = nullptr;
    if (!CheckType(SEMICOLON)) value = expr();
    CheckSyntax(SEMICOLON, "Expect ';' - Return ret");
    return std::make_shared<ReturnStmt>(ReturnStmt(keyw, value));
}

SExpr Parser::statement() {
    SExpr name_expr = expr();
    if (name_expr->toString() == "if") return Ifstatement();
    else if (name_expr->toString() == "while") return Whileloop();
    else if (name_expr->toString() == "for") return Forloop();
    else if (name_expr->toString() == "func") return funcDef();
    else if (name_expr->toString() == "ret") return returnStatement();

    if (name_expr->Type() != ASSIGN && name_expr->Type() != SCOPE && name_expr->Type() != CALL
&& name_expr->Type() != BINARY && name_expr->Type() != UNARY) {
            TokenData name = Previous();
            std::vector<SExpr> args;
            while (!IsEOF() && CurrToken().type != SEMICOLON) {
                args.push_back(std::move(expr()));
            }
            CheckSyntax(SEMICOLON, "Expect ';'");
            return std::make_shared<Statement>(Statement(name,args));
    }
    if (name_expr->Type() != SCOPE) {
        CheckSyntax(SEMICOLON, "Expect ';'");
    }
    return name_expr;
}

SExpr Parser::listStmt() {
    // list [1,2,3,4] or list index a[0]
    std::vector<SExpr> cont;
    while(!IsEOF()) {
        cont.push_back(expr());
        if (CheckType(RIGHT_SQ_BRACKET)) break;
        CheckSyntax(COMMA, "Expect ',' - Liststmt");
    }
    CheckSyntax(RIGHT_SQ_BRACKET, "Expect ']' - Liststmt");
    return std::make_shared<ListStmt>(ListStmt(cont));
}

SExpr Parser::listIndexStmt()
{
    SExpr expr = Shifting();
    if (IsMatch({LEFT_SQ_BRACKET})) {
        SExpr index = listIndexStmt();
        CheckSyntax(RIGHT_SQ_BRACKET, "Expect ']' - ListIndex");
        return std::make_shared<ListIndexStmt>(ListIndexStmt(expr, index));
    }
    return expr;
}

std::shared_ptr<Expr> Parser::expr()
{
    return Assignment();
}

std::shared_ptr<Expr> Parser::Shifting()
{
    std::shared_ptr<Expr> expr = Logical();

    while(IsMatch({SHIFT_LEFT, SHIFT_RIGHT})) {
        TokenData op = Previous();
        std::shared_ptr<Expr> right = Logical();
        expr = std::make_shared<Binary>(Binary(
            expr,
            op,
            right
        ));
    }
    return expr;
}


std::shared_ptr<Expr> Parser::Logical()
{
    std::shared_ptr<Expr> expr = Equality();

    while(IsMatch({AND, XOR, OR})) {
        TokenData op = Previous();
        std::shared_ptr<Expr> right = Equality();
        expr = std::make_shared<Binary>(Binary(
            expr,
            op,
            right
        ));
    }
    return expr;
}

std::shared_ptr<Expr> Parser::Equality()
{
    std::shared_ptr<Expr> expr = Compare();

    while(IsMatch({NOT_EQUAL, EQUAL_EQUAL})) {
        TokenData op = Previous();
        std::shared_ptr<Expr> right = Compare();
        expr = std::make_shared<Binary>(Binary(
            expr,
            op,
            right
        ));
    }
    return expr;
}

std::shared_ptr<Expr> Parser::Compare()
{
    std::shared_ptr<Expr> expr = term();

    while(IsMatch({GREATER, GREATER_EQUAL, LESS, LESS_EQUAL})) {
        TokenData op = Previous();
        std::shared_ptr<Expr> right = term();
        expr = std::make_shared<Binary>(Binary(
            expr,
            op,
            right
        ));
    }
    return expr;
}

std::shared_ptr<Expr> Parser::term()
{
    
    std::shared_ptr<Expr> expr = factor();

    while(IsMatch({MINUS, PLUS})) {
        TokenData op = Previous();
        std::shared_ptr<Expr> right = factor();
        expr = std::make_shared<Binary>(Binary(
            expr,
            op,
            right
        ));
    }
    return expr;
}

std::shared_ptr<Expr> Parser::factor()
{
    
    std::shared_ptr<Expr> expr = unary();

    while(IsMatch({STAR, SLASH})) {
        TokenData op = Previous();
        std::shared_ptr<Expr> right = unary();
        expr = std::make_shared<Binary>(Binary(
            expr,
            op,
            right
        ));
    }
    return expr;
}

std::shared_ptr<Expr> Parser::unary()
{
    if (IsMatch({EXCLA_MARK, MINUS, NOT})) {
        TokenData op = Previous();
        std::shared_ptr<Expr> expr = unary();
        return std::make_shared<Unary>(Unary(expr, op));
    } 
   
    return Call();
}

TokenData Parser::CheckSyntax(TokenType type, std::string message) {
    if (CheckType(type)) return nextToken();
    ErrorHandler::pError(message, filename, CurrToken().line, current);
    exit(1);
}

std::shared_ptr<Expr> Parser::Primary()
{
    if (IsMatch({NUMBER, STRING, CHAR})) {
        return std::make_shared<Literal>(Previous());
    }
    if (IsMatch({LEFT_PAREN})) {
        std::shared_ptr<Expr> expr_ = expr();
        CheckSyntax(RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_shared<Grouping>(Grouping(expr_));
    }
    if (IsMatch({KEYWORDS})) {
        return std::make_shared<Literal>(Literal(Previous()));
    }
    if (IsMatch({IDENTIFIER})) {
        return std::make_shared<Var>(Previous());
    }
    if (IsMatch({LEFT_CR_BRACKET})) return std::make_shared<Block>(Block(block()));
    if (IsMatch({LEFT_SQ_BRACKET})) return listStmt();
    ErrorHandler::pError("Expect Expression at '"+CurrToken().token+"'", filename, CurrToken().line, endcurrent);
    exit(1);
}

std::list<SExpr> Parser::block() {
    std::list<SExpr> st;
    while(!CheckType(RIGHT_CR_BRACKET) && !IsEOF()) st.push_back(Declaration());

    CheckSyntax(RIGHT_CR_BRACKET, "Expect '}'");
    return st;
}
