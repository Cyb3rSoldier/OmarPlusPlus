#include "interpreter.h"

#include <climits>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>

#ifndef _WIN32
#include <sys/resource.h>
#endif

namespace {

struct ProgramExit {};  // thrown by 'shesh' inside a block or function

// How much C++ stack the running program may use before we stop it with a clean
// error (instead of crashing). Windows programs get 1-2 MB of stack by default.
// On Linux / macOS we use 75% of the real stack limit (usually 8 MB).
std::uintptr_t stackBudget() {
#ifdef _WIN32
    return 900 * 1024;
#else
    struct rlimit rl;
    if (getrlimit(RLIMIT_STACK, &rl) == 0 && rl.rlim_cur != RLIM_INFINITY) {
        return static_cast<std::uintptr_t>(rl.rlim_cur) / 4 * 3;
    }
    return 64u * 1024 * 1024;
#endif
}

OmarError runtimeError(int line, const std::string& message) {
    return OmarError("Runtime Error", line, message);
}

std::string formatFloat(double d) {
    if (d == 0) d = 0.0;  // avoid "-0"
    std::ostringstream out;
    out << std::setprecision(12) << d;
    std::string s = out.str();
    // keep dosomik values visibly different from purno: 3 -> 3.0
    if (s.find_first_of(".en") == std::string::npos) s += ".0";
    return s;
}

bool isNumber(const Value& v) { return v.type == Type::INT || v.type == Type::FLOAT; }
double toDouble(const Value& v) { return v.type == Type::INT ? static_cast<double>(v.i) : v.d; }

Value defaultValue(Type t) {
    switch (t) {
        case Type::INT:    return Value::makeInt(0);
        case Type::FLOAT:  return Value::makeFloat(0.0);
        case Type::STRING: return Value::makeString("");
        case Type::BOOL:   return Value::makeBool(false);
        case Type::LIST:   return Value::makeList(std::make_shared<std::vector<Value>>());
        default:           return Value();
    }
}

// Convert a value so it can be stored in something of type 'target'.
Value coerce(const Value& v, Type target, int line) {
    if (target == Type::UNKNOWN || v.type == target) return v;
    if (target == Type::FLOAT && v.type == Type::INT) return Value::makeFloat(static_cast<double>(v.i));
    throw runtimeError(line, "type mismatch: expected " + typeName(target) + " but got " + typeName(v.type));
}

size_t checkIndex(const Value& index, size_t size, int line) {
    if (index.type != Type::INT) {
        throw runtimeError(line, "an index must be purno, but got " + typeName(index.type));
    }
    if (index.i < 0 || static_cast<unsigned long long>(index.i) >= size) {
        throw runtimeError(line, "index " + std::to_string(index.i) + " is out of range (size " +
                                 std::to_string(size) + ")");
    }
    return static_cast<size_t>(index.i);
}

std::string trim(const std::string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    if (a == std::string::npos) return "";
    size_t b = s.find_last_not_of(" \t\r\n");
    return s.substr(a, b - a + 1);
}

// Turn the text typed by the user into a value of the variable's type.
Value parseInput(const std::string& raw, Type type, int line) {
    std::string text = trim(raw);
    try {
        size_t used = 0;
        switch (type) {
            case Type::INT: {
                long long v = std::stoll(text, &used);
                if (used != text.size()) throw std::invalid_argument("extra");
                return Value::makeInt(v);
            }
            case Type::FLOAT: {
                double v = std::stod(text, &used);
                if (used != text.size()) throw std::invalid_argument("extra");
                return Value::makeFloat(v);
            }
            case Type::BOOL:
                if (text == "sotto" || text == "true" || text == "1") return Value::makeBool(true);
                if (text == "mittha" || text == "false" || text == "0") return Value::makeBool(false);
                throw runtimeError(line, "invalid sottoMitha input '" + text + "' (type sotto or mittha)");
            default: {
                std::string s = raw;
                while (!s.empty() && (s.back() == '\r' || s.back() == '\n')) s.pop_back();
                return Value::makeString(s);
            }
        }
    } catch (const std::invalid_argument&) {
        throw runtimeError(line, "invalid " + typeName(type) + " input '" + text + "'");
    } catch (const std::out_of_range&) {
        throw runtimeError(line, "the number '" + text + "' is too large");
    }
}


Value intArithmetic(TokenType op, long long a, long long b, int line, const std::string& name) {
    long long result = 0;
    switch (op) {
        case TokenType::JOG:
            if (__builtin_add_overflow(a, b, &result)) throw runtimeError(line, "integer overflow in 'jog'");
            return Value::makeInt(result);
        case TokenType::BIYOG:
            if (__builtin_sub_overflow(a, b, &result)) throw runtimeError(line, "integer overflow in 'biyog'");
            return Value::makeInt(result);
        case TokenType::GUNON:
            if (__builtin_mul_overflow(a, b, &result)) throw runtimeError(line, "integer overflow in 'gunon'");
            return Value::makeInt(result);
        case TokenType::VAGFOL:
            if (b == 0) throw runtimeError(line, "division by zero");
            if (a == LLONG_MIN && b == -1) throw runtimeError(line, "integer overflow in 'vagfol'");
            return Value::makeInt(a / b);  // purno / purno stays purno (like C)
        case TokenType::VAGSHESH:
            if (b == 0) throw runtimeError(line, "modulus by zero");
            if (b == -1) return Value::makeInt(0);
            return Value::makeInt(a % b);
        case TokenType::GHAT: {
            if (b < 0) {
                throw runtimeError(line,
                    "'ghat' with two purno values needs a non-negative exponent (use a dosomik for fractions)");
            }
            long long base = a, exp = b;
            result = 1;
            while (exp > 0) {
                if ((exp & 1) && __builtin_mul_overflow(result, base, &result)) {
                    throw runtimeError(line, "integer overflow in 'ghat'");
                }
                exp >>= 1;
                if (exp > 0 && __builtin_mul_overflow(base, base, &base)) {
                    throw runtimeError(line, "integer overflow in 'ghat'");
                }
            }
            return Value::makeInt(result);
        }
        default:
            throw runtimeError(line, "internal error: unknown operator '" + name + "'");
    }
}

Value floatArithmetic(TokenType op, double a, double b, int line, const std::string& name) {
    double result = 0;
    switch (op) {
        case TokenType::JOG:   result = a + b; break;
        case TokenType::BIYOG: result = a - b; break;
        case TokenType::GUNON: result = a * b; break;
        case TokenType::VAGFOL:
            if (b == 0) throw runtimeError(line, "division by zero");
            result = a / b;
            break;
        case TokenType::VAGSHESH:
            if (b == 0) throw runtimeError(line, "modulus by zero");
            result = std::fmod(a, b);
            break;
        case TokenType::GHAT:  result = std::pow(a, b); break;
        default: throw runtimeError(line, "internal error: unknown operator '" + name + "'");
    }
    if (!std::isfinite(result)) {
        throw runtimeError(line, "the result of '" + name + "' is not a valid number");
    }
    return Value::makeFloat(result);
}

// somoman, boro, choto
Value compareValues(const BinaryExpr& e, const Value& l, const Value& r) {
    if (e.op == TokenType::SOMOMAN) {
        if (isNumber(l) && isNumber(r)) {
            if (l.type == Type::INT && r.type == Type::INT) return Value::makeBool(l.i == r.i);
            return Value::makeBool(toDouble(l) == toDouble(r));
        }
        if (l.type == Type::STRING && r.type == Type::STRING) return Value::makeBool(l.s == r.s);
        if (l.type == Type::BOOL && r.type == Type::BOOL) return Value::makeBool(l.b == r.b);
    } else {
        bool greater = (e.op == TokenType::BORO);
        if (isNumber(l) && isNumber(r)) {
            if (l.type == Type::INT && r.type == Type::INT) {
                return Value::makeBool(greater ? l.i > r.i : l.i < r.i);
            }
            double a = toDouble(l), b = toDouble(r);
            return Value::makeBool(greater ? a > b : a < b);
        }
        if (l.type == Type::STRING && r.type == Type::STRING) {
            return Value::makeBool(greater ? l.s > r.s : l.s < r.s);
        }
    }
    throw runtimeError(e.line, "cannot compare " + typeName(l.type) + " with " + typeName(r.type));
}

}  // namespace

