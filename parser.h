#ifndef PARSER_H_
#define PARSER_H_

#include "helper.h"
#include "lexer.h"

// Clase del analizador sintáctico (parser)
class Parser {
private:
    bool incompleteAST = false;
    bool hasError = false;

    bool isFollowFactor() {
        // std::cout << "isFollowFactor" << std::endl;
        //std::cout << "\t isFollowFactor - token :" << currentToken.kindToString() << std::endl;

        return currentToken.kind == TokenKind::SemiColonSymbol ||
               currentToken.kind == TokenKind::RightParenthesis ||
               currentToken.kind == TokenKind::CommaSymbol ||
               currentToken.kind == TokenKind::LogicalOr || 
               currentToken.kind == TokenKind::LogicalAnd || 
               currentToken.kind == TokenKind::isEqual || 
               currentToken.kind == TokenKind::NotEqual ||
               currentToken.kind == TokenKind::LessThan || 
               currentToken.kind == TokenKind::LessThanOrEqual || 
               currentToken.kind == TokenKind::GreaterThan || 
               currentToken.kind == TokenKind::GreaterThanOrEqual || 
               currentToken.kind == TokenKind::Addition || 
               currentToken.kind == TokenKind::Subtraction ||
               currentToken.kind == TokenKind::Multiplication ||
               currentToken.kind == TokenKind::Division ||
               currentToken.kind == TokenKind::Modulus ||    
               currentToken.kind == TokenKind::RightBracket;
    }

    bool isFollowEqExpr() {
        // std::cout << "isFollowEq" << std::endl;
        // std::cout << "\t isFollowEq - token :" << currentToken.kindToString() << std::endl;
        return currentToken.kind == TokenKind::LogicalOr || 
               currentToken.kind == TokenKind::RightParenthesis || 
               currentToken.kind == TokenKind::SemiColonSymbol ||
               currentToken.kind == TokenKind::LogicalAnd ||
               currentToken.kind == TokenKind::CommaSymbol;
    }

    bool isFollowTerm() {
        // std::cout << "isFollowTerm" << std::endl;
        // std::cout << "\t isFollowTerm - token :" << currentToken.kindToString() << std::endl;
        return currentToken.kind == TokenKind::Addition || 
               currentToken.kind == TokenKind::Subtraction || 
               currentToken.kind == TokenKind::SemiColonSymbol || 
               currentToken.kind == TokenKind::RightParenthesis || 
               currentToken.kind == TokenKind::CommaSymbol || 
               currentToken.kind == TokenKind::LogicalOr || 
               currentToken.kind == TokenKind::LogicalAnd || 
               currentToken.kind == TokenKind::isEqual || 
               currentToken.kind == TokenKind::NotEqual || 
               currentToken.kind == TokenKind::LessThan || 
               currentToken.kind == TokenKind::LessThanOrEqual || 
               currentToken.kind == TokenKind::GreaterThan || 
               currentToken.kind == TokenKind::GreaterThanOrEqual || 
               currentToken.kind == TokenKind::RightBracket;
    }

    bool isFolllowifStmntPrime(){
        // std::cout << "isFolllowifStmntPrime" << std::endl;
        // std::cout << "\t isFolllowifStmntPrime - token :" << currentToken.kindToString() << std::endl;
        return currentToken.kind == TokenKind::RightBrace ||
                currentToken.kind == TokenKind::LeftBrace ||
                currentToken.kind == TokenKind::KwInteger ||
                currentToken.kind == TokenKind::KwBoolean ||
                currentToken.kind == TokenKind::KwChar ||
                currentToken.kind == TokenKind::KwString ||
                currentToken.kind == TokenKind::KwVoid ||
                currentToken.kind == TokenKind::KwIf ||   
                currentToken.kind == TokenKind::KwFor || 
                currentToken.kind == TokenKind::KwReturn || 
                currentToken.kind == TokenKind::KwPrint || 
                currentToken.kind == TokenKind::LogicalNot || //!
                currentToken.kind == TokenKind::Subtraction ||
                currentToken.kind == TokenKind::Identifier ||  
                currentToken.kind == TokenKind::Number || 
                currentToken.kind == TokenKind::StringVal || 
                currentToken.kind == TokenKind::CharVal || 
                currentToken.kind == TokenKind::LeftParenthesis || 
                currentToken.kind == TokenKind::KwFalse || 
                currentToken.kind == TokenKind::KwTrue;
    }

