#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <optional>
#include <cctype>
#include <stdlib.h>


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
    LogicalNot,
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
            std::cout << "DEBUG LEXER - String detected" << std::endl;	
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

int main() {
    // Leer archivo de entrada
    std::ifstream file("prueba.txt"); // Asegúrate de que el archivo exista
    if (!file.is_open()) {
        std::cerr << "No se pudo abrir el archivo 'codigo_fuente.txt'." << std::endl;
        return 1;
    }

    // Leer el contenido del archivo en un buffer
    std::string buffer((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    file.close();

    // Instanciar el archivo fuente con el contenido del archivo .txt
    SourceFile sourceFile{"codigo_fuente.txt", buffer};

    Lexer lexer(sourceFile);
    Token token;

    std::cout << "INFO SCAN - Start scanning...\n";
    while (true) {
        token = lexer.getNextToken();
        if (token.kind == TokenKind::Eof) break;
        //else if(token.kind == TokenKind::Unknown) {}

        std::string tokenType;
        std::string symbol;

        // Asegúrate de que los tokens que uses aquí están definidos en TokenKind
        switch (token.kind) {
            case TokenKind::KwArray: 
                tokenType = "KW_ARRAY"; 
                symbol = "array";
                break;
            case TokenKind::KwBoolean:
                tokenType = "KW_BOOLEAN"; 
                symbol = "boolean";
                break;
            case TokenKind::KwChar:
                tokenType = "KW_CHAR"; 
                symbol = "char";
                break;
            case TokenKind::KwElse:
                tokenType = "KW_ELSE"; 
                symbol = "else";
                break;
            case TokenKind::KwFalse:
                tokenType = "KW_FALSE"; 
                symbol = "false";
                break;
            case TokenKind::KwFor:
                tokenType = "KW_FOR"; 
                symbol = "for";
                break;
            case TokenKind::KwFunction:
                tokenType = "KW_FUNCTION"; 
                symbol = "function";
                break;
            case TokenKind::KwIf:
                tokenType = "KW_IF"; 
                symbol = "if";
                break;
            case TokenKind::KwInteger:
                tokenType = "KW_INTEGER"; 
                symbol = "integer";
                break;
            case TokenKind::KwPrint:    
                tokenType = "KW_PRINT"; 
                symbol = "print";
                break;
            case TokenKind::KwReturn:
                tokenType = "KW_RETURN"; 
                symbol = "return";
                break;
            case TokenKind::KwString:
                tokenType = "KW_STRING"; 
                symbol = "string";
                break;
            case TokenKind::KwTrue:
                tokenType = "KW_TRUE"; 
                symbol = "true";
                break;
            case TokenKind::KwVoid:
                tokenType = "KW_VOID"; 
                symbol = "void";
                break;
            case TokenKind::KwWhile:
                tokenType = "KW_WHILE"; 
                symbol = "while";
                break;

            case TokenKind::Number: 
                tokenType = "INT"; 
                symbol = token.value.has_value() ? *token.value : "[Número]";
                break;
            case TokenKind::Identifier: 
                tokenType = "ID"; 
                symbol = token.value.has_value() ? *token.value : "[ID]";
                break;
            case TokenKind::StringVal:
                tokenType = "STRING"; 
                symbol = token.value.has_value() ? *token.value : "[Cadena]";
                break;
            case TokenKind::CharVal:
                tokenType = "CHAR"; 
                symbol = token.value.has_value() ? *token.value : "[Caracter]";
                break;

            case TokenKind::ColonSymbol: 
                tokenType = "COLON"; 
                symbol = ":";
                break;
            case TokenKind::SemiColonSymbol:
                tokenType = "SEMICOLON"; 
                symbol = ";";
                break;
            case TokenKind::CommaSymbol:
                tokenType = "COMMA"; 
                symbol = ",";
                break;
            case TokenKind::LeftBracket:
                tokenType = "OPEN_BRACKET"; 
                symbol = "[";
                break;
            case TokenKind::RightBracket:
                tokenType = "CLOSE_BRACKET"; 
                symbol = "]";
                break;
            case TokenKind::LeftBrace:
                tokenType = "OPEN_BRACE"; 
                symbol = "{";
                break;
            case TokenKind::RightBrace:
                tokenType = "CLOSE_BRACE"; 
                symbol = "}";
                break;
            case TokenKind::LeftParenthesis: 
                tokenType = "OPEN_PAR"; 
                symbol = "(";
                break;
            case TokenKind::RightParenthesis: 
                tokenType = "CLOSE_PAR"; 
                symbol = ")";
                break;
            case TokenKind::PostfixIncrement:
                tokenType = "POSTFIX_INC"; 
                symbol = "++";
                break;
            case TokenKind::PostfixDecrement:
                tokenType = "POSTFIX_DEC"; 
                symbol = "--";
                break;
            case TokenKind::LogicalNot:
                tokenType = "LOGICAL_NOT"; 
                symbol = "!";
                break;
            case TokenKind::Exponentiation:
                tokenType = "EXP_OP"; 
                symbol = "^";
                break;
            case TokenKind::Multiplication:
                tokenType = "MULT_OP"; 
                symbol = "*";
                break;
            case TokenKind::Division:
                tokenType = "DIV_OP"; 
                symbol = "/";
                break;
            case TokenKind::Modulus:
                tokenType = "MOD_OP"; 
                symbol = "%";
                break;
            case TokenKind::Addition:
                tokenType = "ADD_OP"; 
                symbol = "+";
                break;
            case TokenKind::Subtraction:    
                tokenType = "SUB_OP"; 
                symbol = "-";
                break;
            case TokenKind::LessThan:  
                tokenType = "LESS_THAN"; 
                symbol = "<";
                break;
            case TokenKind::LessThanOrEqual:
                tokenType = "LESS_THAN_OR_EQUAL"; 
                symbol = "<=";
                break;
            case TokenKind::GreaterThan:
                tokenType = "GREATER_THAN"; 
                symbol = ">";
                break;
            case TokenKind::GreaterThanOrEqual:
                tokenType = "GREATER_THAN_OR_EQUAL"; 
                symbol = ">=";
                break;
            case TokenKind::isEqual:
                tokenType = "EQUAL"; 
                symbol = "==";
                break;
            case TokenKind::NotEqual:
                tokenType = "NOT_EQUAL"; 
                symbol = "!=";
                break; 
            case TokenKind::Assign: 
                tokenType = "ASSIGN_OP"; 
                symbol = "=";
                break;
            case TokenKind::Eof:
                tokenType = "EOF"; 
                symbol = "$";
                break;

            default: 
                tokenType = "Token desconocido"; 
                symbol = "";
        }

        std::cout << "DEBUG SCAN - " << tokenType << " [ " 
                  << symbol << " ] found at (" << token.location.line 
                  << ":" << token.location.col << ")\n";
    }

    std::cout << "INFO SCAN - Completed with " << lexer.getErrorCount() << " error(s)\n";
    return 0;
}