// ---------------------------------------------------------------
// Value
// ---------------------------------------------------------------
Value Value::makeInt(long long v) { Value x; x.type = Type::INT; x.i = v; return x; }
Value Value::makeFloat(double v) { Value x; x.type = Type::FLOAT; x.d = v; return x; }
Value Value::makeString(std::string v) { Value x; x.type = Type::STRING; x.s = std::move(v); return x; }
Value Value::makeBool(bool v) { Value x; x.type = Type::BOOL; x.b = v; return x; }
Value Value::makeList(ListPtr v) { Value x; x.type = Type::LIST; x.list = std::move(v); return x; }

std::string Value::toString(bool quoteText, int depth) const {
    switch (type) {
        case Type::INT:    return std::to_string(i);
        case Type::FLOAT:  return formatFloat(d);
        case Type::BOOL:   return b ? "sotto" : "mittha";
        case Type::STRING: return quoteText ? "\"" + s + "\"" : s;
        case Type::LIST: {
            if (depth > 20) return "[...]";
            std::string out = "[";
            for (size_t k = 0; k < list->size(); k++) {
                if (k > 0) out += ", ";
                out += (*list)[k].toString(true, depth + 1);
            }
            return out + "]";
        }
        default: return "";
    }
}

// ---------------------------------------------------------------
// scopes and variables
// ---------------------------------------------------------------
Interpreter::Var* Interpreter::lookup(const std::string& name) {
    auto& scopes = frames.back().scopes;
    for (auto it = scopes.rbegin(); it != scopes.rend(); ++it) {
        auto found = it->find(name);
        if (found != it->end()) return &found->second;
    }
    // inside a function: global variables are visible too
    if (frames.size() > 1) {
        auto& globals = frames.front().scopes.front();
        auto found = globals.find(name);
        if (found != globals.end()) return &found->second;
    }
    return nullptr;
}

