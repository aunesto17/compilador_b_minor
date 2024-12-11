#ifndef AST_H_
#define AST_H_

#include "helper.h"

class ASTNode {
public:
    virtual ~ASTNode() = default;
    virtual void print(int indent = 0) const = 0;
    // generar IDs unicos a los nodos
    virtual std::string generateMermaidId() const {
        static int counter = 0;
        return "node" + std::to_string(counter++);
    }
    // metodo para generar codigo mermaid
    virtual void toMermaid(std::ostream& out, const std::string& parentId = "") const = 0;
};

// Base Declaration node class
class DeclNode : virtual public ASTNode {
public:
    virtual ~DeclNode() = default;
};

class StmtNode : virtual public ASTNode {
public:
    virtual ~StmtNode() = default;
};

// Program node class - top level container for all declarations
class ProgramNode : public ASTNode {
private:
    std::vector<DeclNode*> declarations;

public:
    ~ProgramNode() {
        for (auto decl : declarations) {
            delete decl;
        }
    }

    void addDeclaration(DeclNode* decl) {
        if (decl) declarations.push_back(decl);
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Program:\n";
        for (const auto& decl : declarations) {
            decl->print(indent + 2);
        }
    }

    const std::vector<DeclNode*>& getDeclarations() const { return declarations; }

    void toMermaid(std::ostream& out, const std::string& parentId = "") const override {
        std::string myId = generateMermaidId();
        out << myId << "[Program]\n";
        if (!parentId.empty()) {
            out << parentId << " --> " << myId << "\n";
        }
        for (const auto& decl : declarations) {
            decl->toMermaid(out, myId);
        }
    }
};

class ExprNode : public ASTNode {
public:
    virtual ~ExprNode() = default;
};

// Type node class
class TypeNode : public ASTNode {
private:
    std::string baseType;
    std::vector<ExprNode*> arrayDimensions;  // For array types

public:
    TypeNode(const std::string& type) : baseType(type) {}
    
    void addArrayDimension(ExprNode* dim) {
        if (dim) arrayDimensions.push_back(dim);
    }

    ~TypeNode() {
        for (auto dim : arrayDimensions) {
            delete dim;
        }
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Type: " << baseType;
        if (!arrayDimensions.empty()) {
            std::cout << " (array with " << arrayDimensions.size() << " dimensions)\n";
            for (const auto& dim : arrayDimensions) {
                if (dim) {
                    dim->print(indent + 2);
                } else {
                    std::cout << std::string(indent + 2, ' ') << "[]";
                }
            }
        } else {
            std::cout << "\n";
        }
    }

    const std::string& getBaseType() const { return baseType; }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[Type: " << baseType << "]\n";
        out << parentId << " --> " << myId << "\n";
        
        for (const auto& dim : arrayDimensions) {
            if (dim) {
                dim->toMermaid(out, myId);
            }
        }   
    }
};



// Variable Declaration node class
class VarDeclNode : public DeclNode, public StmtNode {
private:
    TypeNode* type;
    std::string identifier;
    ExprNode* initializer;  // Optional

public:
    VarDeclNode(TypeNode* type_, const std::string& id, ExprNode* init = nullptr)
        : type(type_), identifier(id), initializer(init) {}
    
    ~VarDeclNode() {
        delete type;
        delete initializer;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "VarDecl: " << identifier << "\n";
        type->print(indent + 2);
        if (initializer) {
            std::cout << std::string(indent + 2, ' ') << "Initializer:\n";
            initializer->print(indent + 4);
        }
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[VarDecl: " << identifier << "]\n";
        out << parentId << " --> " << myId << "\n";
        
        // Add type
        std::string typeId = generateMermaidId();
        out << typeId << "[Type: " << type->getBaseType() << "]\n";
        out << myId << " --> " << typeId << "\n";
        
        // Add initializer if present
        if (initializer) {
            initializer->toMermaid(out, myId);
        }
    }

    void toMermaidF(std::ostream& out, const std::string& parentId) {
        std::string myId = generateMermaidId();
        out << myId << "[VarDecl: " << identifier << "]\n";
        out << parentId << " -->|\"parameter\"| " << myId << "\n";
        
        // Add type
        std::string typeId = generateMermaidId();
        out << typeId << "[Type: " << type->getBaseType() << "]\n";
        out << myId << " --> " << typeId << "\n";
        
        // Add initializer if present
        if (initializer) {
            initializer->toMermaid(out, myId);
        }
    }
    
};


