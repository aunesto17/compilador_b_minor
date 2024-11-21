#ifndef LEXER_H_
#define LEXER_H_

#include <iostream>
#include <fstream> 
#include <string>
#include <unordered_map>
#include <optional>
#include <cctype>
#include <stdlib.h>

#include "helper.h"

// Clase del analizador léxico (lexer)
class Lexer {
    const SourceFile *source;
    size_t idx = 0;
    int line = 1;
    int column = 0;
    unsigned int errorCount = 0;
    std::vector<Token> tokens;

public:
    explicit Lexer(const SourceFile &source) : source(&source) {
        tokenize();
        //std::cout << "DEBUG LEXER - Tokens generated: " << tokens.size() << std::endl;
        // print all tokens
        for(auto &t : tokens) {
            //std::cout << "DEBUG LEXER - Token: " << t.kindToString() << " [ " << t.value.value_or("") << " ] found at (" << t.location.line << ":" << t.location.col << ")\n";
        }

    }

    void printTokens() {
        for (const auto &token : tokens) {
            std::cout << "Token: " << token.kindToString() << " [ " << token.value.value_or("") << " ] found at (" << token.location.line << ":" << token.location.col << ")\n";
        }
    }

    unsigned int getErrorCount() const {
        return errorCount;
    }

    char peekNextChar() const {
    // Check if we're at the end of the buffer
        if (idx >= source->buffer.size()) {
            std::cout << "DEBUG LEXER - Reached EOF while peeking at index: " << idx << std::endl;
            return EOF;  // Return EOF to indicate end of file
        }
        return source->buffer[idx];
    }

    char eatNextChar() {
        // Check if we're at the end of the buffer
        if (idx >= source->buffer.size()) {
            //std::cout << "DEBUG LEXER - Attempted to eat character at EOF, index: " << idx << std::endl;
            return EOF;  // Return EOF to indicate end of file
        }

        char currentChar = source->buffer[idx++];
        ++column;
        if (currentChar == '\n') {
            ++line;
            column = 0;
        }

        //std::cout << "DEBUG LEXER - Consumed character: '" << currentChar << "' at index: " << idx - 1 << " (line: " << line << ", column: " << column << ")" << std::endl;
        return currentChar;
    }

    // insert token in front of the vector
    void insertToken(Token token) {
        tokens.insert(tokens.begin(), token);
    }

    void tokenize() {
        Token token = getNextToken();
        while (token.kind != TokenKind::Eof) {
            tokens.push_back(token);
            token = getNextToken();
        }
        // push back the EOF token
        tokens.push_back(token);
    }

    Token peekToken() {
        if (tokens.empty()) {
            return Token{source->path, line, column, TokenKind::Eof};
        }
        return tokens.front(); // Devolver el siguiente token sin eliminarlo de la lista
    }

    void ungetToken(Token token) {
        tokens.insert(tokens.begin(), token); // Insertar el token al principio de la lista para "devolverlo"
    }   
    //get tokens in order
    Token getToken() {
        if (tokens.empty()) {
            return Token{source->path, line, column, TokenKind::Eof};
        }
        Token token = tokens.front();
        tokens.erase(tokens.begin());
        return token;
    }
    
    
    
