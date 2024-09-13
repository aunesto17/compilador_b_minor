#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <optional>
#include <cctype>

// Definición de los tipos de tokens
enum class TokenKind : char {
    Identifier, KwFn, KwVoid, KwInt, KwBoolean, KwChar, KwString, 
    Number, Lpar, Rpar, Lbrace, Rbrace, Colon, String, Eof, Unk,
    AssignOp, AddOp, Eop, KwPrint // Nuevos tokens agregados, incluyendo AddOp
};

// Mapa de palabras clave
const std::unordered_map<std::string_view, TokenKind> keywords = {
    {"fn", TokenKind::KwFn}, {"void", TokenKind::KwVoid}, 
    {"integer", TokenKind::KwInt}, {"boolean", TokenKind::KwBoolean}, 
    {"char", TokenKind::KwChar}, {"string", TokenKind::KwString}
};

// Caracteres de un solo símbolo como tokens
constexpr char singleCharTokens[] = {'\0', '(', ')', '{', '}', ':'};

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

public:
    explicit Lexer(const SourceFile &source) : source(&source) {}

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

    // Revisar si es un token de un solo carácter
    switch (currentChar) {
        case '(': return Token{tokenStartLocation, TokenKind::Lpar};
        case ')': return Token{tokenStartLocation, TokenKind::Rpar};
        case '{': return Token{tokenStartLocation, TokenKind::Lbrace};
        case '}': return Token{tokenStartLocation, TokenKind::Rbrace};
        case ':': return Token{tokenStartLocation, TokenKind::Colon};
        case '+': return Token{tokenStartLocation, TokenKind::AddOp}; // Operador suma
        case '=': return Token{tokenStartLocation, TokenKind::AssignOp}; // Operador asignación
        case '$': return Token{tokenStartLocation, TokenKind::Eop}; // Fin del programa
    }

    // Ignorar comentarios estilo Python (#)
    if (currentChar == '#') {
        while (peekNextChar() != '\n' && peekNextChar() != '\0') {
            eatNextChar();  // Ignorar el comentario hasta el final de la línea
        }
        return getNextToken();  // Continuar después del comentario
    }

    // Identificar cadenas
    if (currentChar == '\"') {
        std::string value;
        currentChar = eatNextChar();
        while (currentChar != '\"' && currentChar != '\0') {
            if (currentChar == '\\') { // Detectar caracteres de escape
                char nextChar = eatNextChar();
                switch (nextChar) {
                    case 'n': value += '\n'; break;
                    case 't': value += '\t'; break;
                    default: value += nextChar; break;
                }
            } else {
                value += currentChar;
            }
            currentChar = eatNextChar();
        }
        return Token{tokenStartLocation, TokenKind::String, std::move(value)};
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
        if (value == "print") {
            return Token{tokenStartLocation, TokenKind::KwPrint}; // Palabra clave 'print'
        }

        return Token{tokenStartLocation, TokenKind::Identifier, std::move(value)};
    }

    // Si llegamos aquí, no se encontró un token válido
    return Token{tokenStartLocation, TokenKind::Unk};
    }


};

int main() {
    // Buffer con el código de ejemplo de la imagen
    SourceFile sourceFile{"test.bminor", R"(
    a = 2 + 7
    print(a) # este es un comentario
    $
    )"};

    Lexer lexer(sourceFile);
    Token token;

    std::cout << "INFO SCAN - Start scanning...\n";
    while (true) {
        token = lexer.getNextToken();
        if (token.kind == TokenKind::Eof || token.kind == TokenKind::Unk) break;

        std::string tokenType;
        std::string symbol;

        // Asegúrate de que los tokens que uses aquí están definidos en TokenKind
        switch (token.kind) {
            case TokenKind::Lpar: 
                tokenType = "OPEN_PAR"; 
                symbol = "(";
                break;
            case TokenKind::Rpar: 
                tokenType = "CLOSE_PAR"; 
                symbol = ")";
                break;
            case TokenKind::AssignOp: 
                tokenType = "ASSIGN_OP"; 
                symbol = "=";
                break;
            case TokenKind::AddOp: 
                tokenType = "ADD_OP"; 
                symbol = "+";
                break;
            case TokenKind::Eop: 
                tokenType = "EOP"; 
                symbol = "$";
                break;
            case TokenKind::Number: 
                tokenType = "INT"; 
                symbol = token.value.has_value() ? *token.value : "[Número]";
                break;
            case TokenKind::Identifier: 
                tokenType = "ID"; 
                symbol = token.value.has_value() ? *token.value : "[ID]";
                break;
            case TokenKind::KwPrint: 
                tokenType = "PRINT_KEY"; 
                symbol = "print";
                break;
            default: 
                tokenType = "Token desconocido"; 
                symbol = "";
                break;
        }

        std::cout << "DEBUG SCAN - " << tokenType << " [ " 
                  << symbol << " ] found at (" << token.location.line 
                  << ":" << token.location.col << ")\n";
    }

    std::cout << "INFO SCAN - Completed with 0 errors\n";
    return 0;
}