     // Define synchronization token sets
    const std::set<TokenKind> statementStartSet = {
        TokenKind::KwIf,
        TokenKind::KwFor,
        TokenKind::KwReturn,
        TokenKind::KwPrint,
        TokenKind::LeftBrace,
        TokenKind::KwInteger,
        TokenKind::KwBoolean,
        TokenKind::KwChar,
        TokenKind::KwString,
        TokenKind::KwVoid
    };

    const std::set<TokenKind> declarationStartSet = {
        TokenKind::KwFunction,
        TokenKind::KwInteger,
        TokenKind::KwBoolean,
        TokenKind::KwChar,
        TokenKind::KwString,
        TokenKind::KwVoid
    };

    const std::set<TokenKind> expressionStartSet = {
        TokenKind::Identifier,
        TokenKind::Number,
        TokenKind::CharVal,
        TokenKind::StringVal,
        TokenKind::KwTrue,
        TokenKind::KwFalse,
        TokenKind::LeftParenthesis,
        TokenKind::LogicalNot,
        TokenKind::Subtraction
    };

    // Helper method to check if token is a type
    bool isType(TokenKind kind) {
        return kind == TokenKind::KwInteger ||
               kind == TokenKind::KwBoolean ||
               kind == TokenKind::KwChar ||
               kind == TokenKind::KwString ||
               kind == TokenKind::KwVoid;
    }

    void reportError(const std::string& message) {
        hasError = true;
        std::cerr << "ERROR at " << currentToken.location.printLoc() << ": " << message << std::endl;
    }

    void skipUntil(const std::set<TokenKind>& synchronizationSet) {
        std::cout << "Skipping tokens until synchronization point..." << std::endl;
        while (currentToken.kind != TokenKind::Eof && 
               synchronizationSet.find(currentToken.kind) == synchronizationSet.end()) {
            eatToken();
        }
    }

    // error recovery
    // arreglar este syncronize para comer hasta antes del ;
    void synchronize() {
        std::cout << "INFO PARSER - Synchronizing to next semicolon..." << std::endl;
        while (currentToken.kind != TokenKind::Eof) {
            if (currentToken.kind == TokenKind::SemiColonSymbol) {
                // eatToken(); // Consume the semicolon
                return;
            }
            eatToken();
        }
    }

    void handleError(const std::string& message) {
        reportError(message);
        synchronize();
    }

    bool hadError() const {
        return hasError;
    }

public:
    Lexer &lexer;
    Token currentToken;
    Token lastToken;

    explicit Parser(Lexer &lexer) : lexer(lexer) {
        eatToken();
        lastToken = currentToken;
    }

    // backtracks to the last token inserting the last token to the front of the token vector 
    void backtrack() {
        lexer.insertToken(currentToken);
        lexer.insertToken(lastToken);
        //std::cout << "INFO PARSER - Backtracking to token: " << lastToken.kindToString() << std::endl;
        eatToken();
    }

    void eatToken() {
        lastToken = currentToken;
        //std::cout << "INFO PARSER - Consuming token: " << currentToken.kindToString() << std::endl;
        // currentToken = lexer.getNextToken();
        // get next token from vector of tokens in lexer
        currentToken = lexer.getToken();
        //std::cout << "INFO PARSER - Next token: " << currentToken.kindToString() << std::endl;	

    }

    void error() {
        std::cerr << "ERROR PARSER - Parsing failed" << std::endl;
        exit(1);
    }

    void start() {
        std::cout << "INFO PARSER - Parsing program" << std::endl;
        if(program()) {
            if(!hasError) {
                std::cout << "INFO PARSER - Parsing completed successfully" << std::endl;
            } else {
                std::cout << "INFO PARSER - Parsing completed with errors" << std::endl;
            }
        } else {
            std::cout << "INFO PARSER - Parsing failed" << std::endl;
        }
    }