    Token getNextToken() {
    char currentChar = eatNextChar();

    // Check for EOF immediately
    if (currentChar == EOF) {
        return Token{SourceLocation{source->path, line, column}, TokenKind::Eof};
    }

    // Ignorar espacios en blanco
    while (std::isspace(currentChar)) {
        currentChar = eatNextChar();
        if (currentChar == EOF) {
            return Token{SourceLocation{source->path, line, column}, TokenKind::Eof};
        }
    }

    SourceLocation tokenStartLocation{source->path, line, column};

    // Ignorar comentarios // y /* */
    if (currentChar == '/') {
        if (peekNextChar() == '/') {
            // Ignorar el comentario de una línea
            while (peekNextChar() != '\n' && peekNextChar() != EOF) {
                eatNextChar();
            }
            return getNextToken();  // Continuar después del comentario de línea
        } else if (peekNextChar() == '*') {
            eatNextChar();  // Consumir el asterisco '*'
            while (true) {
                if (peekNextChar() == EOF) {
                    errorCount++;
                    return Token{tokenStartLocation, TokenKind::Unknown};
                }
                char nextChar = eatNextChar();
                if (nextChar == '*' && peekNextChar() == '/') {
                    eatNextChar();  // Consumir la barra '/'
                    break;  // Salir del comentario de bloque correctamente cerrado
                }
            }
            return getNextToken();  // Continuar después del comentario de bloque
        }
    }

    // Revisar si es un token de un solo carácter
    switch (currentChar) {
        case ':': return Token{tokenStartLocation, TokenKind::ColonSymbol};
        case ';': return Token{tokenStartLocation, TokenKind::SemiColonSymbol};
        case ',': return Token{tokenStartLocation, TokenKind::CommaSymbol};
        case '{': return Token{tokenStartLocation, TokenKind::LeftBrace};
        case '}': return Token{tokenStartLocation, TokenKind::RightBrace};
        case '[': return Token{tokenStartLocation, TokenKind::LeftBracket};
        case ']': return Token{tokenStartLocation, TokenKind::RightBracket};
        case '(': return Token{tokenStartLocation, TokenKind::LeftParenthesis};
        case ')': return Token{tokenStartLocation, TokenKind::RightParenthesis};
        case '+':
            if (peekNextChar() == '+') {
                eatNextChar();
                return Token{tokenStartLocation, TokenKind::PostfixIncrement};
            }
            return Token{tokenStartLocation, TokenKind::Addition};
        case '-':
            if (peekNextChar() == '-') {
                eatNextChar();
                return Token{tokenStartLocation, TokenKind::PostfixDecrement};
            }
            return Token{tokenStartLocation, TokenKind::Subtraction};
        case '!':
            if (peekNextChar() == '=') {
                eatNextChar();
                return Token{tokenStartLocation, TokenKind::NotEqual};
            }
            return Token{tokenStartLocation, TokenKind::LogicalNot};
        case '&':
            if (peekNextChar() == '&') {
                eatNextChar();
                return Token{tokenStartLocation, TokenKind::LogicalAnd};
            }
            return Token{tokenStartLocation, TokenKind::Unknown};
        case '|':
            if (peekNextChar() == '|') {
                eatNextChar();
                return Token{tokenStartLocation, TokenKind::LogicalOr};
            }
            return Token{tokenStartLocation, TokenKind::Unknown};
        case '^': return Token{tokenStartLocation, TokenKind::Exponentiation};
        case '*': return Token{tokenStartLocation, TokenKind::Multiplication};
        case '/': return Token{tokenStartLocation, TokenKind::Division};
        case '%': return Token{tokenStartLocation, TokenKind::Modulus};
        case '<':
            if (peekNextChar() == '=') {
                eatNextChar();
                return Token{tokenStartLocation, TokenKind::LessThanOrEqual};
            }
            return Token{tokenStartLocation, TokenKind::LessThan};
        case '>':
            if (peekNextChar() == '=') {
                eatNextChar();
                return Token{tokenStartLocation, TokenKind::GreaterThanOrEqual};
            }
            return Token{tokenStartLocation, TokenKind::GreaterThan};
        case '=':
            if (peekNextChar() == '=') {
                eatNextChar();
                return Token{tokenStartLocation, TokenKind::isEqual};
            }
            return Token{tokenStartLocation, TokenKind::Assign};
        case '$': return Token{tokenStartLocation, TokenKind::Eof}; // Fin del programa
    }

    // Identificar cadenas
    if (currentChar == '"') {
        std::string value;
        while (peekNextChar() != '"') {
            if (peekNextChar() == EOF) {
                errorCount++;
                return Token{tokenStartLocation, TokenKind::Unknown};
            }
            value += eatNextChar();
        }
        eatNextChar();  // Consumir la comilla final
        return Token{tokenStartLocation, TokenKind::StringVal, std::move(value)};
    }

    // Identificar caracteres
    if (currentChar == '\'') {
        std::string value;
        if (peekNextChar() == '\\') {
            value += eatNextChar();  // Consumir la barra invertida
        }
        value += eatNextChar();  // Consumir el caracter
        if (peekNextChar() != '\'') {
            errorCount++;
            return Token{tokenStartLocation, TokenKind::Unknown};
        }
        eatNextChar();  // Consumir la comilla final
        return Token{tokenStartLocation, TokenKind::CharVal, std::move(value)};
    }

    // Identificar números
    if (std::isdigit(currentChar)) {
        std::string value{currentChar}; // Añadir el primer dígito
        while (std::isdigit(peekNextChar())) {
            value += eatNextChar(); // Acumular los dígitos
        }
        const char *c = value.c_str(); 
        long long ll = std::strtoll(c, nullptr, 10);
        if (ll > 9223372036854775807LL || ll < -9223372036854775807LL) {
            std::cerr << "ERROR LEXICO - Numero fuera de rango" << std::endl;
            errorCount++;
            return Token{tokenStartLocation, TokenKind::Unknown};
        }
        return Token{tokenStartLocation, TokenKind::Number, std::move(value)};
    }

    // Identificar identificadores o palabras clave
    if (std::isalpha(currentChar) || currentChar == '_') {
        std::string value{currentChar};
        while (std::isalnum(peekNextChar()) || peekNextChar() == '_') {
            value += eatNextChar();
        }

        // Verificar si es una palabra clave
        if (auto it = keywords.find(value); it != keywords.end()) {
            return Token{tokenStartLocation, it->second};
        }

        return Token{tokenStartLocation, TokenKind::Identifier, std::move(value)};
    }

    // Si llegamos aquí, no se encontró un token válido
    return Token{tokenStartLocation, TokenKind::Unknown};
}

    
};

#endif // LEXER_H_