// Literal expression node
class LiteralExprNode : public ExprNode {
private:
    std::string type;
    std::string value;

public:
    LiteralExprNode(const std::string& type, const std::string& val) 
        : type(type), value(val) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') 
                  << "LiteralExpr: " << type << " = " << value << "\n";
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[Literal: " << value << "]\n";
        out << parentId << " --> " << myId << "\n";
    }
};

// Binary operation expression node
class BinaryExprNode : public ExprNode {
private:
    std::string op;
    ExprNode* left;
    ExprNode* right;

public:
    BinaryExprNode(const std::string& operation, ExprNode* leftExpr, ExprNode* rightExpr)
        : op(operation), left(leftExpr), right(rightExpr) {}

    ~BinaryExprNode() {
        delete left;
        delete right;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "BinaryExpr: " << op << "\n";
        std::cout << std::string(indent + 2, ' ') << "Left:\n";
        left->print(indent + 4);
        std::cout << std::string(indent + 2, ' ') << "Right:\n";
        right->print(indent + 4);
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[BinaryExpr: " << op << "]\n";
        out << parentId << " --> " << myId << "\n";
        
        left->toMermaid(out, myId);
        right->toMermaid(out, myId);
    }
};

// Assignment expression node
class AssignExprNode : public ExprNode {
private:
    std::string identifier;
    std::string type;
    ExprNode* value;

public:
    AssignExprNode(const std::string& id, const std::string& type, ExprNode* expr)
        : identifier(id), type(type), value(expr) {}

    ~AssignExprNode() {
        delete value;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "AssignExpr: " 
                  << type << " " << identifier << "\n";
        std::cout << std::string(indent + 2, ' ') <<     "Value:\n";
        value->print(indent + 4);
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[Assign: " << identifier << "]\n";
        out << parentId << " --> " << myId << "\n";
        
        value->toMermaid(out, myId);
    }
};

class UnaryExprNode : public ExprNode {
private:
    std::string op;
    ExprNode* expr;

public:
    UnaryExprNode(const std::string& operator_, ExprNode* expression)
        : op(operator_), expr(expression) {}
    
    ~UnaryExprNode() { delete expr; }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "UnaryExpr: " << op << "\n";
        expr->print(indent + 2);
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[UnaryOp: " << op << "]\n";
        out << parentId << " --> " << myId << "\n";
        
        expr->toMermaid(out, myId);
    }
};

class FunctionCallNode : public ExprNode {
private:
    std::string identifier;
    std::vector<ExprNode*> arguments;

public:
    FunctionCallNode(const std::string& id, const std::vector<ExprNode*>& args)
        : identifier(id), arguments(args) {}
    
    ~FunctionCallNode() {
        for (auto arg : arguments) delete arg;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "FunctionCall: " << identifier << "\n";
        for (const auto& arg : arguments) {
            arg->print(indent + 2);
        }
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[FunctionCall: " << identifier << "]\n";
        out << parentId << " --> " << myId << "\n";
        
        for (const auto& arg : arguments) {
            arg->toMermaid(out, myId);
        }
    }
};

class ArrayAccessNode : public ExprNode {
private:
    std::string identifier;
    std::vector<ExprNode*> indices;

public:
    ArrayAccessNode(const std::string& id, const std::vector<ExprNode*>& idx)
        : identifier(id), indices(idx) {}
    
    ~ArrayAccessNode() {
        for (auto index : indices) delete index;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "ArrayAccess: " << identifier << "\n";
        for (const auto& index : indices) {
            index->print(indent + 2);
        }
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[ArrayAccess: " << identifier << "]\n";
        out << parentId << " --> " << myId << "\n";
        
        for (const auto& index : indices) {
            index->toMermaid(out, myId);
        }
    }
};

class IdentifierNode : public ExprNode {
private:
    std::string name;

public:
    IdentifierNode(const std::string& identifier) : name(identifier) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Identifier: " << name << "\n";
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[Identifier: " << name << "]\n";
        out << parentId << " --> " << myId << "\n";
    }
};

// Function Declaration node
class FunctionDeclNode : public DeclNode {
private:
    TypeNode* returnType;
    std::string identifier;
    std::vector<VarDeclNode*> parameters;
    std::vector<StmtNode*> body;

public:
    FunctionDeclNode(TypeNode* type, const std::string& id)
        : returnType(type), identifier(id) {}
    
