#ifndef LEXER_H_
#define LEXER_H_

#include "helper.h"

// Clase del analizador léxico (lexer)
class Lexer {
    const SourceFile *source;
    size_t idx = 0;
    int line = 1;
    int column = 0;
    unsigned int errorCount = 0;

public:
    explicit Lexer(const SourceFile &source) : source(&source) {}

    unsigned int getErrorCount() const {
        return errorCount;
    }

    char peekNextChar() const {
        return source->buffer[idx];
    }

    char eatNextChar() {
        char currentChar = source->buffer[idx++];
        ++column;
        if (currentChar == '\n') {
            ++line;
            column = 0;
        }
        //std::cout << "DEBUG LEXER - Current char: " << currentChar << " l:" << line << " c:" << column << std::endl;	
        return currentChar;
    }

    Token getNextToken() {
        char currentChar = eatNextChar();

        // Ignorar espacios en blanco
        while (std::isspace(currentChar)) 
            currentChar = eatNextChar();
    
        SourceLocation tokenStartLocation{source->path, line, column};

        // Detectar el fin del archivo
        if (currentChar == '\0') {
            return Token{tokenStartLocation, TokenKind::Eof};
        }

        // Ignorar comentarios // y /* */ 
        if (currentChar == '/') {
            if (peekNextChar() == '/') {
                while (peekNextChar() != '\n' && peekNextChar() != '\0') {
                    eatNextChar();  // Ignorar el comentario hasta el final de la línea
                }
                return getNextToken();  // Continuar después del comentario
            } else if (peekNextChar() == '*') {
                eatNextChar();  // Consumir el asterisco
                while (peekNextChar() != '*' && peekNextChar() != '\n' && peekNextChar() != '\0') {
                    eatNextChar();  // Ignorar el comentario hasta encontrar un asterisco
                }
                if (peekNextChar() == '\0' || peekNextChar() == '\n') {
                        std::cerr << "ERROR LEXICO - Comentario no cerrado" << std::endl;
                        errorCount++;
                        return Token{tokenStartLocation, TokenKind::Unknown};
                    }
                else if (eatNextChar() == '*' && peekNextChar() == '/') {
                    eatNextChar();  // Consumir la barra
                }   
                return getNextToken();  // Continuar después del comentario
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
                if(peekNextChar() == '+') {
                    eatNextChar();
                    return Token{tokenStartLocation, TokenKind::PostfixIncrement};
                }
                return Token{tokenStartLocation, TokenKind::Addition};
            case '-':
                if(peekNextChar() == '-') {
                    eatNextChar();
                    return Token{tokenStartLocation, TokenKind::PostfixDecrement};
                }
                return Token{tokenStartLocation, TokenKind::Subtraction};
            case '!':
                if(peekNextChar() == '=') {
                    eatNextChar();
                    return Token{tokenStartLocation, TokenKind::NotEqual};
                }
                return Token{tokenStartLocation, TokenKind::LogicalNot};
            case '&':
                if(peekNextChar() == '&') {
                    eatNextChar();
                    return Token{tokenStartLocation, TokenKind::LogicalAnd};
                }
                return Token{tokenStartLocation, TokenKind::Unknown};
            case '|':
                if(peekNextChar() == '|') {
                    eatNextChar();
                    return Token{tokenStartLocation, TokenKind::LogicalOr};
                }
                return Token{tokenStartLocation, TokenKind::Unknown};
            case '^': return Token{tokenStartLocation, TokenKind::Exponentiation};
            case '*': return Token{tokenStartLocation, TokenKind::Multiplication};
            case '/': return Token{tokenStartLocation, TokenKind::Division};
            case '%': return Token{tokenStartLocation, TokenKind::Modulus};
            case '<':
                if(peekNextChar() == '=') {
                    eatNextChar();
                    return Token{tokenStartLocation, TokenKind::LessThanOrEqual};
                }
                return Token{tokenStartLocation, TokenKind::LessThan};
            case '>':
                if(peekNextChar() == '=') {
                    eatNextChar();
                    return Token{tokenStartLocation, TokenKind::GreaterThanOrEqual};
                }
                return Token{tokenStartLocation, TokenKind::GreaterThan};
            case '=':
                if(peekNextChar() == '=') {
                    eatNextChar();
                    return Token{tokenStartLocation, TokenKind::isEqual};
                }
                return Token{tokenStartLocation, TokenKind::Assign};
            case '$': return Token{tokenStartLocation, TokenKind::Eof}; // Fin del programa
        }


        // Identificar cadenas
        if (currentChar == '"') {
            //std::cout << "DEBUG LEXER - String detected" << std::endl;	
            std::string value;
            while (peekNextChar() != '"') {
                if (peekNextChar() == '\0' || peekNextChar() == '\n') {
                    std::cerr << "ERROR LEXICO - Cadena no cerrada" << std::endl;
                    errorCount++;
                    return Token{tokenStartLocation, TokenKind::Unknown};
                }
                value += eatNextChar();
                //std::cout << value << std::endl;
            }
            eatNextChar();  // Consumir la comilla final
            return Token{tokenStartLocation, TokenKind::StringVal, std::move(value)};
        }

        // Identificar caracteres
        if (currentChar == '\'') {
            //std::cout << "DEBUG LEXER - Char detected" << std::endl;	
            std::string value;
            if (peekNextChar() == '\\') {
                value += eatNextChar();  // Consumir la barra invertida
            }
            value += eatNextChar();  // Consumir el caracter
            if (peekNextChar() != '\'') {
                std::cerr << "ERROR LEXICO - Caracter no cerrado" << std::endl;
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
            const char * c = value.c_str(); 
            long long ll = std::strtoll(c, nullptr, sizeof(value));
            if(ll > 922337203685475807 || ll < -9223372036854775808) {
                std::cerr << "ERROR LEXICO - Numero fuera de rango" << std::endl;
                errorCount++;
                return Token{tokenStartLocation, TokenKind::Unknown};
            }
            return Token{tokenStartLocation, TokenKind::Number, std::move(value)};
        }

        // Identificar identificadores o palabras clave
        if (std::isalpha(currentChar) || currentChar == '_') {
            std::string value{currentChar};
            while (std::isalnum(peekNextChar()) || peekNextChar() == '_')
                value += eatNextChar();

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