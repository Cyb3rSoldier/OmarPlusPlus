#ifndef OMAR_AST_H
#define OMAR_AST_H

#include <memory>
#include <string>
#include <vector>

#include "token.h"

// ---------------------------------------------------------------
// Types of the language
// ---------------------------------------------------------------
enum class Type {
    INT,      // purno
    FLOAT,    // dosomik
    STRING,   // lekha
    BOOL,     // sottoMitha
    LIST,     // talika
    VOID,     // a function that returns nothing
    UNKNOWN   // not known until runtime (e.g. an element taken from a talika)
};

inline std::string typeName(Type t) {
    switch (t) {
        case Type::INT:     return "purno";
        case Type::FLOAT:   return "dosomik";
        case Type::STRING:  return "lekha";
        case Type::BOOL:    return "sottoMitha";
        case Type::LIST:    return "talika";
        case Type::VOID:    return "nothing";
        case Type::UNKNOWN: return "unknown";
    }
    return "unknown";
}

// ---------------------------------------------------------------
// Expressions
// ---------------------------------------------------------------
enum class ExprKind {
    INT_LIT, FLOAT_LIT, STRING_LIT, BOOL_LIT, LIST_LIT,
    VARIABLE, INDEX, UNARY, BINARY, CALL
};

struct Expr {
    ExprKind kind;
    int line;
    Expr(ExprKind k, int ln) : kind(k), line(ln) {}
    virtual ~Expr() = default;
};
using ExprPtr = std::unique_ptr<Expr>;

struct IntExpr : Expr {
    long long value;
    IntExpr(long long v, int ln) : Expr(ExprKind::INT_LIT, ln), value(v) {}
};

struct FloatExpr : Expr {
    double value;
    FloatExpr(double v, int ln) : Expr(ExprKind::FLOAT_LIT, ln), value(v) {}
};

struct StringExpr : Expr {
    std::string value;
    StringExpr(std::string v, int ln) : Expr(ExprKind::STRING_LIT, ln), value(std::move(v)) {}
};

struct BoolExpr : Expr {
    bool value;
    BoolExpr(bool v, int ln) : Expr(ExprKind::BOOL_LIT, ln), value(v) {}
};

struct ListExpr : Expr {
    std::vector<ExprPtr> elements;
    explicit ListExpr(int ln) : Expr(ExprKind::LIST_LIT, ln) {}
};

struct VarExpr : Expr {
    std::string name;
    VarExpr(std::string n, int ln) : Expr(ExprKind::VARIABLE, ln), name(std::move(n)) {}
};

struct IndexExpr : Expr {
    ExprPtr base, index;
    IndexExpr(ExprPtr b, ExprPtr i, int ln)
        : Expr(ExprKind::INDEX, ln), base(std::move(b)), index(std::move(i)) {}
};

// Unary operators: MINUS (-x), MULOK (square root), NOI (not)
struct UnaryExpr : Expr {
    TokenType op;
    std::string opName;
    ExprPtr operand;
    UnaryExpr(TokenType o, std::string name, ExprPtr e, int ln)
        : Expr(ExprKind::UNARY, ln), op(o), opName(std::move(name)), operand(std::move(e)) {}
};

struct BinaryExpr : Expr {
    TokenType op;
    std::string opName;  // the original word, used in error messages
    ExprPtr left, right;
    BinaryExpr(TokenType o, std::string name, ExprPtr l, ExprPtr r, int ln)
        : Expr(ExprKind::BINARY, ln), op(o), opName(std::move(name)),
          left(std::move(l)), right(std::move(r)) {}
};

struct CallExpr : Expr {
    std::string name;
    std::vector<ExprPtr> args;
    CallExpr(std::string n, int ln) : Expr(ExprKind::CALL, ln), name(std::move(n)) {}
};

// ---------------------------------------------------------------
// Statements
// ---------------------------------------------------------------
enum class StmtKind {
    VAR_DECL, ASSIGN, PRINT, INPUT, IF, FOR, WHILE,
    BREAK, CONTINUE, RETURN, FUNC_DECL, EXPR, INCDEC, BLOCK, EXIT
};

struct Stmt {
    StmtKind kind;
    int line;
    Stmt(StmtKind k, int ln) : kind(k), line(ln) {}
    virtual ~Stmt() = default;
};
using StmtPtr = std::unique_ptr<Stmt>;
using Block = std::vector<StmtPtr>;

struct VarDeclStmt : Stmt {
    Type type = Type::INT;
    std::string name;
    bool isConst = false;
    ExprPtr init;  // may be null (default value is used)
    explicit VarDeclStmt(int ln) : Stmt(StmtKind::VAR_DECL, ln) {}
};

struct AssignStmt : Stmt {
    ExprPtr target;  // VarExpr or IndexExpr
    ExprPtr value;
    explicit AssignStmt(int ln) : Stmt(StmtKind::ASSIGN, ln) {}
};

struct PrintStmt : Stmt {
    std::vector<ExprPtr> values;
    explicit PrintStmt(int ln) : Stmt(StmtKind::PRINT, ln) {}
};

struct InputStmt : Stmt {
    bool hasPrompt = false;
    std::string prompt;
    std::string name;
    explicit InputStmt(int ln) : Stmt(StmtKind::INPUT, ln) {}
};

struct IfStmt : Stmt {
    std::vector<ExprPtr> conditions;  // jodi, onnothayjodi, onnothayjodi, ...
    std::vector<Block> blocks;        // one block per condition
    bool hasElse = false;
    Block elseBlock;                  // nahole
    explicit IfStmt(int ln) : Stmt(StmtKind::IF, ln) {}
};

struct ForStmt : Stmt {
    std::string var;
    ExprPtr from, to;
    Block body;
    explicit ForStmt(int ln) : Stmt(StmtKind::FOR, ln) {}
};

struct WhileStmt : Stmt {
    ExprPtr condition;
    Block body;
    explicit WhileStmt(int ln) : Stmt(StmtKind::WHILE, ln) {}
};

struct BreakStmt : Stmt {
    explicit BreakStmt(int ln) : Stmt(StmtKind::BREAK, ln) {}
};

struct ContinueStmt : Stmt {
    explicit ContinueStmt(int ln) : Stmt(StmtKind::CONTINUE, ln) {}
};

struct ReturnStmt : Stmt {
    ExprPtr value;  // may be null
    explicit ReturnStmt(int ln) : Stmt(StmtKind::RETURN, ln) {}
};

struct Param {
    Type type;
    std::string name;
    int line;
};

struct FuncDeclStmt : Stmt {
    Type returnType = Type::VOID;
    std::string name;
    std::vector<Param> params;
    Block body;
    explicit FuncDeclStmt(int ln) : Stmt(StmtKind::FUNC_DECL, ln) {}
};

struct ExprStmt : Stmt {
    ExprPtr expr;  // always a CallExpr
    explicit ExprStmt(int ln) : Stmt(StmtKind::EXPR, ln) {}
};

struct IncDecStmt : Stmt {
    bool increment = true;  // barao = true, komao = false
    ExprPtr target;
    explicit IncDecStmt(int ln) : Stmt(StmtKind::INCDEC, ln) {}
};

struct BlockStmt : Stmt {
    Block body;
    explicit BlockStmt(int ln) : Stmt(StmtKind::BLOCK, ln) {}
};

struct ExitStmt : Stmt {
    explicit ExitStmt(int ln) : Stmt(StmtKind::EXIT, ln) {}
};

struct Program {
    Block statements;
};

#endif
