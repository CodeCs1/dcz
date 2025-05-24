#include <Token.h>
#include <ErrorHandler.h>

using namespace std;

static vector<string> Keywords = {
        "func",
        "if",
        "else",
        "for",
        "while",
        "class",
        "export",
        "char",
        "int",
        "short",
        "long",
        /*
        suu or sū in japanese which mean 'number'. I represent this as real number. (or 'float' at least)
        This data type take from manga character: 一二三数 [hifumi suu]
        */
        "suu",
        "float",
        "ret",
        "Lprint",
        "import",
        "any"
};


extern int current,start,line,endcurrent;
Token::Token(string code) {
    current = start = endcurrent =0;
    line = 1;
    this->code = code;
}

Token::Token(string filename, string code) {
    this->code = code;
    this->filename = filename;
    current = start = endcurrent =0;
    line = 1;
}

char Token::NextChrs() {
    endcurrent++;
    return IsAtEnd() ? '\0' : this->code[current++];
}

char Token::CurrentChrs() {
    return this->code[current];
}

bool Token::IsAtEnd() {
    return current >= this->code.length();
}

bool Token::Match(char expected) {
    if (IsAtEnd()) return false;
    if (CurrentChrs() != expected) return false;

    current++;
    endcurrent++;
    return true;
  }

void Token::Add2Token(TokenType type, std::string str) {
    token.push_back({this->code.substr(start, current-start),str, type, line});
}
void Token::Add2Token(TokenType type) {
    Add2Token(type, "");
}

bool Token::MatchStr(string str) {
    for (char c : str) {
        if (!Match(c)) return false;
    }
    return true;
}

char Token::NextCurrentChrs() {
    return current+1 >= this->code.length() ? '\0' : this->code[current+1];
}

void Token::ParseSingleToken() {
    char c = NextChrs();
    switch(c) {
        case '(':
            Add2Token(LEFT_PAREN);
            break;
        case ')':
            Add2Token(RIGHT_PAREN);
            break;
        case '{':
            Add2Token(LEFT_CR_BRACKET);
            break;
        case '}':
            Add2Token(RIGHT_CR_BRACKET);
            break;
        case '[':
            Add2Token(LEFT_SQ_BRACKET);
            break;
        case ']':
            Add2Token(RIGHT_SQ_BRACKET);
            break;
        case ',':
            Add2Token(COMMA);
            break;
        case ';':
            Add2Token(SEMICOLON);
            break;
        case ':':
            Add2Token(COLON);
            break;
        case '.':
            Add2Token(DOT);
            break;
        case '!':
            Add2Token(Match('=') ? NOT_EQUAL : EXCLA_MARK);
            break;
        case '=':
            Add2Token(Match('=') ? EQUAL_EQUAL : EQUAL);
            break;
        case '<':
            Add2Token(Match('<') ? SHIFT_LEFT : Match('=') ? LESS_EQUAL : LESS);
            break;
        case '>':
            Add2Token(Match('>') ? SHIFT_RIGHT : Match('=') ? GREATER_EQUAL : GREATER);
            break;
        case '+':
            Add2Token(PLUS);
            break;
        case '&':
            Add2Token(AND);
            break;
        case '^':
            Add2Token(XOR);
            break;
        case '~':
            Add2Token(NOT);
            break;
        case '|':
            Add2Token(OR);
            break;
        case '-':
            Add2Token(MINUS);
            break;
        case '*':
            Add2Token(STAR);
            break;
        case '/':
            Add2Token(SLASH);
            break;
        case '#':
            if (Match('#')) {
                while(!MatchStr("##") && !IsAtEnd()) {
                    if (CurrentChrs() == '\n') { line++; endcurrent = 0; } 
                    current++;
                }
                break;    
            }
            while(CurrentChrs() != '\n' && !IsAtEnd()) NextChrs();
            break;
        case '"':
            while(CurrentChrs() != '"' && !IsAtEnd()) {
                if (CurrentChrs() == '\n') { line++; endcurrent=0;}
                NextChrs();
            }
            if (IsAtEnd()) { 
                ErrorHandler::pError("Unterminated String", filename, line, endcurrent);
                return;
            }
            NextChrs();
            Add2Token(STRING,
            this->code.substr(start+1, current-start-2));
            break;
        case '\'':
            while(CurrentChrs() != '\'' && !IsAtEnd()) {
                if (CurrentChrs() == '\n') {
                    ErrorHandler::pError("Invaild character format", filename, line, endcurrent);
                    return;
                }
                NextChrs();
            }
            if (IsAtEnd()) { 
                ErrorHandler::pError("Unterminated Chars", filename, line, endcurrent);
                return;
            }
            NextChrs();
            Add2Token(CHAR,
            this->code.substr(start+1, current-start-2));

            break;
        case ' ':
        case '\r':
        case '\t':
            break;
        case '\n':
            line++;
            endcurrent=0;
            //Add2Token(SEMICOLON);
            break;
        default:
            if (isdigit(c)) {
                while(isdigit(CurrentChrs())) NextChrs();
                if (CurrentChrs() == '.' && isdigit(NextCurrentChrs())) {
                    NextChrs();
                    while(isdigit(CurrentChrs())) NextChrs();
                }
                Add2Token(NUMBER, this->code.substr(start, current-start));
            } else if (isalpha(c) || c == '_') {
                while(isalnum(CurrentChrs()) || CurrentChrs() == '_') NextChrs();
                TokenType type=IDENTIFIER;
                for (string x : Keywords) {
                    if (x == this->code.substr(start, current-start)) { type = KEYWORDS; break; }
                }
                Add2Token(type);
            }
            else 
                ErrorHandler::pError("Unknown Token",filename,line, endcurrent);
            break;
    }
}

void Token::ParseLexer() {
    while (! IsAtEnd()) { start = current; ParseSingleToken(); }

    Add2Token(eof);
}

vector<TokenData> Token::ReturnLexer() {
    return this->token;
}