    ~FunctionDeclNode() {
        delete returnType;
        for (auto param : parameters) delete param;
        for (auto stmt : body) delete stmt;
    }

    void addParameter(VarDeclNode* param) {
        if (param) parameters.push_back(param);
    }

    void addStatement(StmtNode* stmt) {
        if (stmt) body.push_back(stmt);
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "FunctionDecl: " << identifier << "\n";
        returnType->print(indent + 2);
        std::cout << std::string(indent + 2, ' ') << "Parameters:\n";
        for (const auto& param : parameters) {
            param->print(indent + 4);
        }
        std::cout << std::string(indent + 2, ' ') << "Body:\n";
        for (const auto& stmt : body) {
            stmt->print(indent + 4);
        }
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[Function: " << identifier << "]\n";
        out << parentId << " --> " << myId << "\n";
        // Add return type
        returnType->toMermaid(out, myId);
        // Add parameters
        std::string paramId = generateMermaidId();
        out << paramId << "[Parameters]\n";
        out << myId << " --> " << paramId << "\n";
        
        for (const auto& param : parameters) {
            param->toMermaid(out, paramId);
        }
        // add body node
        std::string bodyId = generateMermaidId();
        out << bodyId << "[Body]\n";
        out << myId << " --> " << bodyId << "\n";
        
        for (const auto& stmt : body) {
            stmt->toMermaid(out, bodyId);
        }
    }
};

// Statement node classes
class BlockStmtNode : public StmtNode {
private:
    std::vector<StmtNode*> statements;

public:
    ~BlockStmtNode() {
        for (auto stmt : statements) delete stmt;
    }

    void addStatement(StmtNode* stmt) {
        if (stmt) statements.push_back(stmt);
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Block:\n";
        for (const auto& stmt : statements) {
            stmt->print(indent + 2);
        }
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[Block]\n";
        out << parentId << " --> " << myId << "\n";
        
        for (const auto& stmt : statements) {
            stmt->toMermaid(out, myId);
        }
    }
};

// class IfStmtNode : public StmtNode {
// private:
//     ExprNode* condition;
//     StmtNode* thenBranch;
//     StmtNode* elseBranch;  // Optional

// public:
//     IfStmtNode(ExprNode* cond, StmtNode* thenStmt, StmtNode* elseStmt = nullptr)
//         : condition(cond), thenBranch(thenStmt), elseBranch(elseStmt) {}
    
//     ~IfStmtNode() {
//         delete condition;
//         delete thenBranch;
//         delete elseBranch;
//     }

//     void print(int indent = 0) const override {
//         std::cout << std::string(indent, ' ') << "If:\n";
//         std::cout << std::string(indent + 2, ' ') << "Condition:\n";
//         condition->print(indent + 4);
//         std::cout << std::string(indent + 2, ' ') << "Then:\n";
//         thenBranch->print(indent + 4);
//         if (elseBranch) {
//             std::cout << std::string(indent + 2, ' ') << "Else:\n";
//             elseBranch->print(indent + 4);
//         }
//     }

//     void toMermaid(std::ostream& out, const std::string& parentId) const override {
//         std::string myId = generateMermaidId();
//         out << myId << "[If]\n";
//         out << parentId << " --> " << myId << "\n";
        
//         condition->toMermaid(out, myId);
//         thenBranch->toMermaid(out, myId);
//         if (elseBranch) {
//             elseBranch->toMermaid(out, myId);
//         }
//     }
// };

class ForStmtNode : public StmtNode {
private:
    StmtNode* initialization;  // Can be VarDeclNode or ExprNode
    ExprNode* condition;
    StmtNode* increment;
    StmtNode* body;

public:
    ForStmtNode(StmtNode* init, ExprNode* cond, StmtNode* inc, StmtNode* bodyStmt)
        : initialization(init), condition(cond), increment(inc), body(bodyStmt) {}
    
    ~ForStmtNode() {
        delete initialization;
        delete condition;
        delete increment;
        delete body;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "For:\n";
        if (initialization) {
            std::cout << std::string(indent + 2, ' ') << "Initialization:\n";
            initialization->print(indent + 4);
        }
        std::cout << std::string(indent + 2, ' ') << "Condition:\n";
        condition->print(indent + 4);
        if (increment) {
            std::cout << std::string(indent + 2, ' ') << "Increment:\n";
            increment->print(indent + 4);
        }
        std::cout << std::string(indent + 2, ' ') << "Body:\n";
        body->print(indent + 4);
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[For]\n";
        out << parentId << " --> " << myId << "\n";
        
        if (initialization) {
            initialization->toMermaid(out, myId);
        }
        condition->toMermaid(out, myId);
        if (increment) {
            increment->toMermaid(out, myId);
        }
        body->toMermaid(out, myId);
    }
};

