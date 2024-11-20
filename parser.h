#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"
#include <iostream>
#include <string>

class Parser {
private:
    Lexer &lexer;
    Token currentToken;
    std::string debugPrefix;

    void eatToken() {
        std::cout << debugPrefix << "Eating token: " << currentToken.kindToString() << " [ " << currentToken.value.value_or("") << " ]" << std::endl;
        currentToken = lexer.getToken();
        std::cout << debugPrefix << "Next token: " << currentToken.kindToString() << " [ " << currentToken.value.value_or("") << " ]" << std::endl;
    }

    void reportError(const std::string &message) {
        std::cerr << "Syntax Error at line " << currentToken.location.line << ", col " << currentToken.location.col << ": " << message << std::endl;
    }

    bool expectToken(TokenKind expectedKind, const std::string &errorMessage) {
        if (currentToken.kind == expectedKind) {
            eatToken();
            return true;
        }
        reportError(errorMessage);
        return false;
    }

    // Helper function to manage debug prefix for indentation
    void increaseDebugPrefix() {
        debugPrefix += "  ";
    }

    void decreaseDebugPrefix() {
        if (debugPrefix.size() >= 2) {
            debugPrefix.resize(debugPrefix.size() - 2);
        }
    }

    // Grammar Rules
    bool program() {
        std::cout << debugPrefix << "Entering program rule" << std::endl;
        increaseDebugPrefix();
        if (!declaration()) return false;
        while (currentToken.kind != TokenKind::Eof) {
            if (!declaration()) return false;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting program rule" << std::endl;
        return true;
    }

    bool declaration() {
        std::cout << debugPrefix << "Entering declaration rule" << std::endl;
        increaseDebugPrefix();
        bool result;
        if (currentToken.kind == TokenKind::KwFunction) {
            result = function();
        } else if (isType(currentToken.kind)) {
            result = varDecl();
        } else {
            reportError("Expected function or variable declaration.");
            result = false;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting declaration rule" << std::endl;
        return result;
    }

    bool function() {
        std::cout << debugPrefix << "Entering function rule" << std::endl;
        increaseDebugPrefix();
        eatToken(); // consume 'function'
        if (!type()) return false;
        if (!expectToken(TokenKind::Identifier, "Expected function name.")) return false;
        if (!expectToken(TokenKind::LeftParenthesis, "Expected '('.")) return false;
        if (!params()) return false;
        if (!expectToken(TokenKind::RightParenthesis, "Expected ')'.")) return false;
        if (!expectToken(TokenKind::LeftBrace, "Expected '{'.")) return false;
        if (!stmntList()) return false;
        if (!expectToken(TokenKind::RightBrace, "Expected '}'.")) return false;
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting function rule" << std::endl;
        return true;
    }

    bool type() {
        std::cout << debugPrefix << "Entering type rule" << std::endl;
        increaseDebugPrefix();
        if (isType(currentToken.kind)) {
            eatToken();
            if (!typePrime()) return false;
            decreaseDebugPrefix();
            std::cout << debugPrefix << "Exiting type rule" << std::endl;
            return true;
        }
        reportError("Expected type.");
        decreaseDebugPrefix();
        return false;
    }

    bool typePrime() {
        std::cout << debugPrefix << "Entering typePrime rule" << std::endl;
        increaseDebugPrefix();
        if (currentToken.kind == TokenKind::LeftBracket) {
            eatToken();
            if (!expectToken(TokenKind::RightBracket, "Expected ']'.")) return false;
            if (!typePrime()) return false;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting typePrime rule" << std::endl;
        return true;
    }

    bool params() {
        std::cout << debugPrefix << "Entering params rule" << std::endl;
        increaseDebugPrefix();
        if (isType(currentToken.kind)) {
            if (!type()) return false;
            if (!expectToken(TokenKind::Identifier, "Expected parameter name.")) return false;
            if (!paramsPrime()) return false;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting params rule" << std::endl;
        return true; // epsilon
    }

    bool paramsPrime() {
        std::cout << debugPrefix << "Entering paramsPrime rule" << std::endl;
        increaseDebugPrefix();
        if (currentToken.kind == TokenKind::CommaSymbol) {
            eatToken();
            if (!params()) return false;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting paramsPrime rule" << std::endl;
        return true; // epsilon
    }

    bool varDecl() {
        std::cout << debugPrefix << "Entering varDecl rule" << std::endl;
        increaseDebugPrefix();
        if (!type()) return false;
        if (!expectToken(TokenKind::Identifier, "Expected variable name.")) return false;
        if (!varDeclPrime()) return false;
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting varDecl rule" << std::endl;
        return true;
    }

    bool varDeclPrime() {
        std::cout << debugPrefix << "Entering varDeclPrime rule" << std::endl;
        increaseDebugPrefix();
        if (currentToken.kind == TokenKind::Assign) {
            eatToken();
            if (!expression()) return false;
        }
        if (!expectToken(TokenKind::SemiColonSymbol, "Expected ';' after variable declaration.")) return false;
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting varDeclPrime rule" << std::endl;
        return true;
    }

    bool stmntList() {
        std::cout << debugPrefix << "Entering stmntList rule" << std::endl;
        increaseDebugPrefix();
        while (currentToken.kind != TokenKind::RightBrace && currentToken.kind != TokenKind::Eof) {
            if (!stmnt()) return false;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting stmntList rule" << std::endl;
        return true;
    }

    bool stmnt() {
        std::cout << debugPrefix << "Entering stmnt rule" << std::endl;
        increaseDebugPrefix();
        bool result;
        switch (currentToken.kind) {
            case TokenKind::KwIf:
                result = ifStmnt();
                break;
            case TokenKind::KwFor:
                result = forStmnt();
                break;
            case TokenKind::KwReturn:
                result = returnStmnt();
                break;
            case TokenKind::KwPrint:
                result = printStmnt();
                break;
            case TokenKind::LeftBrace:
                eatToken();
                result = stmntList() && expectToken(TokenKind::RightBrace, "Expected '}' at end of block.");
                break;
            default:
                if (isType(currentToken.kind)) {
                    result = varDecl();
                } else {
                    result = exprStmnt();
                }
                break;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting stmnt rule" << std::endl;
        return result;
    }

    bool ifStmnt() {
        std::cout << debugPrefix << "Entering ifStmnt rule" << std::endl;
        increaseDebugPrefix();
        eatToken(); // consume 'if'
        if (!expectToken(TokenKind::LeftParenthesis, "Expected '('.")) return false;
        if (!expression()) return false;
        if (!expectToken(TokenKind::RightParenthesis, "Expected ')'.")) return false;
        if (!expectToken(TokenKind::LeftBrace, "Expected '{'.")) return false;
        if (!stmntList()) return false;
        if (!expectToken(TokenKind::RightBrace, "Expected '}'.")) return false;
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting ifStmnt rule" << std::endl;
        return true;
    }

    bool forStmnt() {
        std::cout << debugPrefix << "Entering forStmnt rule" << std::endl;
        increaseDebugPrefix();
        eatToken(); // consume 'for'
        if (!expectToken(TokenKind::LeftParenthesis, "Expected '('.")) return false;
        if (!exprStmnt()) return false;
        if (!expression()) return false;
        if (!expectToken(TokenKind::SemiColonSymbol, "Expected ';'.")) return false;
        if (!exprStmnt()) return false;
        if (!expectToken(TokenKind::RightParenthesis, "Expected ')'.")) return false;
        if (!expectToken(TokenKind::LeftBrace, "Expected '{'.")) return false;
        if (!stmntList()) return false;
        if (!expectToken(TokenKind::RightBrace, "Expected '}'.")) return false;
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting forStmnt rule" << std::endl;
        return true;
    }

    bool returnStmnt() {
        std::cout << debugPrefix << "Entering returnStmnt rule" << std::endl;
        increaseDebugPrefix();
        eatToken(); // consume 'return'
        if (currentToken.kind != TokenKind::SemiColonSymbol) {
            if (!expression()) return false;
        }
        if (!expectToken(TokenKind::SemiColonSymbol, "Expected ';' after return statement.")) return false;
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting returnStmnt rule" << std::endl;
        return true;
    }

    bool printStmnt() {
        std::cout << debugPrefix << "Entering printStmnt rule" << std::endl;
        increaseDebugPrefix();
        eatToken(); // consume 'print'
        if (!expectToken(TokenKind::LeftParenthesis, "Expected '('.")) return false;
        if (!exprList()) return false;
        if (!expectToken(TokenKind::RightParenthesis, "Expected ')'.")) return false;
        if (!expectToken(TokenKind::SemiColonSymbol, "Expected ';' after print statement.")) return false;
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting printStmnt rule" << std::endl;
        return true;
    }

    bool exprStmnt() {
        std::cout << debugPrefix << "Entering exprStmnt rule" << std::endl;
        increaseDebugPrefix();
        if (currentToken.kind == TokenKind::SemiColonSymbol) {
            eatToken();
            decreaseDebugPrefix();
            std::cout << debugPrefix << "Exiting exprStmnt rule" << std::endl;
            return true;
        }
        if (!expression()) return false;
        if (!expectToken(TokenKind::SemiColonSymbol, "Expected ';' after expression.")) return false;
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting exprStmnt rule" << std::endl;
        return true;
    }

    bool exprList() {
        std::cout << debugPrefix << "Entering exprList rule" << std::endl;
        increaseDebugPrefix();
        if (!expression()) return false;
        while (currentToken.kind == TokenKind::CommaSymbol) {
            eatToken();
            if (!expression()) return false;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting exprList rule" << std::endl;
        return true;
    }

    // Updated expression rule following the provided grammar
    bool expression() {
        std::cout << debugPrefix << "Entering expression rule" << std::endl;
        increaseDebugPrefix();
        if (!orExpr()) {
            decreaseDebugPrefix();
            return false;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting expression rule" << std::endl;
        return true;
    }

    bool orExpr() {
        std::cout << debugPrefix << "Entering orExpr rule" << std::endl;
        increaseDebugPrefix();
        if (!andExpr()) {
            decreaseDebugPrefix();
            return false;
        }
        while (currentToken.kind == TokenKind::LogicalOr) {
            eatToken();
            if (!andExpr()) {
                decreaseDebugPrefix();
                return false;
            }
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting orExpr rule" << std::endl;
        return true;
    }

    bool andExpr() {
        std::cout << debugPrefix << "Entering andExpr rule" << std::endl;
        increaseDebugPrefix();
        if (!eqExpr()) {
            decreaseDebugPrefix();
            return false;
        }
        while (currentToken.kind == TokenKind::LogicalAnd) {
            eatToken();
            if (!eqExpr()) {
                decreaseDebugPrefix();
                return false;
            }
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting andExpr rule" << std::endl;
        return true;
    }

    bool eqExpr() {
        std::cout << debugPrefix << "Entering eqExpr rule" << std::endl;
        increaseDebugPrefix();
        if (!relExpr()) {
            decreaseDebugPrefix();
            return false;
        }
        while (currentToken.kind == TokenKind::isEqual || currentToken.kind == TokenKind::NotEqual) {
            eatToken();
            if (!relExpr()) {
                decreaseDebugPrefix();
                return false;
            }
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting eqExpr rule" << std::endl;
        return true;
    }

    bool relExpr() {
        std::cout << debugPrefix << "Entering relExpr rule" << std::endl;
        increaseDebugPrefix();
        if (!expr()) {
            decreaseDebugPrefix();
            return false;
        }
        while (currentToken.kind == TokenKind::LessThan ||
               currentToken.kind == TokenKind::LessThanOrEqual ||
               currentToken.kind == TokenKind::GreaterThan ||
               currentToken.kind == TokenKind::GreaterThanOrEqual) {
            eatToken();
            if (!expr()) {
                decreaseDebugPrefix();
                return false;
            }
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting relExpr rule" << std::endl;
        return true;
    }

    bool expr() {
        std::cout << debugPrefix << "Entering expr rule" << std::endl;
        increaseDebugPrefix();
        if (!term()) {
            decreaseDebugPrefix();
            return false;
        }
        while (currentToken.kind == TokenKind::Addition || currentToken.kind == TokenKind::Subtraction) {
            eatToken();
            if (!term()) {
                decreaseDebugPrefix();
                return false;
            }
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting expr rule" << std::endl;
        return true;
    }

    bool term() {
        std::cout << debugPrefix << "Entering term rule" << std::endl;
        increaseDebugPrefix();
        if (!unary()) {
            decreaseDebugPrefix();
            return false;
        }
        while (currentToken.kind == TokenKind::Multiplication ||
               currentToken.kind == TokenKind::Division ||
               currentToken.kind == TokenKind::Modulus) {
            eatToken();
            if (!unary()) {
                decreaseDebugPrefix();
                return false;
            }
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting term rule" << std::endl;
        return true;
    }

    bool unary() {
        std::cout << debugPrefix << "Entering unary rule" << std::endl;
        increaseDebugPrefix();
        if (currentToken.kind == TokenKind::Subtraction || currentToken.kind == TokenKind::LogicalNot) {
            eatToken();
            if (!unary()) {
                decreaseDebugPrefix();
                return false;
            }
        } else {
            if (!factor()) {
                decreaseDebugPrefix();
                return false;
            }
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting unary rule" << std::endl;
        return true;
    }

    bool factor() {
        std::cout << debugPrefix << "Entering factor rule" << std::endl;
        increaseDebugPrefix();
        if (currentToken.kind == TokenKind::Identifier) {
            eatToken();
            if (currentToken.kind == TokenKind::LeftParenthesis) {
                eatToken();
                if (currentToken.kind != TokenKind::RightParenthesis) {
                    if (!exprList()) {
                        decreaseDebugPrefix();
                        return false;
                    }
                }
                if (!expectToken(TokenKind::RightParenthesis, "Expected ')' after function call.")) {
                    decreaseDebugPrefix();
                    return false;
                }
            }
        } else if (
            currentToken.kind == TokenKind::Number ||
            currentToken.kind == TokenKind::CharVal ||
            currentToken.kind == TokenKind::StringVal ||
            currentToken.kind == TokenKind::KwTrue ||
            currentToken.kind == TokenKind::KwFalse
        ) {
            eatToken();
        } else if (currentToken.kind == TokenKind::LeftParenthesis) {
            eatToken();
            if (!expression()) {
                decreaseDebugPrefix();
                return false;
            }
            if (!expectToken(TokenKind::RightParenthesis, "Expected ')' after expression.")) {
                decreaseDebugPrefix();
                return false;
            }
        } else {
            reportError("Expected factor.");
            decreaseDebugPrefix();
            return false;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting factor rule" << std::endl;
        return true;
    }

    bool isType(TokenKind kind) {
        return kind == TokenKind::KwInteger || kind == TokenKind::KwBoolean || kind == TokenKind::KwChar || kind == TokenKind::KwString || kind == TokenKind::KwVoid;
    }

public:
    explicit Parser(Lexer &lexer) : lexer(lexer), debugPrefix("") {
        eatToken();
    }

    void parse() {
        std::cout << "Starting parse" << std::endl;
        if (program()) {
            std::cout << "Parsing completed successfully." << std::endl;
        } else {
            std::cout << "Parsing failed." << std::endl;
        }
    }
};

#endif // PARSER_H_
