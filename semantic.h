#ifndef OMAR_SEMANTIC_H
#define OMAR_SEMANTIC_H

#include <string>
#include <unordered_map>
#include <vector>

#include "ast.h"

// Phase 3: semantic analysis. Walks the AST BEFORE the program runs and checks:
//   - undeclared variables / functions
//   - redeclaration in the same scope
//   - assignment to a constant (drubok) or to a ghuro loop variable
//   - type mismatches (e.g. storing a lekha in a purno)
//   - thamo / chaliyejao outside a loop
//   - ferot outside a function, and wrong return values
//   - wrong number or type of function arguments
// A stack of scopes (symbol tables) is used for variable lookup.
class SemanticAnalyzer {
public:
    void analyze(const Program& program);

private:
    struct Symbol {
        Type type;
        bool isConst;
        bool isLoopVar;
    };
    struct FuncSig {
        Type ret = Type::VOID;
        std::vector<Type> params;
    };

    std::vector<std::unordered_map<std::string, Symbol>> scopes;
    std::unordered_map<std::string, FuncSig> functions;
    const FuncSig* currentFunction = nullptr;
    std::string currentFunctionName;
    int loopDepth = 0;

    [[noreturn]] void fail(int line, const std::string& message) const;

    Symbol* lookup(const std::string& name);
    static bool assignable(Type target, Type source);
    static const VarExpr* lvalueRoot(const Expr& expr);

    void analyzeBlock(const Block& block);   // statements in the CURRENT scope
    void analyzeScoped(const Block& block);  // statements in a NEW scope
    void analyzeStmt(const Stmt& stmt);
    void checkWritable(const Expr& target, int line);

    Type typeOf(const Expr& expr);
    Type valueType(const Expr& expr);  // like typeOf, but a 'nothing' result is an error
    Type binaryType(const BinaryExpr& expr);
    Type callType(const CallExpr& expr);
};

#endif
