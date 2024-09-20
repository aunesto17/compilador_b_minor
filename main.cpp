#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <optional>
#include <cctype>

// Definición de los tipos de tokens
enum class TokenKind : char {
    Identifier, FunctionKeyword, VoidKeyword, IntKeyword, BooleanKeyword, 
    CharKeyword, StringKeyword, Number, LeftParenthesis, RightParenthesis, 
    LeftBrace, RightBrace, ColonSymbol, String, Eof, Unknown, 
    AssignOp, AddOp, SubOp, MulOp, DivOp, ModOp, Semicolon, 
    Char, PrintKeyword, Eop, GreaterOp,  GreaterEqualOp, LessOp, LessEqualOp,
    LogicalAndOp, LogicalOrOp, NotEqualOp,NotOp,
};

// Mapa de palabras clave
const std::unordered_map<std::string_view, TokenKind> keywords = {
    {"fn", TokenKind::FunctionKeyword}, {"void", TokenKind::VoidKeyword}, 
    {"integer", TokenKind::IntKeyword}, {"boolean", TokenKind::BooleanKeyword}, 
    {"char", TokenKind::CharKeyword}, {"string", TokenKind::StringKeyword}, 
    {"print", TokenKind::PrintKeyword}
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
    const SourceFile* source;
    size_t idx = 0;
    int line = 1;
    int column = 0;

public:
    explicit Lexer(const SourceFile& source) : source(&source) {}

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
        return currentChar;
    }

    bool isEndOfFile() {
        return idx >= source->buffer.size(); // O la forma en que manejas el tamaño del buffer
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


        if (currentChar == '\"') {
            std::string value = "";
            currentChar = eatNextChar(); // Avanzar después de la primera comilla

            while (currentChar != '\"' && currentChar != '\0') { // '\0' representa el final de archivo en muchos sistemas
                value += currentChar;
                currentChar = eatNextChar();
            }

            if (currentChar == '\"') {
                eatNextChar(); // Consumir la comilla de cierre
                return Token{tokenStartLocation, TokenKind::StringKeyword, value};
            } else {
                // Manejar error de cadena no cerrada
                return Token{tokenStartLocation, TokenKind::Unknown, "Error: cadena no cerrada"};
            }
        }


        // Ignorar comentarios multilínea
        if (currentChar == '/' && peekNextChar() == '*') {
            //std::cout << "DEBUG: Iniciando comentario multilínea" << std::endl;
            eatNextChar(); // Consumir '*'
            eatNextChar(); // Consumir el primer char después de '/*'

            // Continuar hasta encontrar el cierre '*/' o fin de archivo
            while (!(currentChar == '*' && peekNextChar() == '/') && currentChar != '\0') {
                //std::cout << "DEBUG: Consumir dentro de comentario multilínea. currentChar: " << currentChar << std::endl;
                currentChar = eatNextChar();
            }

            if (currentChar == '*' && peekNextChar() == '/') {
                //std::cout << "DEBUG: Fin de comentario multilínea" << std::endl;
                eatNextChar(); // Consumir '*'
                eatNextChar(); // Consumir '/'
            }

            // Consumir espacios después del comentario
            while (std::isspace(currentChar)) {
                currentChar = eatNextChar();
            }

            std::cout << "DEBUG: Carácter después del comentario multilínea: " << currentChar << "\n";

            // Llamar nuevamente a getNextToken para continuar el escaneo
            return getNextToken();
        }

        // Ignorar comentarios de una línea
        if (currentChar == '/' && peekNextChar() == '/') {
            //std::cout << "DEBUG: Iniciando comentario de una línea" << std::endl;
            eatNextChar(); // Consumir segundo '/'

            // Continuar hasta el final de la línea o fin de archivo
            while (currentChar != '\n' && currentChar != '\0') {
               // std::cout << "DEBUG: Consumir dentro de comentario de una línea. currentChar: " << currentChar << std::endl;
                currentChar = eatNextChar();
            }


            while (std::isspace(currentChar)) {
                currentChar = eatNextChar();
            }

            std::cout << "DEBUG: Carácter después del comentario de una línea: " << currentChar << "\n";

            //std::cout << "DEBUG: Fin de comentario de una línea" << std::endl;
            // Llamar nuevamente a getNextToken para continuar el escaneo
            return getNextToken();
        }



        // Operadores lógicos
        if (currentChar == '&' && peekNextChar() == '&') {
            eatNextChar();  // Operador &&
            return Token{tokenStartLocation, TokenKind::LogicalAndOp};
        }

        if (currentChar == '|' && peekNextChar() == '|') {
            eatNextChar();  // Operador ||
            return Token{tokenStartLocation, TokenKind::LogicalOrOp};
        }

        if (currentChar == '!') {
            if (peekNextChar() == '=') {
                eatNextChar();  // Operador !=
                return Token{tokenStartLocation, TokenKind::NotEqualOp};
            }
            return Token{tokenStartLocation, TokenKind::NotOp};  // Operador !
        }

        // Operadores relacionales
        if (currentChar == '>') {
            if (peekNextChar() == '=') {
                eatNextChar();  // Operador >=
                return Token{tokenStartLocation, TokenKind::GreaterEqualOp};
            } else {
                return Token{tokenStartLocation, TokenKind::GreaterOp};  // Operador >
            }
        }

        if (currentChar == '<') {
            if (peekNextChar() == '=') {
                eatNextChar();  // Operador <=
                return Token{tokenStartLocation, TokenKind::LessEqualOp};
            } else {
                return Token{tokenStartLocation, TokenKind::LessOp};  // Operador <
            }
        }

        // Identificar cadenas (strings) entre comillas dobles
        if (currentChar == '\"') {
            std::string value;
            currentChar = eatNextChar();
            while (currentChar != '\"' && currentChar != '\0') {
                if (currentChar == '\\') { // Manejar caracteres de escape
                    char nextChar = eatNextChar();
                    switch (nextChar) {
                        case 'n': value += '\n'; break;
                        case 't': value += '\t'; break;
                        case '\\': value += '\\'; break;
                        case '\"': value += '\"'; break;
                        default: value += nextChar; break;
                    }
                } else {
                    value += currentChar;
                }
                currentChar = eatNextChar();
            }
            return Token{tokenStartLocation, TokenKind::StringKeyword, std::move(value)};
        }

        


       

        // Revisar si es un token de un solo carácter
        switch (currentChar) {
            case '(': return Token{tokenStartLocation, TokenKind::LeftParenthesis};
            case ')': return Token{tokenStartLocation, TokenKind::RightParenthesis};
            case '{': return Token{tokenStartLocation, TokenKind::LeftBrace};
            case '}': return Token{tokenStartLocation, TokenKind::RightBrace};
            case ':': return Token{tokenStartLocation, TokenKind::ColonSymbol};
            case '+': return Token{tokenStartLocation, TokenKind::AddOp}; // Operador suma
            case '-': return Token{tokenStartLocation, TokenKind::SubOp}; // Operador resta
            case '*': return Token{tokenStartLocation, TokenKind::MulOp}; // Operador multiplicación
            case '/': return Token{tokenStartLocation, TokenKind::DivOp}; // Operador división
            case '%': return Token{tokenStartLocation, TokenKind::ModOp}; // Operador módulo
            case ';': return Token{tokenStartLocation, TokenKind::Semicolon}; // Punto y coma
            case '=': return Token{tokenStartLocation, TokenKind::AssignOp}; // Operador asignación
            case '\'': { // Detectar caracteres
                char charValue = eatNextChar(); // Tomar el carácter
                if (eatNextChar() == '\'') {
                    return Token{tokenStartLocation, TokenKind::Char, std::string(1, charValue)};
                }
                break;
            }
            case '$': return Token{tokenStartLocation, TokenKind::Eop}; // Fin del programa
        }

        // Identificar números
        if (std::isdigit(currentChar)) {
            std::string value{currentChar};
            while (std::isdigit(peekNextChar()))
                value += eatNextChar();
            return Token{tokenStartLocation, TokenKind::Number, std::move(value)};
        }

        // Identificar palabras clave o identificadores
        if (std::isalpha(currentChar) || currentChar == '_') {
            std::string value{currentChar};
            while (std::isalnum(peekNextChar()) || peekNextChar() == '_')
                value += eatNextChar();

            if (keywords.count(value)) {
                return Token{tokenStartLocation, keywords.at(value), std::move(value)};
            }

            return Token{tokenStartLocation, TokenKind::Identifier, std::move(value)};
        }

        // Si llegamos aquí, no se encontró un token válido
        return Token{tokenStartLocation, TokenKind::Unknown};
    }
};

