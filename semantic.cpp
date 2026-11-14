#include "semantic.h"

namespace {

bool isNumber(Type t) { return t == Type::INT || t == Type::FLOAT; }
bool numberOrUnknown(Type t) { return isNumber(t) || t == Type::UNKNOWN; }

// result type of arithmetic on two numbers
Type arithmeticType(Type l, Type r) {
    if (l == Type::UNKNOWN || r == Type::UNKNOWN) {
        return (l == Type::FLOAT || r == Type::FLOAT) ? Type::FLOAT : Type::UNKNOWN;
    }
    return (l == Type::INT && r == Type::INT) ? Type::INT : Type::FLOAT;
}

}  // namespace

void SemanticAnalyzer::fail(int line, const std::string& message) const {
    throw OmarError("Semantic Error", line, message);
}

SemanticAnalyzer::Symbol* SemanticAnalyzer::lookup(const std::string& name) {
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) return &found->second;
    }
    return nullptr;
}

// Can a value of type 'source' be stored in a variable of type 'target'?
bool SemanticAnalyzer::assignable(Type target, Type source) {
    if (target == Type::UNKNOWN || source == Type::UNKNOWN) return true;  // checked at runtime
    if (target == source) return true;
    return target == Type::FLOAT && source == Type::INT;  // purno -> dosomik is allowed
}

// For x or x[1][2] returns the VarExpr 'x'; otherwise nullptr.
const VarExpr* SemanticAnalyzer::lvalueRoot(const Expr& expr) {
    const Expr* e = &expr;
    while (e->kind == ExprKind::INDEX) e = static_cast<const IndexExpr*>(e)->base.get();
    return e->kind == ExprKind::VARIABLE ? static_cast<const VarExpr*>(e) : nullptr;
}

// ---------------------------------------------------------------
// entry point
// ---------------------------------------------------------------
void SemanticAnalyzer::analyze(const Program& program) {
    scopes.clear();
    functions.clear();
    currentFunction = nullptr;
    loopDepth = 0;
    scopes.emplace_back();  // global scope

    // Pass 1: register every top-level function so they can be called
    // before their definition (and call each other / themselves).
    for (const StmtPtr& s : program.statements) {
        if (s->kind != StmtKind::FUNC_DECL) continue;
        const auto& f = static_cast<const FuncDeclStmt&>(*s);

        if (f.name == "dorgho" || f.name == "jukto") {
            fail(f.line, "'" + f.name + "' is a built-in function and cannot be redefined");
        }
        if (functions.count(f.name)) {
            fail(f.line, "function '" + f.name + "' is already defined");
        }
        FuncSig sig;
        sig.ret = f.returnType;
        for (const Param& p : f.params) sig.params.push_back(p.type);
        functions[f.name] = sig;
    }

    // Pass 2: check everything.
    analyzeBlock(program.statements);
}

void SemanticAnalyzer::analyzeBlock(const Block& block) {
    for (const StmtPtr& s : block) analyzeStmt(*s);
}

void SemanticAnalyzer::analyzeScoped(const Block& block) {
    scopes.emplace_back();
    analyzeBlock(block);
    scopes.pop_back();
}

// Checks that the target of an assignment / increment may be changed.
void SemanticAnalyzer::checkWritable(const Expr& target, int line) {
    const VarExpr* root = lvalueRoot(target);
    if (!root) fail(line, "can only change a variable or an element of a variable");

    Symbol* sym = lookup(root->name);
    if (!sym) fail(root->line, "undeclared variable '" + root->name + "'");
    if (sym->isLoopVar) fail(line, "cannot change the loop variable '" + root->name + "'");
    if (sym->isConst) fail(line, "cannot change the constant '" + root->name + "'");

    if (target.kind == ExprKind::INDEX) {
        const auto& ix = static_cast<const IndexExpr&>(target);
        if (typeOf(*ix.base) == Type::STRING) {
            fail(line, "the characters of a lekha cannot be changed");
        }
    }
}