void Interpreter::declare(const std::string& name, Type type, Value value) {
    frames.back().scopes.back()[name] = Var{std::move(value), type};
}

// Finds the memory location of x or x[i][j] so it can be changed.
Interpreter::LRef Interpreter::resolve(const Expr& target) {
    if (target.kind == ExprKind::VARIABLE) {
        const auto& v = static_cast<const VarExpr&>(target);
        Var* var = lookup(v.name);
        if (!var) throw runtimeError(v.line, "undeclared variable '" + v.name + "'");
        return {&var->value, var->type};
    }
    if (target.kind == ExprKind::INDEX) {
        const auto& ix = static_cast<const IndexExpr&>(target);
        Value index = evaluate(*ix.index);   // evaluate first, take pointers afterwards
        LRef base = resolve(*ix.base);
        if (base.ptr->type != Type::LIST) {
            throw runtimeError(ix.line, "only a talika can be changed by index");
        }
        size_t k = checkIndex(index, base.ptr->list->size(), ix.line);
        return {&(*base.ptr->list)[k], Type::UNKNOWN};
    }
    throw runtimeError(target.line, "cannot change this value");
}

// ---------------------------------------------------------------
// running
// ---------------------------------------------------------------
void Interpreter::run(const Program& program) {
    frames.clear();
    frames.emplace_back();
    frames.back().scopes.emplace_back();
    functions.clear();
    currentFunction = nullptr;
    char marker = 0;
    stackBase = reinterpret_cast<std::uintptr_t>(&marker);

    for (const StmtPtr& s : program.statements) {
        if (s->kind == StmtKind::FUNC_DECL) {
            const auto* f = static_cast<const FuncDeclStmt*>(s.get());
            functions[f->name] = f;
        }
    }

    try {
        executeBlock(program.statements);
    } catch (const ProgramExit&) {
        // 'shesh' was reached: stop quietly
    }
    std::cout.flush();
}

Interpreter::Signal Interpreter::executeBlock(const Block& block) {
    for (const StmtPtr& s : block) {
        Signal sig = execute(*s);
        if (sig != Signal::NONE) return sig;
    }
    return Signal::NONE;
}

Interpreter::Signal Interpreter::executeScoped(const Block& block) {
    frames.back().scopes.emplace_back();
    Signal sig = executeBlock(block);
    frames.back().scopes.pop_back();
    return sig;
}