// Función para probar el lexer con un archivo de ejemplo
int main() {
    std::ifstream file("prueba.txt");
    std::string fileContents((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    SourceFile sourceFile{"prueba.txt", fileContents};
    Lexer lexer(sourceFile);
    Token token;

    std::cout << "INFO SCAN - Start scanning.\n";
    while (true) {
        token = lexer.getNextToken();
        if (token.kind == TokenKind::Eof || token.kind == TokenKind::Unknown) break;

        std::string tokenType;
        std::string symbol;

        // Asegúrate de que los tokens que uses aquí están definidos en TokenKind
        switch (token.kind) {
            case TokenKind::LeftParenthesis:
                tokenType = "OPEN_PAR"; symbol = "(";
                break;
            case TokenKind::RightParenthesis:
                tokenType = "CLOSE_PAR"; symbol = ")";
                break;
            case TokenKind::LeftBrace:
                tokenType = "OPEN_BRACE"; symbol = "{";
                break;
            case TokenKind::RightBrace:
                tokenType = "CLOSE_BRACE"; symbol = "}";
                break;
            case TokenKind::AssignOp:
                tokenType = "ASSIGN_OP"; symbol = "=";
                break;
            case TokenKind::AddOp:
                tokenType = "ADD_OP"; symbol = "+";
                break;
            case TokenKind::SubOp:
                tokenType = "SUB_OP"; symbol = "-";
                break;
            case TokenKind::MulOp:
                tokenType = "MUL_OP"; symbol = "*";
                break;
            case TokenKind::DivOp:
                tokenType = "DIV_OP"; symbol = "/";
                break;
            case TokenKind::ModOp:
                tokenType = "MOD_OP"; symbol = "%";
                break;
            case TokenKind::Semicolon:
                tokenType = "SEMICOLON"; symbol = ";";
                break;
            case TokenKind::Number:
                tokenType = "NUMBER"; symbol = token.value.has_value() ? *token.value : "[Número]";
                break;
            case TokenKind::Identifier:
                tokenType = "ID"; symbol = token.value.has_value() ? *token.value : "[ID]";
                break;
            case TokenKind::Char:
                tokenType = "CHAR"; symbol = token.value.has_value() ? *token.value : "[Char]";
                break;
            case TokenKind::PrintKeyword:
                tokenType = "PRINT_KEY"; symbol = "print";
                break;

            // Agregamos los operadores que faltaban:
            case TokenKind::GreaterOp:
                tokenType = "GREATER_OP"; symbol = ">";
                break;
            case TokenKind::GreaterEqualOp:
                tokenType = "GREATER_EQUAL_OP"; symbol = ">=";
                break;
            case TokenKind::LessOp:
                tokenType = "LESS_OP"; symbol = "<";
                break;
            case TokenKind::LessEqualOp:
                tokenType = "LESS_EQUAL_OP"; symbol = "<=";
                break;
            case TokenKind::LogicalAndOp:
                tokenType = "LOGICAL_AND_OP"; symbol = "&&";
                break;
            case TokenKind::LogicalOrOp:
                tokenType = "LOGICAL_OR_OP"; symbol = "||";
                break;
            case TokenKind::NotOp:
                tokenType = "NOT_OP"; symbol = "!";
                break;

            case TokenKind::StringKeyword:
                tokenType = "STRING"; symbol = token.value.has_value() ? *token.value : "[Cadena]";
                break;

            case TokenKind::Eop:
                tokenType = "EOP"; symbol = "$";
                break;

            // Para comentarios multilínea, simplemente los ignoras y no los retornas como tokens,
            // pero si quieres tratarlos, puedes agregar esto:
            //case TokenKind::Comment:
            // tokenType = "COMMENT"; symbol = "/* ... */";
            // break;

            //         default:
            //             tokenType = "Token desconocido";
            //             symbol = "";
            //             break;
//
            //}

            default:
                tokenType = "UNKNOWN_TOKEN";
                symbol = "[Desconocido]";
                break;
            }

        std::cout << "DEBUG SCAN - " << tokenType << " [ " 
                  << symbol << " ] found at (" << token.location.line 
                  << ":" << token.location.col << ")\n";
    }

    std::cout << "INFO SCAN - Completed with 0 errors\n";
    return 0;
}
