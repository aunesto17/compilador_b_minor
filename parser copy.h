#ifndef PARSER_H_
#define PARSER_H_

#include "helper.h"
#include "lexer.h"

// Clase del analizador sintáctico (parser)
class Parser {
public:
    Lexer &lexer;
    Token currentToken;

    explicit Parser(Lexer &lexer) : lexer(lexer) {
    }

    void eatToken() {
        currentToken = lexer.getNextToken();
    }

    void error() {
        std::cerr << "ERROR PARSER - Parsing failed" << std::endl;
        exit(1);
    }

    void start() {
        std::cout << "INFO PARSER - Parsing program" << std::endl;
        if(program()) {
            if(currentToken.kind == TokenKind::Eof) {
                std::cout << "INFO PARSER - Parsing completed successfully" << std::endl;
            }
            else {
                std::cout << "INFO PARSER - error" << std::endl;
                error();
            }
        }
        else {
            error();
        }
    }

    bool program() {
        // program -> Declaration Program' | ε
        eatToken();
        std::cout << "Program - token :" << currentToken.kindToString() << std::endl;
        if(declaration()) {
            return programPrime();
            if(currentToken.kind == TokenKind::Eof) {
                return true;
            }
            
        }
        return false;
    }

    bool programPrime() {
        // Program' -> Declaration Program' | ε
        std::cout << "ProgramPrime - token :" << currentToken.kindToString() << std::endl;
        if(declaration()) {
            return programPrime();
        }
        else if(currentToken.kind == TokenKind::Eof) {
            return true;
        }
        return false;
    }

    bool declaration() {
        // Declaration -> Function | VarDecl
        std::cout << "declaration"  << std::endl;
        if(function()) {
            return true;
        }
        else if(varDecl()) {
            return true;
        }
        return false;
    }