// ---------------------------------------------------------------
// statements
// ---------------------------------------------------------------
void SemanticAnalyzer::analyzeStmt(const Stmt& stmt) {
    switch (stmt.kind) {
        case StmtKind::VAR_DECL: {
            const auto& d = static_cast<const VarDeclStmt&>(stmt);
            if (scopes.back().count(d.name)) {
                fail(d.line, "variable '" + d.name + "' is already declared in this scope");
            }
            if (d.init) {
                Type t = valueType(*d.init);
                if (!assignable(d.type, t)) {
                    fail(d.line, "cannot store " + typeName(t) + " in " + typeName(d.type) +
                                 " variable '" + d.name + "'");
                }
            }
            scopes.back()[d.name] = {d.type, d.isConst, false};
            break;
        }

        case StmtKind::ASSIGN: {
            const auto& a = static_cast<const AssignStmt&>(stmt);
            checkWritable(*a.target, a.line);
            Type targetType = typeOf(*a.target);
            Type valueT = valueType(*a.value);
            if (!assignable(targetType, valueT)) {
                const VarExpr* root = lvalueRoot(*a.target);
                fail(a.line, "cannot assign " + typeName(valueT) + " to " + typeName(targetType) +
                             " variable '" + root->name + "'");
            }
            break;
        }

        case StmtKind::PRINT: {
            const auto& p = static_cast<const PrintStmt&>(stmt);
            for (const ExprPtr& e : p.values) valueType(*e);
            break;
        }

        case StmtKind::INPUT: {
            const auto& in = static_cast<const InputStmt&>(stmt);
            Symbol* sym = lookup(in.name);
            if (!sym) fail(in.line, "undeclared variable '" + in.name + "'");
            if (sym->isLoopVar) fail(in.line, "cannot change the loop variable '" + in.name + "'");
            if (sym->isConst) fail(in.line, "cannot change the constant '" + in.name + "'");
            if (sym->type == Type::LIST) fail(in.line, "'nao' cannot read into a talika");
            break;
        }

        case StmtKind::IF: {
            const auto& s = static_cast<const IfStmt&>(stmt);
            for (size_t k = 0; k < s.conditions.size(); k++) {
                Type t = valueType(*s.conditions[k]);
                if (t != Type::BOOL && t != Type::UNKNOWN) {
                    fail(s.conditions[k]->line,
                         "the condition must be sottoMitha (true/false), but found " + typeName(t));
                }
                analyzeScoped(s.blocks[k]);
            }
            if (s.hasElse) analyzeScoped(s.elseBlock);
            break;
        }

        case StmtKind::FOR: {
            const auto& f = static_cast<const ForStmt&>(stmt);
            Type from = valueType(*f.from);
            Type to = valueType(*f.to);
            if ((from != Type::INT && from != Type::UNKNOWN) || (to != Type::INT && to != Type::UNKNOWN)) {
                fail(f.line, "the range of 'ghuro' must use purno values, but found " +
                             typeName(from) + " and " + typeName(to));
            }
            scopes.emplace_back();
            scopes.back()[f.var] = {Type::INT, true, true};
            loopDepth++;
            analyzeBlock(f.body);
            loopDepth--;
            scopes.pop_back();
            break;
        }

        case StmtKind::WHILE: {
            const auto& w = static_cast<const WhileStmt&>(stmt);
            Type t = valueType(*w.condition);
            if (t != Type::BOOL && t != Type::UNKNOWN) {
                fail(w.condition->line,
                     "the condition must be sottoMitha (true/false), but found " + typeName(t));
            }
            loopDepth++;
            analyzeScoped(w.body);
            loopDepth--;
            break;
        }

        case StmtKind::BREAK:
            if (loopDepth == 0) fail(stmt.line, "'thamo' can only be used inside a loop");
            break;

        case StmtKind::CONTINUE:
            if (loopDepth == 0) fail(stmt.line, "'chaliyejao' can only be used inside a loop");
            break;

        case StmtKind::RETURN: {
            const auto& r = static_cast<const ReturnStmt&>(stmt);
            if (!currentFunction) fail(r.line, "'ferot' can only be used inside a function (kaj)");

            if (r.value) {
                if (currentFunction->ret == Type::VOID) {
                    fail(r.line, "function '" + currentFunctionName +
                                 "' returns nothing, so 'ferot' cannot give a value");
                }
                Type t = valueType(*r.value);
                if (!assignable(currentFunction->ret, t)) {
                    fail(r.line, "function '" + currentFunctionName + "' must return " +
                                 typeName(currentFunction->ret) + ", but this returns " + typeName(t));
                }
            } else if (currentFunction->ret != Type::VOID) {
                fail(r.line, "function '" + currentFunctionName + "' must return a value of type " +
                             typeName(currentFunction->ret));
            }
            break;
        }

        case StmtKind::FUNC_DECL: {
            const auto& f = static_cast<const FuncDeclStmt&>(stmt);
            if (scopes.size() != 1 || currentFunction) {
                fail(f.line, "functions can only be defined at the top level of the program");
            }
            const FuncSig* sig = &functions.at(f.name);

            scopes.emplace_back();  // parameters + body share one scope
            for (const Param& p : f.params) {
                if (scopes.back().count(p.name)) {
                    fail(p.line, "duplicate parameter name '" + p.name + "'");
                }
                scopes.back()[p.name] = {p.type, false, false};
            }

            int savedLoopDepth = loopDepth;
            loopDepth = 0;
            currentFunction = sig;
            currentFunctionName = f.name;

            analyzeBlock(f.body);

            currentFunction = nullptr;
            currentFunctionName.clear();
            loopDepth = savedLoopDepth;
            scopes.pop_back();
            break;
        }

        case StmtKind::EXPR: {
            const auto& e = static_cast<const ExprStmt&>(stmt);
            typeOf(*e.expr);  // a call used as a statement may return nothing
            break;
        }

        case StmtKind::INCDEC: {
            const auto& s = static_cast<const IncDecStmt&>(stmt);
            checkWritable(*s.target, s.line);
            Type t = typeOf(*s.target);
            if (!numberOrUnknown(t)) {
                fail(s.line, std::string("'") + (s.increment ? "barao" : "komao") +
                             "' needs a purno or dosomik variable, but found " + typeName(t));
            }
            break;
        }

        case StmtKind::BLOCK:
            analyzeScoped(static_cast<const BlockStmt&>(stmt).body);
            break;

        case StmtKind::EXIT:
            break;
    }
}

