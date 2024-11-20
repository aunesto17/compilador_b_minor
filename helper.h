#ifndef HELPER_H_
#define HELPER_H_ 

#include <iostream>
#include <unordered_map>
#include <optional>
#include <string>
#include <set>
#include <vector>

using namespace std;

// Definición de los tipos de tokens
enum class TokenKind : char {
    KwArray, KwBoolean, KwChar, KwElse, KwFalse, KwFor, KwFunction, 
    KwIf, KwInteger, KwPrint, KwReturn, KwString, KwTrue, KwVoid, KwWhile,

    Identifier, 
    Number,
    StringVal,
    CharVal,
    
    ColonSymbol, SemiColonSymbol, CommaSymbol,
    LeftBracket, RightBracket,
    LeftBrace, RightBrace, 
    LeftParenthesis, RightParenthesis, 
    PostfixIncrement, PostfixDecrement, 
    LogicalNot, LogicalAnd, LogicalOr,
    Exponentiation,
    Multiplication, Division, Modulus,
    Addition, Subtraction,
    LessThan, LessThanOrEqual, GreaterThan, GreaterThanOrEqual, isEqual, NotEqual,
    Assign,
    Eof, Unknown
};

// Mapa de palabras clave
const std::unordered_map<std::string_view, TokenKind> keywords = {
    {"array", TokenKind::KwArray},
    {"bool", TokenKind::KwBoolean},
    {"char", TokenKind::KwChar},
    {"else", TokenKind::KwElse},
    {"false", TokenKind::KwFalse},
    {"for", TokenKind::KwFor},
    {"function", TokenKind::KwFunction},
    {"if", TokenKind::KwIf},
    {"int", TokenKind::KwInteger},
    {"print", TokenKind::KwPrint},
    {"return", TokenKind::KwReturn},
    {"string", TokenKind::KwString},
    {"true", TokenKind::KwTrue},
    {"void", TokenKind::KwVoid},
    {"while", TokenKind::KwWhile}
};

// Estructura para la ubicación del token en el archivo
struct SourceLocation {
    std::string_view filepath;
    int line;
    int col;

    // Imprimir ubicación del token
    std::string printLoc() {
        return "line: " + to_string(line) + " col: " + to_string(col);
    }
};

// Estructura del token
struct Token {
    SourceLocation location;
    TokenKind kind;
    std::optional<std::string> value = std::nullopt;

    // token vacio
    //Token() : kind{TokenKind::Unknown} {}

    // retornar tipo de token
    std::string kindToString() const {
    switch (kind) {
        case TokenKind::KwArray: return "KW_ARRAY";
        case TokenKind::KwBoolean: return "KW_BOOLEAN";
        case TokenKind::KwChar: return "KW_CHAR";
        case TokenKind::KwElse: return "KW_ELSE";
        case TokenKind::KwFalse: return "KW_FALSE";
        case TokenKind::KwFor: return "KW_FOR";
        case TokenKind::KwFunction: return "KW_FUNCTION";
        case TokenKind::KwIf: return "KW_IF";
        case TokenKind::KwInteger: return "KW_INTEGER";
        case TokenKind::KwPrint: return "KW_PRINT";
        case TokenKind::KwReturn: return "KW_RETURN";
        case TokenKind::KwString: return "KW_STRING";
        case TokenKind::KwTrue: return "KW_TRUE";
        case TokenKind::KwVoid: return "KW_VOID";
        case TokenKind::KwWhile: return "KW_WHILE";

        case TokenKind::Identifier: return "ID " + value.value_or("");
        case TokenKind::Number: return "INT_LIT " + value.value_or("");
        case TokenKind::StringVal: return "STRING_LIT " + value.value_or("");
        case TokenKind::CharVal: return "CHAR_LIT " + value.value_or("");

        case TokenKind::ColonSymbol: return "COLON";
        case TokenKind::SemiColonSymbol: return "SEMICOLON";
        case TokenKind::CommaSymbol: return "COMMA";
        case TokenKind::LeftBracket: return "OPEN_BRACKET";
        case TokenKind::RightBracket: return "CLOSE_BRACKET";
        case TokenKind::LeftBrace: return "OPEN_BRACE";
        case TokenKind::RightBrace: return "CLOSE_BRACE";
        case TokenKind::LeftParenthesis: return "OPEN_PAR";
        case TokenKind::RightParenthesis: return "CLOSE_PAR";
        case TokenKind::PostfixIncrement: return "POSTFIX_INCREMENT";
        case TokenKind::PostfixDecrement: return "POSTFIX_DECREMENT";
        case TokenKind::LogicalNot: return "LOGICAL_NOT";
        case TokenKind::LogicalAnd: return "LOGICAL_AND";
        case TokenKind::LogicalOr: return "LOGICAL_OR";
        case TokenKind::Exponentiation: return "EXPONENTIATION";
        case TokenKind::Multiplication: return "MULT_OP";
        case TokenKind::Division: return "DIV_OP";
        case TokenKind::Modulus: return "MOD_OP";
        case TokenKind::Addition: return "ADD_OP";
        case TokenKind::Subtraction: return "SUB_OP";
        case TokenKind::LessThan: return "LESS_THAN";
        case TokenKind::LessThanOrEqual: return "LESS_THAN_OR_EQUAL";
        case TokenKind::GreaterThan: return "GREATER_THAN";
        case TokenKind::GreaterThanOrEqual: return "GREATER_THAN_OR_EQUAL";
        case TokenKind::isEqual: return "EQUAL";
        case TokenKind::NotEqual: return "NOT_EQUAL";
        case TokenKind::Assign: return "ASSIGN";
        case TokenKind::Eof: return "EOF";
        case TokenKind::Unknown: return "UNKNOWN";
        default: return "UNRECOGNIZED TOKEN";
    }
}

};

// Estructura del archivo fuente
struct SourceFile {
    std::string_view path;
    std::string buffer;
};


#endif // HELPER_H_