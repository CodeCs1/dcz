#pragma once

#include <string>
#include <vector>
enum TokenType {
    NUMBER,
    STRING,
    CHAR,
    IDENTIFIER,
    KEYWORDS,

    PLUS,
    PLUS_EQUAL,
    MINUS,
    MINUS_EQUAL,
    STAR,
    STAR_EQUAL,
    SLASH,
    SLASH_EQUAL,

    SHIFT_LEFT,
    SHIFT_RIGHT,
    AND,
    OR,
    XOR,
    NOT, // ~
    
    COMMA,
    DOT,
    SEMICOLON,
    COLON,
    LEFT_PAREN,
    RIGHT_PAREN,
    LEFT_SQ_BRACKET,
    RIGHT_SQ_BRACKET,
    LEFT_CR_BRACKET,
    RIGHT_CR_BRACKET,

    EXCLA_MARK,
    NOT_EQUAL,
    EQUAL,
    EQUAL_EQUAL,
    GREATER,
    GREATER_EQUAL,
    LESS,
    LESS_EQUAL,
    eof
};

struct TokenData {
    std::string token;
    std::string literal;
    TokenType type;
    int line;
};

class Token {
    protected:
        std::string code;
        std::string filename="stdin";
        std::vector<TokenData> token;
        bool IsAtEnd();
        bool Match(char c);
        void ParseSingleToken();
        char NextChrs();
        char NextCurrentChrs();
        char CurrentChrs();
        void Add2Token(TokenType type, std::string str);
        void Add2Token(TokenType type);
        bool MatchStr(std::string str);
    public:
        Token(std::string code);
        Token(std::string filename, std::string code);
        void ParseLexer();
        std::vector<TokenData> ReturnLexer();
        
};