// ---------------------------------------------------------------
// expressions
// ---------------------------------------------------------------
Type SemanticAnalyzer::valueType(const Expr& expr) {
    Type t = typeOf(expr);
    if (t == Type::VOID) {
        fail(expr.line, "this function returns nothing, so it cannot be used as a value");
    }
    return t;
}

Type SemanticAnalyzer::typeOf(const Expr& expr) {
    switch (expr.kind) {
        case ExprKind::INT_LIT:    return Type::INT;
        case ExprKind::FLOAT_LIT:  return Type::FLOAT;
        case ExprKind::STRING_LIT: return Type::STRING;
        case ExprKind::BOOL_LIT:   return Type::BOOL;

        case ExprKind::LIST_LIT: {
            const auto& l = static_cast<const ListExpr&>(expr);
            for (const ExprPtr& e : l.elements) valueType(*e);
            return Type::LIST;
        }

        case ExprKind::VARIABLE: {
            const auto& v = static_cast<const VarExpr&>(expr);
            Symbol* sym = lookup(v.name);
            if (!sym) fail(v.line, "undeclared variable '" + v.name + "'");
            return sym->type;
        }

        case ExprKind::INDEX: {
            const auto& ix = static_cast<const IndexExpr&>(expr);
            Type base = valueType(*ix.base);
            Type index = valueType(*ix.index);
            if (base != Type::LIST && base != Type::STRING && base != Type::UNKNOWN) {
                fail(ix.line, "only a talika or a lekha can be indexed, but this is " + typeName(base));
            }
            if (index != Type::INT && index != Type::UNKNOWN) {
                fail(ix.line, "an index must be purno, but found " + typeName(index));
            }
            return base == Type::STRING ? Type::STRING : Type::UNKNOWN;
        }

        case ExprKind::UNARY: {
            const auto& u = static_cast<const UnaryExpr&>(expr);
            Type t = valueType(*u.operand);
            if (u.op == TokenType::NOI) {
                if (t != Type::BOOL && t != Type::UNKNOWN) {
                    fail(u.line, "'noi' needs a sottoMitha value, but found " + typeName(t));
                }
                return Type::BOOL;
            }
            if (!numberOrUnknown(t)) {
                fail(u.line, "'" + u.opName + "' needs a number, but found " + typeName(t));
            }
            return u.op == TokenType::MULOK ? Type::FLOAT : t;
        }

        case ExprKind::BINARY:
            return binaryType(static_cast<const BinaryExpr&>(expr));

        case ExprKind::CALL:
            return callType(static_cast<const CallExpr&>(expr));
    }
    return Type::UNKNOWN;
}