    bool function() {
        // Function -> F Type ID ( Params ) { Block }
        std::cout << "function"  << std::endl;
        if(currentToken.kind == TokenKind::KwFunction) {
            eatToken();
            std::cout << "function - token :" << currentToken.kindToString() << std::endl;
            if(type()) {
                if(currentToken.kind == TokenKind::Identifier) {
                    std::cout << "id found"  << std::endl;
                    eatToken();
                    if(currentToken.kind == TokenKind::LeftParenthesis) {
                        std::cout << "( found"  << std::endl;
                        eatToken();
                        if(params()) {
                            if(currentToken.kind == TokenKind::RightParenthesis) {
                                std::cout << ") foundd"  << std::endl;
                                eatToken();
                                if(currentToken.kind == TokenKind::LeftBrace) {
                                    std::cout << "{ found"  << std::endl;
                                    eatToken();
                                    if(stmntList()) {
                                        if(currentToken.kind == TokenKind::RightBrace) {
                                            std::cout << "} found"  << std::endl;
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
        std::cout << "Type"  << std::endl;
        std::cout << "type - token :" << currentToken.kindToString() << std::endl;
        if(currentToken.kind == TokenKind::KwInteger || 
           currentToken.kind == TokenKind::KwBoolean || 
           currentToken.kind == TokenKind::KwChar || 
           currentToken.kind == TokenKind::KwString ||
           currentToken.kind == TokenKind::KwVoid) {
            std::cout << "type found"  << std::endl;
            eatToken();
            return typePrime();
        }
        return false;
    }

    bool typePrime() {
        // Type' -> [ ] Type' | ε
        std::cout << "TypePrime"  << std::endl;
        if(currentToken.kind == TokenKind::LeftBracket) {
            eatToken();
            if(currentToken.kind == TokenKind::RightBracket) {
                eatToken();
                return typePrime();
            }
            return false;
        }
        else if(currentToken.kind == TokenKind::Identifier){
            std::cout << "id found"  << std::endl;
            return true;
        }
        return false;
    }

    bool params() {
        // Params -> Type ID Params'
        std::cout << "params"  << std::endl;
        if(type()) {
            if(currentToken.kind == TokenKind::Identifier) {
                std::cout << "id found"  << std::endl;
                eatToken();
                return paramsPrime();
            }
        }
        return true;
    }

    bool paramsPrime() {
        // Params' -> , Params | ε
        std::cout << "paramsPrime"  << std::endl;
        if(currentToken.kind == TokenKind::CommaSymbol) {
            std::cout << "comma found"  << std::endl;
            return params();
        }
        else if(currentToken.kind == TokenKind::RightParenthesis) {
            //std::cout << ") found"  << std::endl;
            return true;
        }
        return false;
    }

    bool varDecl() {
        // VarDecl -> Type ID VarDecl'
        std::cout << "varDclr"  << std::endl;
        if(type()) {
            if(currentToken.kind == TokenKind::Identifier) {
                eatToken();
                //std::cout << " varDecl - token :" << currentToken.kindToString() << std::endl;
                return varDeclPrime();
            }
        }
        return false;
    }

    bool varDeclPrime() {
        // VarDecl' -> = Expr ; | ;
        std::cout << "varDclrPrime"  << std::endl;
        std::cout << "varDeclPrime - token :" << currentToken.kindToString() << std::endl;
        if(currentToken.kind == TokenKind::Assign) {
            std::cout << "assign found"  << std::endl;
            eatToken();
            if(expr()) {
                if(currentToken.kind == TokenKind::SemiColonSymbol) {
                    eatToken();
                    return true;
                }
            }
            else if(currentToken.kind == TokenKind::SemiColonSymbol) {
                std::cout << "; found"  << std::endl;
                eatToken();
                return true;
            }
        }
        else if(currentToken.kind == TokenKind::SemiColonSymbol) {
            std::cout << "; found"  << std::endl;
            eatToken();
            return true;
        }
        return false;
    }


    bool stmntList() {
        // StmntList -> Stmnt StmntList'
        std::cout << "stmntList"  << std::endl;
        if(stmnt()) {
            return stmntListPrime();
        }
        return false;
    }

    bool stmntListPrime() {
        // StmntList' -> Stmnt StmntList' | ε
        std::cout << "stmntListPrime"  << std::endl;
        std::cout << "stmntListPrime - token :" << currentToken.kindToString() << std::endl;
        if(stmnt()) {
            return stmntListPrime();
        }
        else if(currentToken.kind == TokenKind::RightBrace) {
            std::cout << "} found"  << std::endl;
            return true;
        }
        return false;
    }

    bool stmnt() {
        // Stmnt -> Block | VarDecl | IfStmnt | WhileStmnt | ForStmnt | ReturnStmnt | PrintStmnt | Expr ;
        std::cout << "stmnt"  << std::endl;
        std::cout << "stmnt - token :" << currentToken.kindToString() << std::endl;
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
        std::cout << "ifStmnt"  << std::endl;	
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
        std::cout << "ifStmntPrime"  << std::endl;
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
        std::cout << "forStmnt"  << std::endl;
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
        std::cout << "exprStmnt"  << std::endl;
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
        std::cout << "returnStmnt"  << std::endl;
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
        std::cout << "printStmnt"  << std::endl;
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
        std::cout << "exprList"  << std::endl;
        if(expression()) {
            return exprListPrime();
        }
        return false;
    }

    bool expression(){
        // Expression -> Identifier Assign Expression | OrExpr
        std::cout << "expression"  << std::endl;
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
        std::cout << "exprListPrime"  << std::endl;
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
        std::cout << "orExpr"  << std::endl;
        if(andExpr()) {
            return orExprPrime();
        }
        return false;
    }

    bool orExprPrime(){
        // OrExpr' -> || AndExpr OrExpr' | ε
        std::cout << "orExprPrime"  << std::endl;
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
        std::cout << "andExpr"  << std::endl;
        if(EqExpr()) {
            return andExprPrime();
        }
        return false;
    }

    bool EqExpr(){
        // EqExpr -> RelExpr EqExpr'
        std::cout << "EqExpr"  << std::endl;
        if(RelExpr()) {
            return EqExprPrime();
        }
        return false;
    }

    bool EqExprPrime(){
        // EqExpr' -> == RelExpr EqExpr' | != RelExpr EqExpr' | ε
        std::cout << "EqExprPrime"  << std::endl;
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
        std::cout << "RelExpr"  << std::endl;
        if(expr()) {
            return relExprPrime();
        }
        return false;
    }

    bool relExprPrime(){
        // RelExpr' -> < RelExpr' | <= RelExpr' | > RelExpr' | >= RelExpr' | ε
        std::cout << "relExprPrime"  << std::endl;
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
        std::cout << "andExprPrime"  << std::endl;
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
        std::cout << "term"  << std::endl;
        if(unary()) {
            return termPrime();
        }
        return false;
    }

    
    bool expr(){
        // Expr -> Term Expr'
        std::cout << "expr"  << std::endl;
        std::cout << "expr - token :" << currentToken.kindToString() << std::endl;
        if(term()) {
            return exprPrime();
        }
        return false;
    }

    bool exprPrime(){
        // Expr' -> + Term Expr' | - Term Expr' | ε
        std::cout << "exprPrime"  << std::endl;
        std::cout << "exprPrime - token :" << currentToken.kindToString() << std::endl;
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
            std::cout << "exprPrime follow" << std::endl;
            return true;
        }
        return false;
    }

    bool unary(){
        // Unary -> ! Unary | - Unary | Factor
        std::cout << "unary"  << std::endl;
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
        std::cout << "factor"  << std::endl;
        std::cout << " factor - token :" << currentToken.kindToString() << std::endl;
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
        std::cout << "factorId"  << std::endl;
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
        std::cout << "factorPrime"  << std::endl;
        std::cout << "factorPrime - token :" << currentToken.kindToString() << std::endl;
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
        std::cout << "termPrime"  << std::endl;
        std::cout << "termPrime - token :" << currentToken.kindToString() << std::endl;
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
            std::cout << "termPrimeFollow" << std::endl;
            return true;
        }
    }  

};

#endif // PARSER_H_