#ifndef HELPER_H_
#define HELPER_H_

#include <iostream>
#include <unordered_map>
#include <optional>

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
    {"boolean", TokenKind::KwBoolean},
    {"char", TokenKind::KwChar},
    {"else", TokenKind::KwElse},
    {"false", TokenKind::KwFalse},
    {"for", TokenKind::KwFor},
    {"function", TokenKind::KwFunction},
    {"if", TokenKind::KwIf},
    {"integer", TokenKind::KwInteger},
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
};

// Estructura del token
struct Token {
    SourceLocation location;
    TokenKind kind;
    std::optional<std::string> value = std::nullopt;
};

// Estructura del archivo fuente
struct SourceFile {
    std::string_view path;
    std::string buffer;
};


#endif // HELPER_H_