    bool program() {
        // Program -> Declaration Program' $
        // std::cout << "Program - token: " << currentToken.kindToString() << std::endl;
        
        if(!declaration()) {
            reportError("Expected declaration at program start");
            return false;
        }
        
        if(!programPrime()) {
            return false;
        }
        
        // Check for end of file ($)
        if(currentToken.kind != TokenKind::Eof) {
            reportError("Expected end of file");
            return false;
        }
        
        return true;
    }

    bool programPrime() {
        // Program' -> Declaration Program' | ε
        // std::cout << "program prime"  << std::endl;
        // std::cout << "ProgramPrime - token :" << currentToken.kindToString() << std::endl;

        // Check for ε case first - if we see EOF, it's valid
        if(currentToken.kind == TokenKind::Eof) {
            return true;
        }
        
        // Otherwise, we should see another declaration
        if(!declaration()) {
            return false;  // declaration() will report the specific error
        }
        
        // Recursively handle the rest of the program
        return programPrime();
    }

    bool declaration() {
        // Declaration -> Function | VarDecl
        // std::cout << "declaration"  << std::endl;
        // std::cout << "\t declaration - token :" << currentToken.kindToString() << std::endl;
        if(currentToken.kind == TokenKind::KwFunction) {
            return function();
        }
        else if(isType(currentToken.kind)) {
            return varDecl();
        }

        //synchronize("declaration");
        return false;
    }

    bool function() {
        // Function -> F Type ID ( Params ) { stmntList }
        // std::cout << "function"  << std::endl;
        // std::cout << "\t function - token :" << currentToken.kindToString() << std::endl;

        if(currentToken.kind != TokenKind::KwFunction){
            //reportError("Expected 'function' keyword");
            return false;
        }

        eatToken();

        if(!type()) {
            reportError("Expected function return type");
            return false;
        }

        if (currentToken.kind != TokenKind::Identifier) {
            reportError("Expected function name");
            //synchronize("declaration");
            return false;
        }

        eatToken();
        
        if (currentToken.kind != TokenKind::LeftParenthesis) {
            reportError("Expected '('");
            //synchronize("declaration");
            return false;
        }
        
        eatToken();
        params(); // Optional parameters
        
        if (currentToken.kind != TokenKind::RightParenthesis) {
            reportError("Expected ')'");
            //synchronize("statement");
            return false;
        }
        
        eatToken();
        
        if (currentToken.kind != TokenKind::LeftBrace) {
            reportError("Expected '{'");
            //synchronize("statement");
            return false;
        }
        
        eatToken();
        // std::cout << "\t function  antes de stmntList - token:" << currentToken.kindToString() << std::endl;
        stmntList();
        // std::cout << "\t function  despues de stmntList - token:" << currentToken.kindToString() << std::endl;

        if (currentToken.kind != TokenKind::RightBrace) {
            reportError("Expected '}'");
            //synchronize("declaration");
            return false;
        }
        
        eatToken();
        return true;
    }

