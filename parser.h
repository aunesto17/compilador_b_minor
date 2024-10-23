#ifndef PARSER_H_
#define PARSER_H_

#include "helper.h"
#include "lexer.h"

// Clase del analizador sintáctico (parser)
class Parser {
    Lexer &lexer;
    Token currentToken;

    void eatToken() {
        currentToken = lexer.getNextToken();
    }

    void error() {
        std::cerr << "ERROR PARSER - Parsing failed" << std::endl;
        exit(1);
    }




    bool program() {
        // program -> Declaration Program' | ε
        eatToken();
        if(declaration()) {
            if(currentToken.kind == TokenKind::Eof) {
                return true;
            }
            return false;
        }
    }

    bool declaration() {
        // Declaration -> Function | VarDecl
        if(function()) {
            return true;
        }
        if(varDecl()) {
            return true;
        }
        return false;
    }

    bool function() {
        // Function -> F Type ID ( Params ) { Block }
        if(currentToken.kind == TokenKind::KwFunction) {
            eatToken();
            if(type()) {
                if(currentToken.kind == TokenKind::Identifier) {
                    eatToken();
                    if(currentToken.kind == TokenKind::LeftParenthesis) {
                        eatToken();
                        if(params()) {
                            if(currentToken.kind == TokenKind::RightParenthesis) {
                                eatToken();
                                if(currentToken.kind == TokenKind::LeftBrace) {
                                    eatToken();
                                    if(stmntList()) {
                                        if(currentToken.kind == TokenKind::RightBrace) {
                                            eatToken();
                                            return true;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    bool type() {
        // Type -> int | char | boolean | void
        if(currentToken.kind == TokenKind::KwInteger || 
           currentToken.kind == TokenKind::KwBoolean || 
           currentToken.kind == TokenKind::KwChar || 
           currentToken.kind == TokenKind::KwString ||
           currentToken.kind == TokenKind::KwVoid) {
            eatToken();
            return typePrime();
        }
        return false;
    }

    bool typePrime() {
        // Type' -> [ ] Type' | ε
        if(currentToken.kind == TokenKind::LeftBracket) {
            eatToken();
            if(currentToken.kind == TokenKind::RightBracket) {
                eatToken();
                return typePrime();
            }
            return false;
        }
        else if(currentToken.kind == TokenKind::Identifier){
            return true;
        }
        return false;
    }

    bool params() {
        // Params -> Type ID Params'
        if(type()) {
            if(currentToken.kind == TokenKind::Identifier) {
                return paramsPrime();
            }
        }
        return true;
    }

    bool paramsPrime() {
        // Params' -> , Params | ε
        if(currentToken.kind == TokenKind::CommaSymbol) {
            eatToken();
            return params();
        }
        else if(currentToken.kind == TokenKind::RightParenthesis) {
            return true;
        }
        return false;
    }

    bool varDecl() {
        // VarDecl -> Type ID VarDecl'
        if(type()) {
            if(currentToken.kind == TokenKind::Identifier) {
                eatToken();
                return varDeclPrime();
            }
        }
        return false;
    }

    bool varDeclPrime() {
        // VarDecl' -> = Expr ; | ;
        if(currentToken.kind == TokenKind::Assign) {
            eatToken();
            if(expr()) {
                if(currentToken.kind == TokenKind::SemiColonSymbol) {
                    eatToken();
                    return true;
                }
            }
        }
        else if(currentToken.kind == TokenKind::SemiColonSymbol) {
            eatToken();
            return true;
        }
        return false;
    }


    bool stmntList() {
        // StmntList -> Stmnt StmntList'
        if(stmnt()) {
            return stmntListPrime();
        }
        return false;
    }

    bool stmntListPrime() {
        // StmntList' -> Stmnt StmntList' | ε
        if(stmnt()) {
            return stmntListPrime();
        }
        else if(currentToken.kind == TokenKind::RightBrace) {
            return true;
        }
        return false;
    }

    bool stmnt() {
        // Stmnt -> Block | VarDecl | IfStmnt | WhileStmnt | ForStmnt | ReturnStmnt | PrintStmnt | Expr ;
        if(currentToken.kind == TokenKind::LeftBrace) {
            return stmntList();
        }
        else if(varDecl()) {
            return true;
        }
        else if(ifStmnt()) {
            return true;
        }
        else if(forStmnt()) {
            return true;
        }
        else if(returnStmnt()) {
            return true;
        }
        else if(printStmnt()) {
            return true;
        }
        else if(expr()) {
            if(currentToken.kind == TokenKind::SemiColonSymbol) {
                eatToken();
                return true;
            }
        }
        return false;
    }

    bool ifStmnt() {
        // IfStmnt -> if ( Expr ) { Stmnt } IfStmnt'	
        if(currentToken.kind == TokenKind::KwIf) {
            eatToken();
            if(currentToken.kind == TokenKind::LeftParenthesis) {
                eatToken();
                if(expr()) {
                    if(currentToken.kind == TokenKind::RightParenthesis) {
                        eatToken();
                        if(currentToken.kind == TokenKind::LeftBrace) {
                            eatToken();
                            if(stmnt()) {
                                if(currentToken.kind == TokenKind::RightBrace) {
                                    eatToken();
                                    return ifStmntPrime();
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    bool ifStmntPrime(){
        // IfStmnt' -> else { Stmnt } | ε
        if(currentToken.kind == TokenKind::KwElse) {
            eatToken();
            if(currentToken.kind == TokenKind::LeftBrace) {
                eatToken();
                if(stmnt()) {
                    if(currentToken.kind == TokenKind::RightBrace) {
                        eatToken();
                        return true;
                    }
                }
            }
        }
        else if(currentToken.kind == TokenKind::RightBrace ||
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
                currentToken.kind == TokenKind::KwTrue) {
            return true;
        }
        return false;
    }

    bool forStmnt(){
        // ForStmnt -> for ( ExprStmnt Expr ; ExprStmnt ) Stmnt
        if(currentToken.kind == TokenKind::KwFor) {
            eatToken();
            if(currentToken.kind == TokenKind::LeftParenthesis) {
                eatToken();
                if(exprStmnt()) {
                    if(expr()) {
                        if(currentToken.kind == TokenKind::SemiColonSymbol) {
                            eatToken();
                            if(exprStmnt()) {
                                if(currentToken.kind == TokenKind::RightParenthesis) {
                                    eatToken();
                                    return stmnt();
                                }
                            }
                        }
                    }
                }
            }
        }
        return false;
    }

    bool exprStmnt(){
        // ExprStmnt -> Expr ; | ;
        if(expr()) {
            if(currentToken.kind == TokenKind::SemiColonSymbol) {
                eatToken();
                return true;
            }
        }
        else if(currentToken.kind == TokenKind::SemiColonSymbol){
            eatToken();
            return true;
        }
        return false;
    }

    bool returnStmnt(){
        // ReturnStmnt -> return Expr ;
        if(currentToken.kind == TokenKind::KwReturn) {
            eatToken();
            if(expr()) {
                if(currentToken.kind == TokenKind::SemiColonSymbol) {
                    eatToken();
                    return true;
                }
            }
        }
        return false;
    }

    bool printStmnt(){
        // PrintStmnt -> print ( ExprList ) ;
        if(currentToken.kind == TokenKind::KwPrint) {
            eatToken();
            if(currentToken.kind == TokenKind::LeftParenthesis) {
                eatToken();
                if(exprList()) {
                    if(currentToken.kind == TokenKind::RightParenthesis) {
                        eatToken();
                        if(currentToken.kind == TokenKind::SemiColonSymbol) {
                            eatToken();
                            return true;
                        }
                    }
                }
            }
        }
        return false;
    }

    bool exprList(){
        // ExprList -> Expr ExprList'
        if(expression()) {
            return exprListPrime();
        }
        return false;
    }

    bool expression(){
        // Expression -> Identifier Assign Expression | OrExpr
        if(currentToken.kind == TokenKind::Identifier) {
            eatToken();
            if(currentToken.kind == TokenKind::Assign) {
                eatToken();
                return expression();
            }
        }
        else if(orExpr()) {
            return true;
        }
        return false;	
    }

    bool exprListPrime(){
        // ExprList' -> , ExprList | ε
        if(currentToken.kind == TokenKind::CommaSymbol) {
            eatToken();
            return exprList();
            
        }
        else if(currentToken.kind == TokenKind::RightParenthesis) {
            return true;
        }
        return false;
    }

    bool orExpr(){
        // OrExpr -> AndExpr OrExpr'
        if(andExpr()) {
            return orExprPrime();
        }
        return false;
    }

    bool orExprPrime(){
        // OrExpr' -> || AndExpr OrExpr' | ε
        if(currentToken.kind == TokenKind::LogicalOr) {
            eatToken();
            if(andExpr()) {
                return orExprPrime();
            }
        }
        else if(currentToken.kind == TokenKind::RightParenthesis || 
                currentToken.kind == TokenKind::SemiColonSymbol ||
                currentToken.kind == TokenKind::CommaSymbol) {
            return true;
        }
        return false;
    }

    bool andExpr(){
        // AndExpr -> EqExpr AndExpr'
        if(EqExpr()) {
            return andExprPrime();
        }
        return false;
    }

    bool EqExpr(){
        // EqExpr -> RelExpr EqExpr'
        if(RelExpr()) {
            return EqExprPrime();
        }
        return false;
    }

    bool EqExprPrime(){
        // EqExpr' -> == RelExpr EqExpr' | != RelExpr EqExpr' | ε
        if(currentToken.kind == TokenKind::isEqual || 
           currentToken.kind == TokenKind::NotEqual) {
            eatToken();
            return EqExprPrime();

        }
        else if(currentToken.kind == TokenKind::LogicalOr || 
                currentToken.kind == TokenKind::RightParenthesis || 
                currentToken.kind == TokenKind::SemiColonSymbol ||
                currentToken.kind == TokenKind::LogicalAnd||
                currentToken.kind == TokenKind::CommaSymbol) {
            return true;
        }
        return false;
    }

    bool RelExpr(){
        // RelExpr -> expr RelExpr'
        if(expr()) {
            return relExprPrime();
        }
        return false;
    }

    bool relExprPrime(){
        // RelExpr' -> < RelExpr' | <= RelExpr' | > RelExpr' | >= RelExpr' | ε
        if(currentToken.kind == TokenKind::LessThan || 
           currentToken.kind == TokenKind::LessThanOrEqual || 
           currentToken.kind == TokenKind::GreaterThan || 
           currentToken.kind == TokenKind::GreaterThanOrEqual) {
            eatToken();
            return relExprPrime();
           
        }
        else if(currentToken.kind == TokenKind::LogicalOr || 
                currentToken.kind == TokenKind::RightParenthesis || 
                currentToken.kind == TokenKind::SemiColonSymbol ||
                currentToken.kind == TokenKind::LogicalAnd ||
                currentToken.kind == TokenKind::isEqual ||
                currentToken.kind == TokenKind::NotEqual ||
                currentToken.kind == TokenKind::CommaSymbol) {
            return true;
        }
        return false;
    }

    bool andExprPrime(){
        // AndExpr' -> && EqExpr AndExpr' | ε
        if(currentToken.kind == TokenKind::LogicalAnd) {
            eatToken();
            if(EqExpr()) {
                return andExprPrime();
            }
        }
        else if(currentToken.kind == TokenKind::LogicalOr || 
                currentToken.kind == TokenKind::RightParenthesis || 
                currentToken.kind == TokenKind::SemiColonSymbol ||
                currentToken.kind == TokenKind::CommaSymbol) {
            return true;
        }
        return false;
    }

    bool term(){
        // Term -> Unary Term'
        if(unary()) {
            return termPrime();
        }
    }

    
    bool expr(){
        // Expr -> Term Expr'
        if(term()) {
            return exprPrime();
        }
        return false;
    }

    bool exprPrime(){
        // Expr' -> + Term Expr' | - Term Expr' | ε
        if(currentToken.kind == TokenKind::Addition || 
           currentToken.kind == TokenKind::Subtraction) {
            eatToken();
            if(term()) {
                return exprPrime();
            }
        }
        else if(currentToken.kind == TokenKind::SemiColonSymbol ||
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
            return true;
        }
        return false;
    }

    bool unary(){
        // Unary -> ! Unary | - Unary | Factor
        if(currentToken.kind == TokenKind::LogicalNot || 
           currentToken.kind == TokenKind::Subtraction) {
            eatToken();
            return unary();
        }
        else if(factor()) {
            return true;
        }
        return false;
    }

    bool factor(){
        // Factor -> Identifier FactorId | number Factor' | charVal Factor' | StringVal Factor' | kwfalse Factor' | kwtrue Factor' | ( Expr ) Factor'
        if(currentToken.kind == TokenKind::Identifier) {
            eatToken();
            return factorId();
        }
        else if(currentToken.kind == TokenKind::Number) {
            eatToken();
            return factorPrime();
        }
        else if(currentToken.kind == TokenKind::CharVal) {
            eatToken();
            return factorPrime();
        }
        else if(currentToken.kind == TokenKind::StringVal) {
            eatToken();
            return factorPrime();
        }
        else if(currentToken.kind == TokenKind::KwFalse) {
            eatToken();
            return factorPrime();
        }
        else if(currentToken.kind == TokenKind::KwTrue) {
            eatToken();
            return factorPrime();
        }
        else if(currentToken.kind == TokenKind::LeftParenthesis) {
            eatToken();
            if(expr()) {
                if(currentToken.kind == TokenKind::RightParenthesis) {
                    eatToken();
                    return factorPrime();
                }
            }
        }
        return false;
    }

    bool factorId(){
        // FactorId -> Factor' | ( Expr ) Factor'
        if(currentToken.kind == TokenKind::LeftParenthesis) {
            eatToken();
            if(expr()) {
                if(currentToken.kind == TokenKind::RightParenthesis) {
                    eatToken();
                    return factorPrime();
                }
            }
        }
        else if(currentToken.kind == TokenKind::SemiColonSymbol ||
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
                currentToken.kind == TokenKind::RightBracket) {
            return true;
        }
    }

    bool factorPrime(){
        // Factor' -> ( Expr ) | ε
        if(currentToken.kind == TokenKind::LeftParenthesis) {
            eatToken();
            if(expr()) {
                if(currentToken.kind == TokenKind::RightParenthesis) {
                    eatToken();
                    return true;
                }
            }
        }
        else if(currentToken.kind == TokenKind::SemiColonSymbol ||
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
                currentToken.kind == TokenKind::RightBracket) {
            return true;
        }
    }

    bool termPrime(){
        // Term' -> * Unary Term' | / Unary Term' | % Unary Term' | ε
        if(currentToken.kind == TokenKind::Multiplication || 
           currentToken.kind == TokenKind::Division || 
           currentToken.kind == TokenKind::Modulus) {
            eatToken();
            if(unary()) {
                return termPrime();
            }
        }
        else if(currentToken.kind == TokenKind::Addition || 
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
                currentToken.kind == TokenKind::RightBracket) {
            return true;
        }
    }  

};

#endif // PARSER_H_