bool Interpreter::condition(const Expr& expr) {
    Value v = evaluate(expr);
    if (v.type != Type::BOOL) {
        throw runtimeError(expr.line, "the condition must be sottoMitha, but got " + typeName(v.type));
    }
    return v.b;
}

Interpreter::Signal Interpreter::execute(const Stmt& stmt) {
    switch (stmt.kind) {
        case StmtKind::VAR_DECL: {
            const auto& d = static_cast<const VarDeclStmt&>(stmt);
            Value v = d.init ? coerce(evaluate(*d.init), d.type, d.line) : defaultValue(d.type);
            declare(d.name, d.type, std::move(v));
            return Signal::NONE;
        }

        case StmtKind::ASSIGN: {
            const auto& a = static_cast<const AssignStmt&>(stmt);
            Value v = evaluate(*a.value);
            LRef ref = resolve(*a.target);
            *ref.ptr = coerce(v, ref.type, a.line);
            return Signal::NONE;
        }

        case StmtKind::PRINT:
            execPrint(static_cast<const PrintStmt&>(stmt));
            return Signal::NONE;

        case StmtKind::INPUT:
            execInput(static_cast<const InputStmt&>(stmt));
            return Signal::NONE;

        case StmtKind::IF: {
            const auto& s = static_cast<const IfStmt&>(stmt);
            for (size_t k = 0; k < s.conditions.size(); k++) {
                if (condition(*s.conditions[k])) return executeScoped(s.blocks[k]);
            }
            if (s.hasElse) return executeScoped(s.elseBlock);
            return Signal::NONE;
        }

        case StmtKind::FOR:
            return execFor(static_cast<const ForStmt&>(stmt));

        case StmtKind::WHILE: {
            const auto& w = static_cast<const WhileStmt&>(stmt);
            while (condition(*w.condition)) {
                Signal sig = executeScoped(w.body);
                if (sig == Signal::BREAK) break;
                if (sig == Signal::RETURN) return sig;
            }
            return Signal::NONE;
        }

        case StmtKind::BREAK:    return Signal::BREAK;
        case StmtKind::CONTINUE: return Signal::CONTINUE;

        case StmtKind::RETURN: {
            const auto& r = static_cast<const ReturnStmt&>(stmt);
            if (r.value) {
                Value v = evaluate(*r.value);
                returnValue = coerce(v, currentFunction->returnType, r.line);
            } else {
                returnValue = Value();
            }
            return Signal::RETURN;
        }

        case StmtKind::FUNC_DECL:
            return Signal::NONE;  // already registered before the program started

        case StmtKind::EXPR:
            evaluate(*static_cast<const ExprStmt&>(stmt).expr);
            return Signal::NONE;

        case StmtKind::INCDEC:
            execIncDec(static_cast<const IncDecStmt&>(stmt));
            return Signal::NONE;

        case StmtKind::BLOCK:
            return executeScoped(static_cast<const BlockStmt&>(stmt).body);

        case StmtKind::EXIT:
            throw ProgramExit{};
    }
    return Signal::NONE;
}

void Interpreter::execPrint(const PrintStmt& p) {
    std::string line;
    for (size_t k = 0; k < p.values.size(); k++) {
        if (k > 0) line += " ";
        line += evaluate(*p.values[k]).toString();
    }
    std::cout << line << "\n";
}

void Interpreter::execInput(const InputStmt& in) {
    Type type = lookup(in.name)->type;
    if (in.hasPrompt) std::cout << in.prompt;
    std::cout.flush();

    std::string text;
    if (!std::getline(std::cin, text)) {
        throw runtimeError(in.line, "no input available for '" + in.name + "'");
    }
    Value v = parseInput(text, type, in.line);
    lookup(in.name)->value = v;
}

