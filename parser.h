#ifndef PARSER_H_
#define PARSER_H_

#include "lexer.h"
#include <iostream>
#include <string>
#include <set>
#include "ast.h"

class Parser {
private:
    Lexer &lexer;
    Token currentToken;
    std::string debugPrefix;
    ProgramNode *programNodeRoot;
    bool completeAst = true;
    vector<string> errors;
    int errorCount = 0;

     // Función para realizar la recuperación por pánico
    void panicRecoveryForRule(const std::set<TokenKind> &followSet) {
        std::cout << debugPrefix << "Entering panic recovery mode." << std::endl; // Nuevo mensaje
        while (currentToken.kind != TokenKind::Eof && followSet.find(currentToken.kind) == followSet.end()) {
            eatToken();
        }
        std::cout << debugPrefix << "Exiting panic recovery mode." << std::endl; // Nuevo mensaje
    }

    // retorna el tipo de currentToken en string
    std::string getCurrentTokenValue() {
        return currentToken.kindToString();
    }

    void eatToken() {
        std::cout << debugPrefix << "Eating token: " << currentToken.kindToString() << " [ " << currentToken.value.value_or("") << " ]" << std::endl;
        currentToken = lexer.getToken();
        std::cout << debugPrefix << "Next token: " << currentToken.kindToString() << " [ " << currentToken.value.value_or("") << " ]" << std::endl;
    }

    void reportError(const std::string &message) {
        // add error message to the list
        completeAst = false;
        errorCount++;
        errors.push_back("Syntax Error at line " + to_string(currentToken.location.line) + ", col " + to_string(currentToken.location.col) + ": " + message);
        std::cerr << "Syntax Error at line " << currentToken.location.line << ", col " << currentToken.location.col << ": " << message << std::endl;
    }


    bool expectToken(TokenKind expectedKind, const std::string &errorMessage, const std::set<TokenKind> &followSet = {}) {
        if (currentToken.kind == expectedKind) {
            eatToken();
            return true;
        }
        reportError(errorMessage);
        panicRecoveryForRule(followSet);
        return followSet.empty() || followSet.find(currentToken.kind) != followSet.end();
    }

      void increaseDebugPrefix() {
        debugPrefix += "  ";
    }

    void decreaseDebugPrefix() {
        if (debugPrefix.size() >= 2) {
            debugPrefix.resize(debugPrefix.size() - 2);
        }
    }