class WhileStmtNode : public StmtNode {
private:
    ExprNode* condition;
    StmtNode* body;

public:
    WhileStmtNode(ExprNode* cond, StmtNode* bodyStmt)
        : condition(cond), body(bodyStmt) {}
    
    ~WhileStmtNode() {
        delete condition;
        delete body;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "While:\n";
        std::cout << std::string(indent + 2, ' ') << "Condition:\n";
        condition->print(indent + 4);
        std::cout << std::string(indent + 2, ' ') << "Body:\n";
        body->print(indent + 4);
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[While]\n";
        out << parentId << " --> " << myId << "\n";
        
        condition->toMermaid(out, myId);
        body->toMermaid(out, myId);
    }
};

class ReturnStmtNode : public StmtNode {
private:
    ExprNode* returnValue;  // Optional, can be nullptr for void returns

public:
    ReturnStmtNode(ExprNode* value = nullptr) : returnValue(value) {}
    
    ~ReturnStmtNode() {
        delete returnValue;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Return:\n";
        if (returnValue) {
            returnValue->print(indent + 2);
        }
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[Return]\n";
        out << parentId << " --> " << myId << "\n";
        
        if (returnValue) {
            returnValue->toMermaid(out, myId);
        }
    }
};

class PrintStmtNode : public StmtNode {
private:
    std::vector<ExprNode*> expressions;

public:
    PrintStmtNode(const std::vector<ExprNode*>& exprs) : expressions(exprs) {}
    
    ~PrintStmtNode() {
        for (auto expr : expressions) {
            delete expr;
        }
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "Print:\n";
        for (const auto& expr : expressions) {
            expr->print(indent + 2);
        }
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[Print]\n";
        out << parentId << " --> " << myId << "\n";
        
        for (const auto& expr : expressions) {
            expr->toMermaid(out, myId);
        }
    }
};

class ExprStmtNode : public StmtNode {
private:
    ExprNode* expression;  // Optional, can be nullptr for empty statements

public:
    ExprStmtNode(ExprNode* expr = nullptr) : expression(expr) {}
    
    ~ExprStmtNode() {
        delete expression;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "ExprStmt:\n";
        if (expression) {
            expression->print(indent + 2);
        }
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[ExprStmt]\n";
        out << parentId << " --> " << myId << "\n";
        
        if (expression) {
            expression->toMermaid(out, myId);
        }
    }
};

class IfStmtNode : public StmtNode {
private:
    ExprNode* condition;
    BlockStmtNode* thenBranch;
    BlockStmtNode* elseBranch;  // Optional, can be nullptr

public:
    IfStmtNode(ExprNode* cond, BlockStmtNode* thenBlock, BlockStmtNode* elseBlock = nullptr)
        : condition(cond), thenBranch(thenBlock), elseBranch(elseBlock) {}
    
    ~IfStmtNode() {
        delete condition;
        delete thenBranch;
        delete elseBranch;
    }

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "If Statement:\n";
        std::cout << std::string(indent + 2, ' ') << "Condition:\n";
        condition->print(indent + 4);
        std::cout << std::string(indent + 2, ' ') << "Then Branch:\n";
        thenBranch->print(indent + 4);
        if (elseBranch) {
            std::cout << std::string(indent + 2, ' ') << "Else Branch:\n";
            elseBranch->print(indent + 4);
        }
    }

    void toMermaid(std::ostream& out, const std::string& parentId) const override {
        std::string myId = generateMermaidId();
        out << myId << "[If]\n";
        out << parentId << " --> " << myId << "\n";

        std::string condId = generateMermaidId();
        out << condId << "[Condition]\n";
        out << myId << " --> " << condId << "\n";
        condition->toMermaid(out, condId);

        std::string thenId = generateMermaidId();
        out << thenId << "[Then]\n";
        out << myId << " --> " << thenId << "\n";
        thenBranch->toMermaid(out, thenId);

        if (elseBranch) {
            std::string elseId = generateMermaidId();
            out << elseId << "[Else]\n";
            out << myId << " --> " << elseId << "\n";
            elseBranch->toMermaid(out, elseId);
        }
    }
};

#endif // AST_H_