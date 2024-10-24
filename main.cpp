#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <optional>
#include <cctype>
#include <stdlib.h>

#include "helper.h"
#include "parser.h"

int main() {
    // Leer archivo de entrada
    std::ifstream file("pruebaParser.txt"); // Asegúrate de que el archivo exista
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

    // std::cout << "INFO SCAN - Start scanning...\n";
    // while (true) {
    //     token = lexer.getNextToken();
    //     if (token.kind == TokenKind::Eof) break;
    //     //else if(token.kind == TokenKind::Unknown) {}

    //     std::string tokenType;
    //     std::string symbol;

    //     // Asegúrate de que los tokens que uses aquí están definidos en TokenKind
    //     switch (token.kind) {
    //         case TokenKind::KwArray: 
    //             tokenType = "KW_ARRAY"; 
    //             symbol = "array";
    //             break;
    //         case TokenKind::KwBoolean:
    //             tokenType = "KW_BOOLEAN"; 
    //             symbol = "boolean";
    //             break;
    //         case TokenKind::KwChar:
    //             tokenType = "KW_CHAR"; 
    //             symbol = "char";
    //             break;
    //         case TokenKind::KwElse:
    //             tokenType = "KW_ELSE"; 
    //             symbol = "else";
    //             break;
    //         case TokenKind::KwFalse:
    //             tokenType = "KW_FALSE"; 
    //             symbol = "false";
    //             break;
    //         case TokenKind::KwFor:
    //             tokenType = "KW_FOR"; 
    //             symbol = "for";
    //             break;
    //         case TokenKind::KwFunction:
    //             tokenType = "KW_FUNCTION"; 
    //             symbol = "function";
    //             break;
    //         case TokenKind::KwIf:
    //             tokenType = "KW_IF"; 
    //             symbol = "if";
    //             break;
    //         case TokenKind::KwInteger:
    //             tokenType = "KW_INTEGER"; 
    //             symbol = "integer";
    //             break;
    //         case TokenKind::KwPrint:    
    //             tokenType = "KW_PRINT"; 
    //             symbol = "print";
    //             break;
    //         case TokenKind::KwReturn:
    //             tokenType = "KW_RETURN"; 
    //             symbol = "return";
    //             break;
    //         case TokenKind::KwString:
    //             tokenType = "KW_STRING"; 
    //             symbol = "string";
    //             break;
    //         case TokenKind::KwTrue:
    //             tokenType = "KW_TRUE"; 
    //             symbol = "true";
    //             break;
    //         case TokenKind::KwVoid:
    //             tokenType = "KW_VOID"; 
    //             symbol = "void";
    //             break;
    //         case TokenKind::KwWhile:
    //             tokenType = "KW_WHILE"; 
    //             symbol = "while";
    //             break;

    //         case TokenKind::Number: 
    //             tokenType = "INT"; 
    //             symbol = token.value.has_value() ? *token.value : "[Número]";
    //             break;
    //         case TokenKind::Identifier: 
    //             tokenType = "ID"; 
    //             symbol = token.value.has_value() ? *token.value : "[ID]";
    //             break;
    //         case TokenKind::StringVal:
    //             tokenType = "STRING"; 
    //             symbol = token.value.has_value() ? *token.value : "[Cadena]";
    //             break;
    //         case TokenKind::CharVal:
    //             tokenType = "CHAR"; 
    //             symbol = token.value.has_value() ? *token.value : "[Caracter]";
    //             break;

    //         case TokenKind::ColonSymbol: 
    //             tokenType = "COLON"; 
    //             symbol = ":";
    //             break;
    //         case TokenKind::SemiColonSymbol:
    //             tokenType = "SEMICOLON"; 
    //             symbol = ";";
    //             break;
    //         case TokenKind::CommaSymbol:
    //             tokenType = "COMMA"; 
    //             symbol = ",";
    //             break;
    //         case TokenKind::LeftBracket:
    //             tokenType = "OPEN_BRACKET"; 
    //             symbol = "[";
    //             break;
    //         case TokenKind::RightBracket:
    //             tokenType = "CLOSE_BRACKET"; 
    //             symbol = "]";
    //             break;
    //         case TokenKind::LeftBrace:
    //             tokenType = "OPEN_BRACE"; 
    //             symbol = "{";
    //             break;
    //         case TokenKind::RightBrace:
    //             tokenType = "CLOSE_BRACE"; 
    //             symbol = "}";
    //             break;
    //         case TokenKind::LeftParenthesis: 
    //             tokenType = "OPEN_PAR"; 
    //             symbol = "(";
    //             break;
    //         case TokenKind::RightParenthesis: 
    //             tokenType = "CLOSE_PAR"; 
    //             symbol = ")";
    //             break;
    //         case TokenKind::PostfixIncrement:
    //             tokenType = "POSTFIX_INC"; 
    //             symbol = "++";
    //             break;
    //         case TokenKind::PostfixDecrement:
    //             tokenType = "POSTFIX_DEC"; 
    //             symbol = "--";
    //             break;
    //         case TokenKind::LogicalNot:
    //             tokenType = "LOGICAL_NOT"; 
    //             symbol = "!";
    //             break;
    //         case TokenKind::LogicalAnd:
    //             tokenType = "LOGICAL_AND"; 
    //             symbol = "&&";
    //             break;
    //         case TokenKind::LogicalOr:
    //             tokenType = "LOGICAL_OR"; 
    //             symbol = "||";
    //             break;
    //         case TokenKind::Exponentiation:
    //             tokenType = "EXP_OP"; 
    //             symbol = "^";
    //             break;
    //         case TokenKind::Multiplication:
    //             tokenType = "MULT_OP"; 
    //             symbol = "*";
    //             break;
    //         case TokenKind::Division:
    //             tokenType = "DIV_OP"; 
    //             symbol = "/";
    //             break;
    //         case TokenKind::Modulus:
    //             tokenType = "MOD_OP"; 
    //             symbol = "%";
    //             break;
    //         case TokenKind::Addition:
    //             tokenType = "ADD_OP"; 
    //             symbol = "+";
    //             break;
    //         case TokenKind::Subtraction:    
    //             tokenType = "SUB_OP"; 
    //             symbol = "-";
    //             break;
    //         case TokenKind::LessThan:  
    //             tokenType = "LESS_THAN"; 
    //             symbol = "<";
    //             break;
    //         case TokenKind::LessThanOrEqual:
    //             tokenType = "LESS_THAN_OR_EQUAL"; 
    //             symbol = "<=";
    //             break;
    //         case TokenKind::GreaterThan:
    //             tokenType = "GREATER_THAN"; 
    //             symbol = ">";
    //             break;
    //         case TokenKind::GreaterThanOrEqual:
    //             tokenType = "GREATER_THAN_OR_EQUAL"; 
    //             symbol = ">=";
    //             break;
    //         case TokenKind::isEqual:
    //             tokenType = "EQUAL"; 
    //             symbol = "==";
    //             break;
    //         case TokenKind::NotEqual:
    //             tokenType = "NOT_EQUAL"; 
    //             symbol = "!=";
    //             break; 
    //         case TokenKind::Assign: 
    //             tokenType = "ASSIGN_OP"; 
    //             symbol = "=";
    //             break; 
    //         case TokenKind::Eof:
    //             tokenType = "EOF"; 
    //             symbol = "$";
    //             break;

    //         default: 
    //             tokenType = "Token desconocido"; 
    //             symbol = "";
    //     }

    //     std::cout << "DEBUG SCAN - " << tokenType << " [ " 
    //               << symbol << " ] found at (" << token.location.line 
    //               << ":" << token.location.col << ")\n";
    // }
    // std::cout << "INFO SCAN - Completed with " << lexer.getErrorCount() << " error(s)\n";

    // Instanciar el parser con el lexer
    Parser parser(lexer);
    parser.start();


    return 0;
}