    // Grammar Rules
    ProgramNode * program() {
        std::cout << debugPrefix << "Entering program rule" << std::endl;
        increaseDebugPrefix();
        
        ProgramNode* programNode = new ProgramNode();
        
        DeclNode* firstDecl = declaration();
        if (!firstDecl) {
            panicRecoveryForRule({TokenKind::Eof});
        } else {
            programNode->addDeclaration(firstDecl);
        }

        // programPrime
        while (currentToken.kind != TokenKind::Eof) {
            DeclNode* decl = declaration();
            if (!decl) {
                panicRecoveryForRule({TokenKind::Eof});
                continue;
            }
            programNode->addDeclaration(decl);
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting program rule" << std::endl;
        return programNode;
    }

    DeclNode * declaration() {
        std::cout << debugPrefix << "Entering declaration rule" << std::endl;
        increaseDebugPrefix();
        
        DeclNode* result = nullptr;
        
        if (currentToken.kind == TokenKind::KwFunction) {
            result = function();
        } else if (isType(currentToken.kind)) {
            result = varDecl();
        } else {
            reportError("Expected function or variable declaration.");
            panicRecoveryForRule({TokenKind::KwFunction, TokenKind::KwInteger, TokenKind::KwBoolean, 
                                TokenKind::KwChar, TokenKind::KwString, TokenKind::KwVoid, TokenKind::Eof});
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting declaration rule" << std::endl;
        return result;
    }

    
    FunctionDeclNode * function() {
        std::cout << debugPrefix << "Entering function rule" << std::endl;
        increaseDebugPrefix();

        eatToken(); // consume 'function'

        // Parse return type
        TypeNode* returnType = parseType();
        if (!returnType) {
            panicRecoveryForRule({TokenKind::Identifier});
            decreaseDebugPrefix();
            return nullptr;
        }
        
        // Parse function name
        if (currentToken.kind != TokenKind::Identifier) {
            reportError("Expected function name");
            delete returnType;
            decreaseDebugPrefix();
            return nullptr;
        }
        std::string functionName = currentToken.value.value_or("");
        eatToken();
        
        // Create function node
        FunctionDeclNode* functionNode = new FunctionDeclNode(returnType, functionName);
        
        if (!expectToken(TokenKind::LeftParenthesis, "Expected '('.")) {
            delete functionNode;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        // Parse parameters
        if (currentToken.kind != TokenKind::RightParenthesis) {
            do {
                TypeNode* paramType = parseType();
                if (!paramType) break;
                
                if (currentToken.kind != TokenKind::Identifier) {
                    reportError("Expected parameter name");
                    delete paramType;
                    break;
                }
                std::string paramName = currentToken.value.value_or("");
                eatToken();
                
                functionNode->addParameter(new VarDeclNode(paramType, paramName));
                
                if (currentToken.kind != TokenKind::CommaSymbol) break;
                eatToken();
            } while (true);
        }
        
        if (!expectToken(TokenKind::RightParenthesis, "Expected ')'.")) {
            delete functionNode;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        if (!expectToken(TokenKind::LeftBrace, "Expected '{'.")) {
            delete functionNode;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        // Parse function body
        while (currentToken.kind != TokenKind::RightBrace && currentToken.kind != TokenKind::Eof) {
            StmtNode* stmt = parseStatement();
            if (stmt) {
                functionNode->addStatement(stmt);
            } else {
                panicRecoveryForRule({TokenKind::RightBrace});
            }
        }
        
        if (!expectToken(TokenKind::RightBrace, "Expected '}' at end of function.")) {
            delete functionNode;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting function rule" << std::endl;
        return functionNode;
    }

    StmtNode* parseStatement() {
        std::cout << debugPrefix << "Entering parseStatement rule" << std::endl;
        increaseDebugPrefix();

        switch (currentToken.kind) {
            case TokenKind::KwIf:
                return ifStmnt();
            case TokenKind::KwFor:
                return forStmnt();
            case TokenKind::KwWhile:
                return whileStmnt();
            case TokenKind::KwReturn:
                return returnStmnt();
            case TokenKind::KwPrint:
                return printStmnt();
            case TokenKind::LeftBrace: {
                eatToken();
                BlockStmtNode* block = new BlockStmtNode();
                while (currentToken.kind != TokenKind::RightBrace && currentToken.kind != TokenKind::Eof) {
                    StmtNode* stmt = parseStatement();
                    if (stmt) {
                        block->addStatement(stmt);
                    }
                }
                expectToken(TokenKind::RightBrace, "Expected '}' at end of block.");
                return block;
            }
            default:
                if (isType(currentToken.kind)) {
                    return varDecl();
                } else {
                    return exprStmnt();
                }
        }
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
        while (currentToken.kind == TokenKind::LeftBracket) {
            eatToken(); // consume '['
            if (currentToken.kind != TokenKind::RightBracket) {
                if (!expression()) {
                    decreaseDebugPrefix();
                    return false;
                }
            }
            if (!expectToken(TokenKind::RightBracket, "Expected ']' after array size expression.")) return false;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting typePrime rule" << std::endl;
        return true;
    }


    bool params() {
        std::cout << debugPrefix << "Entering params rule" << std::endl;
        increaseDebugPrefix();
        if (isType(currentToken.kind)) {
            if (!type()) {
                panicRecoveryForRule({TokenKind::Identifier});
            }
            if (!expectToken(TokenKind::Identifier, "Expected parameter name.", {TokenKind::CommaSymbol, TokenKind::RightParenthesis})) {
                decreaseDebugPrefix();
                return false;
            }
            if (!paramsPrime()) {
                panicRecoveryForRule({TokenKind::RightParenthesis});
            }
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting params rule" << std::endl;
        return true; // epsilon
    }

    bool paramsPrime() {
        std::cout << debugPrefix << "Entering paramsPrime rule" << std::endl;
        increaseDebugPrefix();
        while (currentToken.kind == TokenKind::CommaSymbol) {
            eatToken();
            if (!type()) {
                panicRecoveryForRule({TokenKind::Identifier});
            }
            if (!expectToken(TokenKind::Identifier, "Expected parameter name.", {TokenKind::CommaSymbol, TokenKind::RightParenthesis})) {
                decreaseDebugPrefix();
                return false;
            }
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting paramsPrime rule" << std::endl;
        return true;
    }

    VarDeclNode * varDecl() {
        std::cout << debugPrefix << "Entering varDecl rule" << std::endl;
        increaseDebugPrefix();

        // Parse type
        TypeNode* typeNode = parseType();
        if (!typeNode) {
            panicRecoveryForRule({TokenKind::Identifier});
            decreaseDebugPrefix();
            return nullptr;
        }

        // Parse identifier
        if (currentToken.kind != TokenKind::Identifier) {
            reportError("Expected variable name.");
            delete typeNode;
            panicRecoveryForRule({TokenKind::Assign, TokenKind::SemiColonSymbol});
            decreaseDebugPrefix();
            return nullptr;
        }

        std::string identifier = currentToken.value.value_or("");
        eatToken();  // consume identifier

        // Parse initialization (if any)
        ExprNode* initExpr = nullptr;
        if (!varDeclPrime(&initExpr)) {
            delete typeNode;
            decreaseDebugPrefix();
            return nullptr;
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting varDecl rule" << std::endl;
        return new VarDeclNode(typeNode, identifier, initExpr);
    }

    TypeNode* parseType() {
        std::cout << debugPrefix << "Entering parseType rule" << std::endl;
        increaseDebugPrefix();
        
        if (!isType(currentToken.kind)) {
            reportError("Expected type specifier.");
            return nullptr;
        }

        std::string baseType;
        switch (currentToken.kind) {
            case TokenKind::KwInteger: baseType = "int"; break;
            case TokenKind::KwBoolean: baseType = "boolean"; break;
            case TokenKind::KwChar: baseType = "char"; break;
            case TokenKind::KwString: baseType = "string"; break;
            case TokenKind::KwVoid: baseType = "void"; break;
            default: return nullptr;
        }
        
        eatToken();  // consume type keyword
        
        TypeNode* typeNode = new TypeNode(baseType);
        
        // Handle array type declarations
        while (currentToken.kind == TokenKind::LeftBracket) {
            eatToken();  // consume '['
            
            ExprNode* dimExpr = nullptr;
            if (currentToken.kind != TokenKind::RightBracket) {
                dimExpr = expression();
                if (!dimExpr) {
                    delete typeNode;
                    return nullptr;
                }
            }
            
            if (!expectToken(TokenKind::RightBracket, "Expected ']' after array dimension")) {
                delete typeNode;
                delete dimExpr;
                return nullptr;
            }
            
            typeNode->addArrayDimension(dimExpr);
        }
        
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting parseType rule" << std::endl;
        return typeNode;
    }


    bool varDeclPrime(ExprNode ** initExpr) {
        std::cout << debugPrefix << "Entering varDeclPrime rule" << std::endl;
        increaseDebugPrefix();

        if (currentToken.kind == TokenKind::Assign) {
            eatToken();  // consume '='
            *initExpr = expression();
            if (!*initExpr) {
                panicRecoveryForRule({TokenKind::SemiColonSymbol});
                decreaseDebugPrefix();
                return false;
            }
        }

        if (!expectToken(TokenKind::SemiColonSymbol, "Expected ';' after variable declaration.")) {
            decreaseDebugPrefix();
            return false;
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting varDeclPrime rule" << std::endl;
        return true;
    }

    bool stmntList() {
        std::cout << debugPrefix << "Entering stmntList rule" << std::endl;
        increaseDebugPrefix();
        while (currentToken.kind != TokenKind::RightBrace && currentToken.kind != TokenKind::Eof) {
            if (!stmnt()) {
                panicRecoveryForRule({TokenKind::RightBrace, TokenKind::KwIf, TokenKind::KwFor, TokenKind::KwWhile, TokenKind::KwReturn, TokenKind::KwPrint});
            }
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
            case TokenKind::KwWhile:
                result = whileStmnt();
                break;
            case TokenKind::KwReturn:
                result = returnStmnt();
                break;
            case TokenKind::KwPrint:
                result = printStmnt();
                break;
            case TokenKind::LeftBrace:
                eatToken();
                result = stmntList() && expectToken(TokenKind::RightBrace, "Expected '}' at end of block.", {TokenKind::KwIf, TokenKind::KwFor, TokenKind::KwWhile, TokenKind::KwReturn, TokenKind::KwPrint, TokenKind::RightBrace});
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


    StmtNode * ifStmnt() {
        std::cout << debugPrefix << "Entering ifStmnt rule" << std::endl;
        increaseDebugPrefix();

        // Manejo del "if"
        eatToken(); // consume 'if'
        
        // Parse condition
        if (!expectToken(TokenKind::LeftParenthesis, "Expected '(' after if.")) {
            decreaseDebugPrefix();
            return nullptr;
        }

        ExprNode* condition = expression();
        if (!condition) {
            decreaseDebugPrefix();
            return nullptr;
        }

        if (!expectToken(TokenKind::RightParenthesis, "Expected ')' after if condition.")) {
            delete condition;
            decreaseDebugPrefix();
            return nullptr;
        }

        // Parse then branch
        if (!expectToken(TokenKind::LeftBrace, "Expected '{' at start of if body.")) {
            delete condition;
            decreaseDebugPrefix();
            return nullptr;
        }

        BlockStmtNode* thenBranch = new BlockStmtNode();
        while (currentToken.kind != TokenKind::RightBrace && currentToken.kind != TokenKind::Eof) {
            StmtNode* stmt = parseStatement();
            if (!stmt) {
                delete condition;
                delete thenBranch;
                decreaseDebugPrefix();
                return nullptr;
            }
            thenBranch->addStatement(stmt);
        }

        if (!expectToken(TokenKind::RightBrace, "Expected '}' at end of if body.")) {
            delete condition;
            delete thenBranch;
            decreaseDebugPrefix();
            return nullptr;
        }

        // Parse optional else branch (IfStmnt')
        BlockStmtNode* elseBranch = nullptr;
        if (currentToken.kind == TokenKind::KwElse) {
            eatToken(); // consume 'else'

            if (!expectToken(TokenKind::LeftBrace, "Expected '{' at start of else body.")) {
                delete condition;
                delete thenBranch;
                decreaseDebugPrefix();
                return nullptr;
            }

            elseBranch = new BlockStmtNode();
            while (currentToken.kind != TokenKind::RightBrace && currentToken.kind != TokenKind::Eof) {
                StmtNode* stmt = parseStatement();
                if (!stmt) {
                    delete condition;
                    delete thenBranch;
                    delete elseBranch;
                    decreaseDebugPrefix();
                    return nullptr;
                }
                elseBranch->addStatement(stmt);
            }

            if (!expectToken(TokenKind::RightBrace, "Expected '}' at end of else body.")) {
                delete condition;
                delete thenBranch;
                delete elseBranch;
                decreaseDebugPrefix();
                return nullptr;
            }
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting ifStmnt rule" << std::endl;
        return new IfStmtNode(condition, thenBranch, elseBranch);
    }


    StmtNode * forStmnt() {
        std::cout << debugPrefix << "Entering forStmnt rule" << std::endl;
        increaseDebugPrefix();
        
        eatToken(); // consume 'for'
        
        if (!expectToken(TokenKind::LeftParenthesis, "Expected '('.")) {
            decreaseDebugPrefix();
            return nullptr;
        }
        
        // Parse initialization - must be an ExprStmnt
        StmtNode* init = exprStmnt();
        if (!init) {
            decreaseDebugPrefix();
            return nullptr;
        }
        
        // Parse condition Expression
        ExprNode* cond = expression();
        if (!cond) {
            delete init;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        if (!expectToken(TokenKind::SemiColonSymbol, "Expected ';' after condition.")) {
            delete init;
            delete cond;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        // Parse increment - must be an ExprStmnt
        StmtNode* increment = exprStmnt();
        if (!increment) {
            delete init;
            delete cond;
            decreaseDebugPrefix();
            return nullptr;
        }

        if (!expectToken(TokenKind::RightParenthesis, "Expected ';' after condition.")) {
            delete init;
            delete cond;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        if (!expectToken(TokenKind::LeftBrace, "Expected '{' after for header.")) {
            delete init;
            delete cond;
            delete increment;
            decreaseDebugPrefix();
            return nullptr;
        }

        // Parse body
        BlockStmtNode* body = new BlockStmtNode();
        while (currentToken.kind != TokenKind::RightBrace && currentToken.kind != TokenKind::Eof) {
            StmtNode* stmt = parseStatement();
            if (stmt) {
                body->addStatement(stmt);
            } else {
                delete init;
                delete cond;
                delete increment;
                delete body;
                decreaseDebugPrefix();
                return nullptr;
            }
        }

        if (!expectToken(TokenKind::RightBrace, "Expected '}' at end of for body.")) {
            delete init;
            delete cond;
            delete increment;
            delete body;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting forStmnt rule" << std::endl;
        return new ForStmtNode(init, cond, increment, body);
    }

    StmtNode * whileStmnt() {
        std::cout << debugPrefix << "Entering whileStmnt rule" << std::endl;
        increaseDebugPrefix();

        eatToken(); // consume 'while'

        if (!expectToken(TokenKind::LeftParenthesis, "Expected '('.")) {
            decreaseDebugPrefix();
            return nullptr;
        }
        
        ExprNode* condition = expression();
        if (!condition) {
            decreaseDebugPrefix();
            return nullptr;
        }
        
        if (!expectToken(TokenKind::RightParenthesis, "Expected ')'.")) {
            delete condition;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        StmtNode* body = parseStatement();
        if (!body) {
            delete condition;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting whileStmnt rule" << std::endl;
        return new WhileStmtNode(condition, body);
    }


    StmtNode * returnStmnt() {
        std::cout << debugPrefix << "Entering returnStmnt rule" << std::endl;
        increaseDebugPrefix();

        eatToken(); // consume 'return'

        ExprNode* returnValue = nullptr;
        if (currentToken.kind != TokenKind::SemiColonSymbol) {
            returnValue = expression();
            if (!returnValue) {
                decreaseDebugPrefix();
                return nullptr;
            }
        }
        
        if (!expectToken(TokenKind::SemiColonSymbol, "Expected ';' after return.")) {
            delete returnValue;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting returnStmnt rule" << std::endl;
        return new ReturnStmtNode(returnValue);
    }

    StmtNode * printStmnt() {
        std::cout << debugPrefix << "Entering printStmnt rule" << std::endl;
        increaseDebugPrefix();

        eatToken(); // consume 'print'

        if (!expectToken(TokenKind::LeftParenthesis, "Expected '('.")) {
            decreaseDebugPrefix();
            return nullptr;
        }
        
        std::vector<ExprNode*> expressions;
        do {
            ExprNode* expr = expression();
            if (!expr) {
                for (auto e : expressions) delete e;
                decreaseDebugPrefix();
                return nullptr;
            }
            expressions.push_back(expr);
            
            if (currentToken.kind != TokenKind::CommaSymbol) break;
            eatToken(); // consexprStmntume ','
        } while (true);
        
        if (!expectToken(TokenKind::RightParenthesis, "Expected ')'.")) {
            for (auto e : expressions) delete e;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        if (!expectToken(TokenKind::SemiColonSymbol, "Expected ';' after print statement.")) {
            for (auto e : expressions) delete e;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting printStmnt rule" << std::endl;
        return new PrintStmtNode(expressions);
    }

    StmtNode * exprStmnt() {
        std::cout << debugPrefix << "Entering exprStmnt rule" << std::endl;
        increaseDebugPrefix();

        ExprNode* expr = nullptr;
        if (currentToken.kind != TokenKind::SemiColonSymbol) {
            expr = expression();
            if (!expr) {
                decreaseDebugPrefix();
                return nullptr;
            }
        }
        
        if (!expectToken(TokenKind::SemiColonSymbol, "Expected ';' after expression.")) {
            delete expr;
            decreaseDebugPrefix();
            return nullptr;
        }
        
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting exprStmnt rule" << std::endl;
        return new ExprStmtNode(expr);
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
    ExprNode * expression() {
        std::cout << debugPrefix << "Entering expression rule" << std::endl;
        increaseDebugPrefix();

        // Similar to before, but return appropriate nodes
        if (currentToken.kind == TokenKind::Identifier) {
            Token lookaheadToken = lexer.peekToken();
            
            if (lookaheadToken.kind == TokenKind::Assign) {
                std::string identifier = currentToken.value.value_or("");
                eatToken(); // consume identifier
                eatToken(); // consume '='
                
                ExprNode* rightExpr = expression();
                if(!rightExpr) {
                    decreaseDebugPrefix();
                    return nullptr;
                }
                decreaseDebugPrefix();
                std::cout << debugPrefix << "Exiting expression rule" << std::endl;
                return new AssignExprNode(identifier, "auto", rightExpr);
            }
        }
        
        ExprNode* orExprNode = orExpr();
        if (!orExprNode) {
            decreaseDebugPrefix();
            return nullptr;
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting expression rule" << std::endl;
        return orExprNode;
    }


    ExprNode * orExpr() {
        std::cout << debugPrefix << "Entering orExpr rule" << std::endl;
        increaseDebugPrefix();

        ExprNode* left = andExpr();
        if (!left) {
            decreaseDebugPrefix();
            return nullptr;
        }
        while (currentToken.kind == TokenKind::LogicalOr) {
            std::string op = currentToken.value.value_or("||");
            eatToken();
            ExprNode* right = andExpr();
            if (!right) {
                decreaseDebugPrefix();
                return nullptr;
            }

            left = new BinaryExprNode(op, left, right);
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting orExpr rule" << std::endl;
        return left;
    }

    ExprNode * andExpr() {
        std::cout << debugPrefix << "Entering andExpr rule" << std::endl;
        increaseDebugPrefix();

        ExprNode* left = eqExpr();
        if (!left) {
            decreaseDebugPrefix();
            return nullptr;
        }
        while (currentToken.kind == TokenKind::LogicalAnd) {
            std::string op = currentToken.value.value_or("&&");
            eatToken();
            ExprNode* right = eqExpr();
            if (!right) {
                decreaseDebugPrefix();
                return nullptr;
            }
            left = new BinaryExprNode(op, left, right);
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting andExpr rule" << std::endl;
        return left;
    }

    ExprNode * eqExpr() {
        std::cout << debugPrefix << "Entering eqExpr rule" << std::endl;
        increaseDebugPrefix();

        ExprNode* left = relExpr();
        if (!left) {
            decreaseDebugPrefix();
            return nullptr;
        }
        while (currentToken.kind == TokenKind::isEqual || currentToken.kind == TokenKind::NotEqual) {
            std::string op = currentToken.kind == TokenKind::isEqual ? "==" : "!=";
            eatToken();
            ExprNode* right = relExpr();
            if (!right) {
                decreaseDebugPrefix();
                return nullptr;
            }
            left = new BinaryExprNode(op, left, right);
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting eqExpr rule" << std::endl;
        return left;
    }

    ExprNode * relExpr() {
        std::cout << debugPrefix << "Entering relExpr rule" << std::endl;
        increaseDebugPrefix();

        ExprNode* left = expr();
        if (!left) {
            decreaseDebugPrefix();
            return nullptr;
        }

        while (currentToken.kind == TokenKind::LessThan || 
            currentToken.kind == TokenKind::GreaterThan || 
            currentToken.kind == TokenKind::LessThanOrEqual || 
            currentToken.kind == TokenKind::GreaterThanOrEqual) {
            std::string op;
            switch (currentToken.kind) {
                case TokenKind::LessThan: op = "<"; break;
                case TokenKind::GreaterThan: op = ">"; break;
                case TokenKind::LessThanOrEqual: op = "<="; break;
                case TokenKind::GreaterThanOrEqual: op = ">="; break;
                default: break;
            }
            eatToken();
            ExprNode* right = expr();
            if (!right) {
                decreaseDebugPrefix();  
                return nullptr;
            }
            left = new BinaryExprNode(op, left, right);
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting relExpr rule" << std::endl;
        return left;
    }

    ExprNode * expr() {
        std::cout << debugPrefix << "Entering expr rule" << std::endl;
        increaseDebugPrefix();

        ExprNode* left = term();
        if (!left) {
            decreaseDebugPrefix();
            return nullptr;
        }
        
        while (currentToken.kind == TokenKind::Addition || 
            currentToken.kind == TokenKind::Subtraction) {
            std::string op = currentToken.kind == TokenKind::Addition ? "+" : "-";
            eatToken();
            ExprNode* right = term();
            if (!right) {
                decreaseDebugPrefix();
                return nullptr;
            }
            left = new BinaryExprNode(op, left, right);
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting expr rule" << std::endl;
        return left;
    }

    ExprNode * term() {
        std::cout << debugPrefix << "Entering term rule" << std::endl;
        increaseDebugPrefix();

        ExprNode* left = unary();
        if (!left) {
            decreaseDebugPrefix();
            return nullptr;
        }
        
        while (currentToken.kind == TokenKind::Multiplication || 
            currentToken.kind == TokenKind::Division || 
            currentToken.kind == TokenKind::Modulus) {
            std::string op;
            switch (currentToken.kind) {
                case TokenKind::Multiplication: op = "*"; break;
                case TokenKind::Division: op = "/"; break;
                case TokenKind::Modulus: op = "%"; break;
                default: break;
            }
            eatToken();
            ExprNode* right = unary();
            if (!right) {
                decreaseDebugPrefix();
                return nullptr;
            }
            left = new BinaryExprNode(op, left, right);
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting term rule" << std::endl;
        return left;
    }

    ExprNode * unary() {
        std::cout << debugPrefix << "Entering unary rule" << std::endl;
        increaseDebugPrefix();
        
        if (currentToken.kind == TokenKind::Subtraction || 
            currentToken.kind == TokenKind::LogicalNot) {
            std::string op = currentToken.kind == TokenKind::Subtraction ? "-" : "!";
            eatToken();
            ExprNode* expr = unary();
            if (!expr) {
                decreaseDebugPrefix();
                return nullptr;
            }
            return new UnaryExprNode(op, expr);
        }

        ExprNode* result = factor();
        if(!result) {
            decreaseDebugPrefix();
            return nullptr;
        }
        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting unary rule" << std::endl;
        return result;
    }

    ExprNode * factor() {
        std::cout << debugPrefix << "Entering factor rule" << std::endl;
        increaseDebugPrefix();

        ExprNode* result = nullptr;

        switch (currentToken.kind) {
            case TokenKind::Identifier: {
                std::string id = currentToken.value.value_or("");
                eatToken();
                if (currentToken.kind == TokenKind::LeftParenthesis) {
                    eatToken(); // consume '('
                    std::vector<ExprNode*> args;
                    if (currentToken.kind != TokenKind::RightParenthesis) {
                        do {
                            ExprNode* arg = expression();
                            if (!arg) {
                                decreaseDebugPrefix();
                                return nullptr;
                            }
                            args.push_back(arg);
                            if (currentToken.kind != TokenKind::CommaSymbol) break;
                            eatToken(); // consume ','
                        } while (true);
                    }
                    if (!expectToken(TokenKind::RightParenthesis, "Expected ')'")) {
                        decreaseDebugPrefix();
                        return nullptr;
                    }
                    result = new FunctionCallNode(id, args);
                } else if (currentToken.kind == TokenKind::LeftBracket) {
                    // Array access
                    std::vector<ExprNode*> indices;
                    while (currentToken.kind == TokenKind::LeftBracket) {
                        eatToken(); // consume '['
                        ExprNode* index = expression();
                        if (!index) {
                            decreaseDebugPrefix();
                            return nullptr;
                        }
                        indices.push_back(index);
                        if (!expectToken(TokenKind::RightBracket, "Expected ']'")) {
                            decreaseDebugPrefix();
                            return nullptr;
                        }
                    }
                    result = new ArrayAccessNode(id, indices);
                } else {
                    result = new IdentifierNode(id);
                }
                break;
            }
            case TokenKind::Number:
                result = new LiteralExprNode("int", currentToken.value.value_or(""));
                eatToken();
                break;
            case TokenKind::CharVal:
                result = new LiteralExprNode("char", currentToken.value.value_or(""));
                eatToken();
                break;
            case TokenKind::StringVal:
                result = new LiteralExprNode("string", currentToken.value.value_or(""));
                eatToken();
                break;
            case TokenKind::KwTrue:
            case TokenKind::KwFalse:
                result = new LiteralExprNode("boolean", currentToken.value.value_or(""));
                eatToken();
                break;
            case TokenKind::LeftParenthesis:
                eatToken();
                result = expression();
                if (!expectToken(TokenKind::RightParenthesis, "Expected ')'")) {
                    decreaseDebugPrefix();
                    return nullptr;
                }
                break;
            default:
                reportError("Expected factor");
                decreaseDebugPrefix();
                return nullptr;
        }

        decreaseDebugPrefix();
        std::cout << debugPrefix << "Exiting factor rule" << std::endl;
        return result;
    }


    bool isType(TokenKind kind) {
        return kind == TokenKind::KwInteger || kind == TokenKind::KwBoolean || kind == TokenKind::KwChar || kind == TokenKind::KwString || kind == TokenKind::KwVoid;
    }

    void generateASTDiagram(const ASTNode* root, const std::string& filename) {
        std::ofstream outFile(filename);
        outFile << "```mermaid\n"; 
        outFile << "graph TD\n";  // Top-down graph
        root->toMermaid(outFile);
        outFile << "```";
        outFile.close();
    }


public:
    explicit Parser(Lexer &lexer) : lexer(lexer), debugPrefix("") {
        eatToken();
    }

    void parse() {
        std::cout << "INFO PARSE- Start parsing...\n";
        programNodeRoot = program();
        generateASTDiagram(programNodeRoot, "ast.md");
        std::cout << endl;
        if (completeAst) {
            std::cout << "INFO PARSE - Parsing completed successfully." << std::endl;
            programNodeRoot->print();
            
        } else {
            std::cout << "INFO PARSE - Parsing completed with " << errorCount << " error(s). " << std::endl;
            programNodeRoot->print();
            printErrors();
        }
    }

    // print error messages
    void printErrors() {
        for (const std::string &error : errors) {
            std::cerr << error << std::endl;
        }
    }
};

#endif // PARSER_H_