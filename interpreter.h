#ifndef OMAR_INTERPRETER_H
#define OMAR_INTERPRETER_H

#include <cstdint>
#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "ast.h"

// A runtime value.
struct Value;
using ListPtr = std::shared_ptr<std::vector<Value>>;

struct Value {
    Type type = Type::VOID;  // INT, FLOAT, STRING, BOOL, LIST or VOID
    long long i = 0;         // purno
    double d = 0;            // dosomik
    bool b = false;         // sottoMitha
    std::string s;           // lekha
    ListPtr list;            // talika (shared: copies of a talika refer to the same list)

    static Value makeInt(long long v);
    static Value makeFloat(double v);
    static Value makeString(std::string v);
    static Value makeBool(bool v);
    static Value makeList(ListPtr v);

    // quoteText: put "" around lekha values (used when printing list elements)
    std::string toString(bool quoteText = false, int depth = 0) const;
};

// Phase 4: tree-walking interpreter. Executes the checked AST.
class Interpreter {
public:
    void run(const Program& program);

private:
    enum class Signal { NONE, BREAK, CONTINUE, RETURN };

    struct Var {
        Value value;
        Type type;  // declared type of the variable
    };
    using Scope = std::unordered_map<std::string, Var>;
    struct Frame {
        std::vector<Scope> scopes;  // innermost scope is at the back
    };
    struct LRef {  // a place that can be assigned to
        Value* ptr;
        Type type;  // declared type (UNKNOWN for talika elements)
    };

    std::deque<Frame> frames;  // frames[0] = global frame, others = function calls
    std::unordered_map<std::string, const FuncDeclStmt*> functions;
    const FuncDeclStmt* currentFunction = nullptr;
    Value returnValue;
    std::uintptr_t stackBase = 0;  // address near the top of the C++ stack when run() started

    // scopes and variables
    Var* lookup(const std::string& name);
    void declare(const std::string& name, Type type, Value value);
    LRef resolve(const Expr& target);

    // statements (the bigger ones live in their own functions to keep each C++ stack frame small)
    Signal executeBlock(const Block& block);
    Signal executeScoped(const Block& block);
    Signal execute(const Stmt& stmt);
    void execPrint(const PrintStmt& stmt);
    void execInput(const InputStmt& stmt);
    Signal execFor(const ForStmt& stmt);
    void execIncDec(const IncDecStmt& stmt);
    bool condition(const Expr& expr);

    // expressions
    Value evaluate(const Expr& expr);
    Value evalUnary(const UnaryExpr& expr);
    Value evalBinary(const BinaryExpr& expr);
    Value evalIndex(const IndexExpr& expr);
    Value evalList(const ListExpr& expr);
    Value evalCall(const CallExpr& expr);
    Value evalBuiltin(const CallExpr& expr);
};

#endif