Type SemanticAnalyzer::binaryType(const BinaryExpr& e) {
    Type l = valueType(*e.left);
    Type r = valueType(*e.right);
    std::string pair = typeName(l) + " and " + typeName(r);

    switch (e.op) {
        case TokenType::JOG:
            if (l == Type::STRING || r == Type::STRING) return Type::STRING;  // text joining
            if (numberOrUnknown(l) && numberOrUnknown(r)) return arithmeticType(l, r);
            fail(e.line, "'jog' needs two numbers (or at least one lekha), but found " + pair);

        case TokenType::BIYOG:
        case TokenType::GUNON:
        case TokenType::VAGFOL:
        case TokenType::VAGSHESH:
        case TokenType::GHAT:
            if (numberOrUnknown(l) && numberOrUnknown(r)) return arithmeticType(l, r);
            fail(e.line, "'" + e.opName + "' needs two numbers, but found " + pair);

        case TokenType::BORO:
        case TokenType::CHOTO:
            if (numberOrUnknown(l) && numberOrUnknown(r)) return Type::BOOL;
            if ((l == Type::STRING || l == Type::UNKNOWN) && (r == Type::STRING || r == Type::UNKNOWN)) {
                return Type::BOOL;
            }
            fail(e.line, "cannot compare " + pair + " with '" + e.opName + "'");

        case TokenType::SOMOMAN:
            if (l == Type::UNKNOWN || r == Type::UNKNOWN) return Type::BOOL;
            if (isNumber(l) && isNumber(r)) return Type::BOOL;
            if (l == r && (l == Type::STRING || l == Type::BOOL)) return Type::BOOL;
            fail(e.line, "cannot compare " + pair + " with 'somoman'");

        case TokenType::EBONG:
        case TokenType::OTHOBA:
            if ((l == Type::BOOL || l == Type::UNKNOWN) && (r == Type::BOOL || r == Type::UNKNOWN)) {
                return Type::BOOL;
            }
            fail(e.line, "'" + e.opName + "' needs two sottoMitha values, but found " + pair);

        default:
            fail(e.line, "internal error: unknown operator");
    }
}

Type SemanticAnalyzer::callType(const CallExpr& c) {
    // built-in functions
    if (c.name == "dorgho") {  // length of a talika or lekha
        if (c.args.size() != 1) fail(c.line, "function 'dorgho' expects 1 argument but got " + std::to_string(c.args.size()));
        Type t = valueType(*c.args[0]);
        if (t != Type::LIST && t != Type::STRING && t != Type::UNKNOWN) {
            fail(c.line, "'dorgho' needs a talika or a lekha, but found " + typeName(t));
        }
        return Type::INT;
    }
    if (c.name == "jukto") {  // add an element at the end of a talika
        if (c.args.size() != 2) fail(c.line, "function 'jukto' expects 2 arguments but got " + std::to_string(c.args.size()));
        Type t = valueType(*c.args[0]);
        if (t != Type::LIST && t != Type::UNKNOWN) {
            fail(c.line, "the first argument of 'jukto' must be a talika, but found " + typeName(t));
        }
        valueType(*c.args[1]);
        return Type::VOID;
    }

    auto it = functions.find(c.name);
    if (it == functions.end()) fail(c.line, "undefined function '" + c.name + "'");
    const FuncSig& sig = it->second;

    if (c.args.size() != sig.params.size()) {
        fail(c.line, "function '" + c.name + "' expects " + std::to_string(sig.params.size()) +
                     " argument(s) but got " + std::to_string(c.args.size()));
    }
    for (size_t k = 0; k < c.args.size(); k++) {
        Type t = valueType(*c.args[k]);
        if (!assignable(sig.params[k], t)) {
            fail(c.args[k]->line, "argument " + std::to_string(k + 1) + " of '" + c.name + "' must be " +
                                  typeName(sig.params[k]) + ", but found " + typeName(t));
        }
    }
    return sig.ret;
}