Interpreter::Signal Interpreter::execFor(const ForStmt& f) {
    Value from = evaluate(*f.from);
    Value to = evaluate(*f.to);
    if (from.type != Type::INT || to.type != Type::INT) {
        throw runtimeError(f.line, "the range of 'ghuro' must use purno values");
    }

    long long step = (from.i <= to.i) ? 1 : -1;  // counts down when start > end
    frames.back().scopes.emplace_back();
    declare(f.var, Type::INT, from);

    Signal result = Signal::NONE;
    for (long long k = from.i;; k += step) {
        lookup(f.var)->value = Value::makeInt(k);
        Signal sig = executeScoped(f.body);
        if (sig == Signal::BREAK) break;
        if (sig == Signal::RETURN) { result = sig; break; }
        if (k == to.i) break;  // the end value is included
    }
    frames.back().scopes.pop_back();
    return result;
}

void Interpreter::execIncDec(const IncDecStmt& s) {
    LRef ref = resolve(*s.target);
    Value& v = *ref.ptr;
    if (v.type == Type::INT) {
        long long result;
        bool overflow = s.increment ? __builtin_add_overflow(v.i, 1LL, &result)
                                    : __builtin_sub_overflow(v.i, 1LL, &result);
        if (overflow) throw runtimeError(s.line, "integer overflow");
        v.i = result;
    } else if (v.type == Type::FLOAT) {
        v.d += s.increment ? 1.0 : -1.0;
    } else {
        throw runtimeError(s.line, "'barao' and 'komao' need a number");
    }
}

// ---------------------------------------------------------------
// expressions
// ---------------------------------------------------------------
Value Interpreter::evaluate(const Expr& expr) {
    switch (expr.kind) {
        case ExprKind::INT_LIT:    return Value::makeInt(static_cast<const IntExpr&>(expr).value);
        case ExprKind::FLOAT_LIT:  return Value::makeFloat(static_cast<const FloatExpr&>(expr).value);
        case ExprKind::STRING_LIT: return Value::makeString(static_cast<const StringExpr&>(expr).value);
        case ExprKind::BOOL_LIT:   return Value::makeBool(static_cast<const BoolExpr&>(expr).value);

        case ExprKind::LIST_LIT: return evalList(static_cast<const ListExpr&>(expr));

        case ExprKind::VARIABLE: {
            const auto& v = static_cast<const VarExpr&>(expr);
            Var* var = lookup(v.name);
            if (!var) throw runtimeError(v.line, "undeclared variable '" + v.name + "'");
            return var->value;
        }

        case ExprKind::INDEX:  return evalIndex(static_cast<const IndexExpr&>(expr));
        case ExprKind::UNARY:  return evalUnary(static_cast<const UnaryExpr&>(expr));
        case ExprKind::BINARY: return evalBinary(static_cast<const BinaryExpr&>(expr));
        case ExprKind::CALL:   return evalCall(static_cast<const CallExpr&>(expr));
    }
    throw runtimeError(expr.line, "internal error: unknown expression");
}

Value Interpreter::evalUnary(const UnaryExpr& e) {
    Value v = evaluate(*e.operand);

    if (e.op == TokenType::NOI) {
        if (v.type != Type::BOOL) throw runtimeError(e.line, "'noi' needs a sottoMitha value");
        return Value::makeBool(!v.b);
    }

    if (!isNumber(v)) {
        throw runtimeError(e.line, "'" + e.opName + "' needs a number, but got " + typeName(v.type));
    }

    if (e.op == TokenType::MINUS) {
        if (v.type == Type::INT) {
            if (v.i == LLONG_MIN) throw runtimeError(e.line, "integer overflow");
            return Value::makeInt(-v.i);
        }
        return Value::makeFloat(-v.d);
    }

    // MULOK (square root) always gives a dosomik
    double x = toDouble(v);
    if (x < 0) throw runtimeError(e.line, "cannot take 'mulok' of a negative number");
    return Value::makeFloat(std::sqrt(x));
}