    bool type() {
        // Type -> int | char | boolean | void
        // std::cout << "Type"  << std::endl;
        // std::cout << "\t type - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind == TokenKind::KwInteger || 
           currentToken.kind == TokenKind::KwBoolean || 
           currentToken.kind == TokenKind::KwChar || 
           currentToken.kind == TokenKind::KwString ||
           currentToken.kind == TokenKind::KwVoid) {
            // std::cout << "type found"  << std::endl;
            eatToken();
            return typePrime();
        }
        reportError("Expected type specifier");
        return false;
    }

    bool typePrime() {
        // Type' -> [ ] Type' | ε
        // std::cout << "TypePrime"  << std::endl;
        // std::cout << "\t typePrime - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind == TokenKind::LeftBracket) {
            // Handle array type
            eatToken();
            
            if(currentToken.kind != TokenKind::RightBracket) {
                reportError("Expected ']' in array type declaration");
                return false;
            }
            
            eatToken();
            return typePrime(); // Handle multi-dimensional arrays
        }
        
        // Not an array type, must be identifier or other valid follower
        return true;
    }

    bool params() {
        // Params -> Type ID Params'
        // std::cout << "params"  << std::endl;
        // std::cout << "\t params - token :" << currentToken.kindToString() << std::endl;
        
        // Check for empty parameter list first (ε case)
        if(currentToken.kind == TokenKind::RightParenthesis) {
            return true;
        }

        // If not empty, must start with a type
        if(!type()) {
            return false;
        }

        if(currentToken.kind != TokenKind::Identifier) {
            reportError("Expected parameter name");
            return false;
        }

        eatToken();
        return paramsPrime();
    }

    bool paramsPrime() {
        // Params' -> , Params | ε
        // std::cout << "paramsPrime"  << std::endl;
        // std::cout << "\t paramsPrime - token :" << currentToken.kindToString() << std::endl;
        if(currentToken.kind == TokenKind::CommaSymbol) {
            // std::cout << "comma found"  << std::endl;
            eatToken();
            return params();
        }
        else if(currentToken.kind == TokenKind::RightParenthesis) {
            // std::cout << ") found"  << std::endl;
            return true;
        }
        reportError("Expected ',' or ')'");
        return false;
    }

    bool varDecl() {
        // VarDecl -> Type ID VarDecl'
        // std::cout << "varDclr"  << std::endl;
        // std::cout << "\t varDecl - token :" << currentToken.kindToString() << std::endl;
        
        if(!type()) {
            reportError("Expected type in variable declaration");
            return false;
        }

        if(currentToken.kind != TokenKind::Identifier) {
            reportError("Expected identifier in variable declaration");
            return false;
        }

        eatToken();
        return varDeclPrime();
    }

    bool varDeclPrime() {
        // VarDecl' -> = Expr ; | ;
        // std::cout << "varDclrPrime"  << std::endl;
        // std::cout << "\t varDeclPrime - token :" << currentToken.kindToString() << std::endl;
        if(currentToken.kind == TokenKind::Assign) {
            // std::cout << "assign found"  << std::endl;
            eatToken();
            if(!expression()) {
                reportError("Invalid expression in variable initialization");
                return false;
            }    
        }
        if(currentToken.kind != TokenKind::SemiColonSymbol) {
            reportError("Expected ';' after variable declaration");
            return false;
        }

        eatToken();
        return true;
    }

    bool stmntList() {
        // StmntList -> Stmnt StmntList'
        // std::cout << "stmntList"  << std::endl;
        // std::cout << "\t stmntList - token :" << currentToken.kindToString() << std::endl;
        if(!stmnt()) {
            synchronize();
            //return false;
            //continue;
        }
        std::cout << "\t stmntList - token :" << currentToken.kindToString() << std::endl;

        return stmntListPrime();
    }

    bool stmntListPrime() {
        // StmntList' -> Stmnt StmntList' | ε
        // std::cout << "stmntListPrime"  << std::endl;
        // std::cout << "\t stmntListPrime - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind == TokenKind::RightBrace) { // follow, vacio
            // std::cout << "follow stmntlistprime"  << std::endl;
            // std::cout << "} found"  << std::endl;
            return true;
        }
        if(stmnt()) {
            return stmntListPrime();
        }
        return false;
    }

    bool stmnt() {
        // Stmnt -> VarDecl	| IfStmnt | ForStmnt | ReturnStmnt | PrintStmnt	| ExprStmnt | { StmntList }
        // std::cout << "stmnt"  << std::endl;
        // std::cout << "\t stmnt - token :" << currentToken.kindToString() << std::endl;
        
        switch(currentToken.kind) {
            case TokenKind::LeftBrace: 
            {
                // Handle block: { StmntList }
                eatToken();
                if(!stmntList()) {
                    reportError("Invalid statement list in block");
                    return false;
                }
                if(currentToken.kind != TokenKind::RightBrace) {
                    reportError("Expected '}' at end of block");
                    return false;
                }
                eatToken();
                return true;
            }

            case TokenKind::KwInteger:
            case TokenKind::KwBoolean:
            case TokenKind::KwChar:
            case TokenKind::KwString:
            case TokenKind::KwVoid: {
                // Handle variable declaration
                return varDecl();
            }

            case TokenKind::KwIf:
                return ifStmnt();

            case TokenKind::KwFor:
                return forStmnt();

            case TokenKind::KwReturn:
                return returnStmnt();

            case TokenKind::KwPrint:
                return printStmnt();

            default:
                // Must be an expression statement
                return exprStmnt();
        }
    }

    bool ifStmnt() {
        // IfStmnt -> if ( expression ) { Stmnt } IfStmnt'
        // std::cout << "ifStmnt"  << std::endl;
        // std::cout << "\t ifStmnt - token :" << currentToken.kindToString() << std::endl;
        if(currentToken.kind != TokenKind::KwIf) {
            return false;
        }
        
        eatToken();
        if(currentToken.kind != TokenKind::LeftParenthesis) {
            reportError("Expected '(' after 'if'");
            return false;
        }
        
        eatToken();
        if(!expression()) {
            reportError("Expected expression in if condition");
            return false;
        }
        
        if(currentToken.kind != TokenKind::RightParenthesis) {
            reportError("Expected ')' after if condition");
            return false;
        }
        
        eatToken();
        if(currentToken.kind != TokenKind::LeftBrace) {
            reportError("Expected '{' after if condition");
            return false;
        }
        
        eatToken();
        if(!stmnt()) {
            reportError("Invalid statement in if body");
            return false;
        }
        
        if(currentToken.kind != TokenKind::RightBrace) {
            reportError("Expected '}' after if body");
            return false;
        }
        
        eatToken();
        return ifStmntPrime();
    }

    bool ifStmntPrime(){
        // IfStmnt' -> else { Stmnt } | ε
        // std::cout << "ifStmntPrime"  << std::endl;
        // std::cout << "\t ifStmntPrime - token :" << currentToken.kindToString() << std::endl;
       
        if(isFolllowifStmntPrime()) {
            return true; // ε case
        }
        
        eatToken();
        if(currentToken.kind != TokenKind::LeftBrace) {
            reportError("Expected '{' after 'else'");
            return false;
        }
        
        eatToken();
        if(!stmnt()) {
            reportError("Invalid statement in else body");
            return false;
        }
        
        if(currentToken.kind != TokenKind::RightBrace) {
            reportError("Expected '}' after else body");
            return false;
        }
        
        eatToken();
        return true;
    }

    bool forStmnt(){
        // ForStmnt -> for ( ExprStmnt expression ; ExprStmnt ) Stmnt
        // std::cout << "forStmnt"  << std::endl;
        // std::cout << "\t forStmnt - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind != TokenKind::KwFor) {
            return false;
        }
        
        eatToken();
        if(currentToken.kind != TokenKind::LeftParenthesis) {
            reportError("Expected '(' after 'for'");
            return false;
        }
        
        eatToken();
        if(!exprStmnt()) {
            reportError("Invalid initialization in for loop");
            return false;
        }
        
        if(!expression()) {
            reportError("Invalid condition in for loop");
            return false;
        }
        
        if(currentToken.kind != TokenKind::SemiColonSymbol) {
            reportError("Expected ';' after for loop condition");
            return false;
        }
        
        eatToken();
        if(!exprStmnt()) {
            reportError("Invalid increment expression in for loop");
            return false;
        }
        
        if(currentToken.kind != TokenKind::RightParenthesis) {
            reportError("Expected ')' after for loop header");
            return false;
        }
        
        eatToken();
        if(currentToken.kind != TokenKind::LeftBrace) {
            reportError("Expected '{' before for loop body");
            return false;
        }
        
        eatToken();
        if(!stmnt()) {
            reportError("Invalid statement in for loop body");
            return false;
        }
        
        if(currentToken.kind != TokenKind::RightBrace) {
            reportError("Expected '}' after for loop body");
            return false;
        }
        
        eatToken();
        return true;
    }

    bool exprStmnt(){
        // ExprStmnt -> expression ; | ;
        // std::cout << "exprStmnt"  << std::endl;
        // std::cout << "\t exprStmnt - token :" << currentToken.kindToString() << std::endl;
        if(currentToken.kind == TokenKind::SemiColonSymbol) {
            eatToken();
            return true;
        }
        
        if(!expression()) {
            return false;
        }
        
        if(currentToken.kind != TokenKind::SemiColonSymbol) {
            reportError("Expected ';' after expression");
            return false;
        }
        
        eatToken();
        return true;
    }

    bool returnStmnt(){
        // ReturnStmnt	→ 'return' Expression ;
        // std::cout << "returnStmnt"  << std::endl;
        // std::cout << "\t returnStmnt - token :" << currentToken.kindToString() << std::endl;
    
        if(currentToken.kind != TokenKind::KwReturn) {
            return false;
        }
        
        eatToken();
        if(!expression()) {
            reportError("Expected expression after 'return'");
            return false;
        }
        
        if(currentToken.kind != TokenKind::SemiColonSymbol) {
            reportError("Expected ';' after return expression");
            return false;
        }
        
        eatToken();
        return true;
    }

    bool printStmnt(){
        // PrintStatement -> 'print' ( ExprList ) ;
        // std::cout << "printStmnt"  << std::endl;
        // std::cout << " \t printStmnt - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind != TokenKind::KwPrint) {
            return false;
        }
        
        eatToken();
        if(currentToken.kind != TokenKind::LeftParenthesis) {
            reportError("Expected '(' after 'print'");
            return false;
        }
        
        eatToken();
        if(!exprList()) {
            reportError("Invalid expression list in print statement");
            return false;
        }
        
        if(currentToken.kind != TokenKind::RightParenthesis) {
            reportError("Expected ')' after print arguments");
            return false;
        }
        
        eatToken();
        if(currentToken.kind != TokenKind::SemiColonSymbol) {
            reportError("Expected ';' after print statement");
            return false;
        }
        
        eatToken();
        return true;
    }

    bool exprList(){
        // ExprList -> Expression ExprList’
        // std::cout << "exprList"  << std::endl;
        // std::cout << " \t exprList - token :" << currentToken.kindToString() << std::endl;
        
        if(!expression()) {
            return false;
        }
        return exprListPrime();
    }

    bool expression(){
        // Expression -> Identifier = Expression | OrExpr
        // std::cout << "expression"  << std::endl;
        // std::cout << " \t expression - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind == TokenKind::Identifier) {
            eatToken();
            // std::cout << " \t expression - token :" << currentToken.kindToString() << std::endl;

            if(currentToken.kind == TokenKind::Assign) {
                eatToken();
                if(!expression()) {
                    reportError("Invalid expression after assignment");
                    return false;
                }
                return true;
            }
            
            backtrack(); // Backtrack
        }
        
        return orExpr();	
    }

    bool exprListPrime(){
        // ExprList’ -> , ExprList | ε
        // std::cout << "exprListPrime"  << std::endl;
        // std::cout << " \t exprListPrime - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind != TokenKind::CommaSymbol) {
            return true; // ε case
        }
        
        eatToken();
        return exprList();
    }

    bool orExpr(){
        // OrExpr -> AndExpr OrExpr'
        // std::cout << "orExpr"  << std::endl;
        // std::cout << " \t orExpr - token :" << currentToken.kindToString() << std::endl;
        
        if(!andExpr()) {
            return false;
        }
        return orExprPrime();
    }

    bool orExprPrime(){
        // OrExpr' -> || AndExpr OrExpr' | ε
        // std::cout << "orExprPrime"  << std::endl;
        // std::cout << " \t orExprPrime - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind != TokenKind::LogicalOr) {
            if(currentToken.kind == TokenKind::RightParenthesis || // empty case
                currentToken.kind == TokenKind::SemiColonSymbol ||
                currentToken.kind == TokenKind::CommaSymbol) {
                return true;
            }
            reportError("Expected follow orExprPrime '), ;, ,'");
            return true; // ε case
        }

        eatToken();
        if(!andExpr()) {
            reportError("Expected expression after '||'");
            return false;
        }
        return orExprPrime();
    }

    bool andExpr(){
        // AndExpr -> EqExpr AndExpr'
        // std::cout << "andExpr"  << std::endl;
        // std::cout << " \t andExpr - token :" << currentToken.kindToString() << std::endl;
        if(!EqExpr()) {
            return false;
        }
        return andExprPrime();
    }

    bool EqExpr(){
        // EqExpr -> RelExpr EqExpr'
        // std::cout << "EqExpr"  << std::endl;
        // std::cout << " \t EqExpr - token :" << currentToken.kindToString() << std::endl;
        
        if(!RelExpr()) {
            return false;   
        }
        return EqExprPrime();
    }

    bool EqExprPrime(){
        // EqExpr' -> == RelExpr EqExpr' | != RelExpr EqExpr' | ε
        // std::cout << "EqExprPrime"  << std::endl;
        // std::cout << " \t EqExprPrime - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind != TokenKind::isEqual && 
           currentToken.kind != TokenKind::NotEqual) {
            if(currentToken.kind == TokenKind::LogicalOr || 
                currentToken.kind == TokenKind::RightParenthesis || 
                currentToken.kind == TokenKind::SemiColonSymbol ||
                currentToken.kind == TokenKind::LogicalAnd||
                currentToken.kind == TokenKind::CommaSymbol) {
                return true;
            }
            reportError("Expected '==, !=, ), ;, ,'");  
            return false;
        }
        TokenKind op = currentToken.kind;
        eatToken();
    
        if(!RelExpr()) {
            reportError("Expected expression after comparison operator");
            return false;
        }
        return EqExprPrime();
    }

    bool RelExpr(){
        // RelExpr -> expr RelExpr'
        // std::cout << "RelExpr"  << std::endl;
        // std::cout << " \t RelExpr - token :" << currentToken.kindToString() << std::endl;
        
        if(!expr()) {
            return false;
        }
        return relExprPrime();
    }

    bool relExprPrime(){
        // RelExpr' -> < RelExpr' | <= RelExpr' | > RelExpr' | >= RelExpr' | ε
        // std::cout << "relExprPrime"  << std::endl;
        // std::cout << " relExprPrime - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind != TokenKind::LessThan && 
            currentToken.kind != TokenKind::LessThanOrEqual &&
            currentToken.kind != TokenKind::GreaterThan && 
            currentToken.kind != TokenKind::GreaterThanOrEqual) {
            if(currentToken.kind == TokenKind::LogicalOr || 
                currentToken.kind == TokenKind::RightParenthesis || 
                currentToken.kind == TokenKind::SemiColonSymbol ||
                currentToken.kind == TokenKind::LogicalAnd ||
                currentToken.kind == TokenKind::isEqual ||
                currentToken.kind == TokenKind::NotEqual ||
                currentToken.kind == TokenKind::CommaSymbol ||
                currentToken.kind == TokenKind::Number) {
                return true; // ε case
            }
            reportError("Expected '<, <=, >, >=, ), ;, ,'");
            return false;
        }

        TokenKind op = currentToken.kind;
        eatToken();
    
        if(!expr()) {
            reportError("Expected expression after relational operator");
            return false;
        }
        return relExprPrime();
    }

    bool andExprPrime(){
        // AndExpr' -> && EqExpr AndExpr' | ε
        // std::cout << "andExprPrime"  << std::endl;
        // std::cout << " andExprPRime - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind != TokenKind::LogicalAnd) { // empty case
            if(currentToken.kind == TokenKind::LogicalOr || 
                currentToken.kind == TokenKind::RightParenthesis || 
                currentToken.kind == TokenKind::SemiColonSymbol ||
                currentToken.kind == TokenKind::CommaSymbol) {
                return true;
            }
            reportError("Expected '&&, ||, ), ;, ,'");
            return false;
        }

        eatToken();
        if(!EqExpr()) {
            reportError("Expected expression after '&&'");
            return false;
        }
        return andExprPrime();
    }

    bool term(){
        // Term -> Unary Term'
        // std::cout << "term"  << std::endl;
        // std::cout << " term - token :" << currentToken.kindToString() << std::endl;
        
        if(!unary()) {
            return false;
        }
        return termPrime();
    }

    bool expr(){
        // Expr -> Term Expr'
        // std::cout << "expr"  << std::endl;
        // std::cout << "expr - token :" << currentToken.kindToString() << std::endl;
        
        if(!term()) {
            return false;
        }
        return exprPrime();
    }

    bool exprPrime(){
        // Expr' -> + Term Expr' | - Term Expr' | ε
        // std::cout << "exprPrime"  << std::endl;
        // std::cout << "exprPrime - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind != TokenKind::Addition && 
            currentToken.kind != TokenKind::Subtraction) {
            if(currentToken.kind == TokenKind::SemiColonSymbol ||
                currentToken.kind == TokenKind::RightParenthesis ||
                currentToken.kind == TokenKind::CommaSymbol ||
                currentToken.kind == TokenKind::LogicalOr || 
                currentToken.kind == TokenKind::LogicalAnd || 
                currentToken.kind == TokenKind::isEqual || 
                currentToken.kind == TokenKind::NotEqual ||
                currentToken.kind == TokenKind::LessThan || 
                currentToken.kind == TokenKind::LessThanOrEqual || 
                currentToken.kind == TokenKind::GreaterThan || 
                currentToken.kind == TokenKind::GreaterThanOrEqual || 
                currentToken.kind == TokenKind::RightBracket) {
                return true; // ε case
            }
            reportError("Expected '+, -, ), ;, ,'");
            return false; 
        }
        TokenKind op = currentToken.kind;
        eatToken();
        
        if(!term()) {
            reportError("Expected term after arithmetic operator");
            return false;
        }
        return exprPrime();
    }

    bool unary(){
        // Unary -> ! Unary | - Unary | Factor
        // std::cout << "unary"  << std::endl;
        // std::cout << "unary - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind == TokenKind::LogicalNot || 
            currentToken.kind == TokenKind::Subtraction) {
            TokenKind op = currentToken.kind;
            eatToken();
            if(!unary()) {
                reportError("Expected expression after unary operator");
                return false;
            }
            return true;
        }
        
        return factor();
    }

    bool factor(){
        // Factor -> Identifier FactorId | number Factor' | charVal Factor' | StringVal Factor' | kwfalse Factor' | kwtrue Factor' | ( Expr ) Factor'
        // std::cout << "factor"  << std::endl;
        // std::cout << " \t factor - token :" << currentToken.kindToString() << std::endl;
        
        switch(currentToken.kind) {
            case TokenKind::Identifier: {
                eatToken();
                // std::cout << " \t factor - token :" << currentToken.kindToString() << std::endl;
                return factorId();
            }
            case TokenKind::Number:
            case TokenKind::CharVal:
            case TokenKind::StringVal:
            case TokenKind::KwFalse:
            case TokenKind::KwTrue: {
                eatToken();
                return factorPrime();
            }
            case TokenKind::LeftParenthesis: {
                eatToken();
                if(!expr()) {
                    reportError("Invalid expression in parentheses");
                    return false;
                }
                
                if(currentToken.kind != TokenKind::RightParenthesis) {
                    reportError("Expected ')'");
                    return false;
                }
                
                eatToken();
                return factorPrime();
            }
            default:
                reportError("Expected factor");
                return false;
        }
    }

    bool factorId(){
        // FactorId -> Factor' | ( Expr ) Factor'
        // std::cout << "factorId"  << std::endl;
        // std::cout << " \t factorId - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind == TokenKind::LeftParenthesis) {
            eatToken();
            if(!exprList()) {
                reportError("Invalid expression list in function call");
                return false;
            }
            
            if(currentToken.kind != TokenKind::RightParenthesis) {
                reportError("Expected ')'");
                return false;
            }
            
            eatToken();
        }
        return factorPrime();
    }

    bool factorPrime(){
        // Factor' -> [ expression ] | ε
        // std::cout << "factorPrime"  << std::endl;
        // std::cout << "\t factorPrime - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind != TokenKind::LeftBracket) {
            if(isFollowFactor()) {
                return true; // ε case
            }
            reportError("Expected '[, ), ;, ,'");
            return false;
        }
        
        eatToken();
        if(!expression()) {
            reportError("Invalid expression");
            return false;
        }
        
        if(currentToken.kind != TokenKind::RightBracket) {
            reportError("Expected ']'");
            return false;
        }
        
        eatToken();
        return factorPrime();
    }

    bool termPrime(){
        // Term' -> * Unary Term' | / Unary Term' | % Unary Term' | ε
        // std::cout << "termPrime"  << std::endl;
        // std::cout << "termPrime - token :" << currentToken.kindToString() << std::endl;
        
        if(currentToken.kind != TokenKind::Multiplication && 
            currentToken.kind != TokenKind::Division && 
            currentToken.kind != TokenKind::Modulus) {
            if(isFollowTerm()) {
                return true; // ε case
            }
            reportError("Expected '*, /, %, +, -, ), ;, ,'");
            return false;
        }
        
        TokenKind op = currentToken.kind;
        eatToken();
        
        if(!unary()) {
            reportError("Expected expression after operator");
            return false;
        }
        return termPrime();
    }  
};

#endif // PARSER_H_