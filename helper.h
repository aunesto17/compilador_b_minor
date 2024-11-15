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
    std::string_view kindToString() const {
        switch (kind) {
            case TokenKind::KwArray: return "KW_ARRAY "+value.value_or("");
            case TokenKind::KwBoolean: return "KW_BOOLEAN "+value.value_or("");
            case TokenKind::KwChar: return "KW_CHAR "+value.value_or("");
            case TokenKind::KwElse: return "KW_ELSE "+value.value_or("");
            case TokenKind::KwFalse: return "KW_FALSE "+value.value_or("");
            case TokenKind::KwFor: return "KW_FOR "+value.value_or("");
            case TokenKind::KwFunction: return "KW_FUNCTION "+value.value_or("");
            case TokenKind::KwIf: return "KW_IF "+value.value_or("");
            case TokenKind::KwInteger: return "KW_INTEGER "+value.value_or("");
            case TokenKind::KwPrint: return "KW_PRINT "+value.value_or("");
            case TokenKind::KwReturn: return "KW_RETURN "+value.value_or("");
            case TokenKind::KwString: return "KW_STRING "+value.value_or("");
            case TokenKind::KwTrue: return "KW_TRUE "+value.value_or("");
            case TokenKind::KwVoid: return "KW_VOID "+value.value_or("");
            case TokenKind::KwWhile: return "KW_WHILE "+value.value_or("");
            
            case TokenKind::Identifier: return "ID "+value.value_or("");
            case TokenKind::Number: return "INT_LIT "+value.value_or("");
            case TokenKind::StringVal: return "STRING_LIT "+value.value_or("");
            case TokenKind::CharVal: return "CHAR_LIT "+value.value_or("");

            case TokenKind::ColonSymbol: return "COLON"+value.value_or("");
            case TokenKind::SemiColonSymbol: return "SEMICOLON"+value.value_or("");
            case TokenKind::CommaSymbol: return "COMMA"+value.value_or("");
            case TokenKind::LeftBracket: return "OPEN_BRACKET"+value.value_or("");
            case TokenKind::RightBracket: return "CLOSE_BRACKET"+value.value_or("");
            case TokenKind::LeftBrace: return "OPEN_BRACE"+value.value_or("");
            case TokenKind::RightBrace: return "CLOSE_BRACE"+value.value_or("");
            case TokenKind::LeftParenthesis: return "OPEN_PAR"+value.value_or("");
            case TokenKind::RightParenthesis: return "CLOSE_PAR"+value.value_or("");
            case TokenKind::PostfixIncrement: return "POSTFIX_INCREMENT"+value.value_or("");
            case TokenKind::PostfixDecrement: return "POSTFIX_DECREMENT"+value.value_or("");
            case TokenKind::LogicalNot: return "LOGICAL_NOT"+value.value_or("");
            case TokenKind::LogicalAnd: return "LOGICAL_AND"+value.value_or("");
            case TokenKind::LogicalOr: return "LOGICAL_OR"+value.value_or("");
            case TokenKind::Exponentiation: return "EXPONENTIATION"+value.value_or("");
            case TokenKind::Multiplication: return "MULT_OP"+value.value_or("");
            case TokenKind::Division: return "DIV_OP"+value.value_or("");
            case TokenKind::Modulus: return "MOD_OP"+value.value_or("");
            case TokenKind::Addition: return "ADD_OP"+value.value_or("");
            case TokenKind::Subtraction: return "SUB_OP"+value.value_or("");
            case TokenKind::LessThan: return "LESS_THAN"+value.value_or("");
            case TokenKind::LessThanOrEqual: return "LESS_THAN_OR_EQUAL"+value.value_or("");
            case TokenKind::GreaterThan: return "GREATER_THAN"+value.value_or("");
            case TokenKind::GreaterThanOrEqual: return "GREATER_THAN_OR_EQUAL"+value.value_or("");
            case TokenKind::isEqual: return "EQUAL"+value.value_or("");
            case TokenKind::NotEqual: return "NOT_EQUAL"+value.value_or("");
            case TokenKind::Assign: return "ASSIGN"+value.value_or("");
            case TokenKind::Eof: return "EOF"+value.value_or("");
            case TokenKind::Unknown: return "UNKNOWN"+value.value_or("");
        
        }
    }
};

// Estructura del archivo fuente
struct SourceFile {
    std::string_view path;
    std::string buffer;
};


#endif // HELPER_H_