Value Interpreter::evalBinary(const BinaryExpr& e) {
    // ebong / othoba stop early: the right side runs only if it is needed
    if (e.op == TokenType::EBONG || e.op == TokenType::OTHOBA) {
        Value l = evaluate(*e.left);
        if (l.type != Type::BOOL) throw runtimeError(e.line, "'" + e.opName + "' needs sottoMitha values");
        if (e.op == TokenType::EBONG && !l.b) return Value::makeBool(false);
        if (e.op == TokenType::OTHOBA && l.b) return Value::makeBool(true);
        Value r = evaluate(*e.right);
        if (r.type != Type::BOOL) throw runtimeError(e.line, "'" + e.opName + "' needs sottoMitha values");
        return Value::makeBool(r.b);
    }

    Value l = evaluate(*e.left);
    Value r = evaluate(*e.right);

    // 'jog' joins text when either side is a lekha
    if (e.op == TokenType::JOG && (l.type == Type::STRING || r.type == Type::STRING)) {
        return Value::makeString(l.toString() + r.toString());
    }

    if (e.op == TokenType::SOMOMAN || e.op == TokenType::BORO || e.op == TokenType::CHOTO) {
        return compareValues(e, l, r);
    }

    // ---- arithmetic ----
    if (!isNumber(l) || !isNumber(r)) {
        throw runtimeError(e.line, "'" + e.opName + "' needs two numbers, but got " +
                                   typeName(l.type) + " and " + typeName(r.type));
    }
    if (l.type == Type::INT && r.type == Type::INT) {
        return intArithmetic(e.op, l.i, r.i, e.line, e.opName);
    }
    return floatArithmetic(e.op, toDouble(l), toDouble(r), e.line, e.opName);
}

Value Interpreter::evalList(const ListExpr& l) {
    auto items = std::make_shared<std::vector<Value>>();
    for (const ExprPtr& e : l.elements) items->push_back(evaluate(*e));
    return Value::makeList(items);
}

Value Interpreter::evalIndex(const IndexExpr& ix) {
    Value base = evaluate(*ix.base);
    Value index = evaluate(*ix.index);
    if (base.type == Type::LIST) {
        return (*base.list)[checkIndex(index, base.list->size(), ix.line)];
    }
    if (base.type == Type::STRING) {
        return Value::makeString(std::string(1, base.s[checkIndex(index, base.s.size(), ix.line)]));
    }
    throw runtimeError(ix.line, "only a talika or a lekha can be indexed");
}

Value Interpreter::evalBuiltin(const CallExpr& c) {
    if (c.name == "dorgho") {
        Value v = evaluate(*c.args[0]);
        if (v.type == Type::LIST)   return Value::makeInt(static_cast<long long>(v.list->size()));
        if (v.type == Type::STRING) return Value::makeInt(static_cast<long long>(v.s.size()));
        throw runtimeError(c.line, "'dorgho' needs a talika or a lekha, but got " + typeName(v.type));
    }
    // jukto(list, item)
    Value list = evaluate(*c.args[0]);
    Value item = evaluate(*c.args[1]);
    if (list.type != Type::LIST) {
        throw runtimeError(c.line, "the first argument of 'jukto' must be a talika");
    }
    list.list->push_back(item);
    return Value();
}

Value Interpreter::evalCall(const CallExpr& c) {
    if (c.name == "dorgho" || c.name == "jukto") return evalBuiltin(c);

    // ---- user functions ----
    const FuncDeclStmt* fn = functions.at(c.name);

    std::vector<Value> args;
    for (size_t k = 0; k < c.args.size(); k++) {
        args.push_back(coerce(evaluate(*c.args[k]), fn->params[k].type, c.line));
    }

    char marker = 0;
    std::uintptr_t here = reinterpret_cast<std::uintptr_t>(&marker);
    std::uintptr_t used = here > stackBase ? here - stackBase : stackBase - here;
    if (used > stackBudget()) {
        throw runtimeError(c.line, "too many nested function calls (infinite recursion?)");
    }

    frames.emplace_back();
    frames.back().scopes.emplace_back();
    for (size_t k = 0; k < args.size(); k++) {
        declare(fn->params[k].name, fn->params[k].type, args[k]);
    }

    const FuncDeclStmt* savedFunction = currentFunction;
    currentFunction = fn;
    returnValue = Value();

    Signal sig = executeBlock(fn->body);

    currentFunction = savedFunction;
    frames.pop_back();

    if (sig == Signal::RETURN) {
        Value result = returnValue;
        returnValue = Value();
        return result;
    }
    if (fn->returnType != Type::VOID) {
        throw runtimeError(c.line, "function '" + fn->name + "' ended without returning a value");
    }
    return Value();
}
