// Omar++ : the whole language in ONE file.
// Build:  g++ -std=c++17 -O1 -o omar omarpp_all.cpp
// (This file is generated from the separate .h/.cpp files in this folder.)

// ===================== token.h =====================
#ifndef OMAR_TOKEN_H
#define OMAR_TOKEN_H

#include <stdexcept>
#include <string>

enum class TokenType {
    // ---- keywords from the proposal ----
    DEKHAO,        // print
    NAO,           // input
    DHORO,         // declare / assign
    DRUBOK,        // constant
    JOG,           // +
    BIYOG,         // -
    GUNON,         // *
    VAGFOL,        // /
    VAGSHESH,      // %
    GHAT,          // power
    MULOK,         // square root
    JODI,          // if
    NAHOLE,        // else
    ONNOTHAYJODI,  // else if
    GHURO,         // for
    CHOLTETHAKO,   // while
    KAJ,           // function
    FEROT,         // return
    THAMO,         // break
    CHALIYEJAO,    // continue
    TALIKA,        // list / array
    LEKHA,         // string
    PURNO,         // integer
    DOSOMIK,       // float
    SOTTOMITHA,    // boolean
    EBONG,         // and
    OTHOBA,        // or
    NOI,           // not
    SOMOMAN,       // ==
    BORO,          // >
    CHOTO,         // <
    BARAO,         // ++
    KOMAO,         // --
    SHESH,         // end program

    // ---- small extras needed to make the language usable ----
    THEKE,         // "from ... theke ... " range in ghuro
    SOTTO,         // true literal
    MITTHA,        // false literal

    // ---- literals ----
    INT_LIT,
    FLOAT_LIT,
    STRING_LIT,
    IDENTIFIER,

    // ---- symbols ----
    MINUS,         // unary minus only
    LPAREN, RPAREN,
    LBRACE, RBRACE,
    LBRACKET, RBRACKET,
    SEMICOLON,
    COMMA,

    END_OF_FILE
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
};

// One error type for every compiler phase.
// phase: "Lexical Error", "Syntax Error", "Semantic Error" or "Runtime Error".
class OmarError : public std::runtime_error {
public:
    OmarError(std::string phase, int line, const std::string& message)
        : std::runtime_error(message), phase_(std::move(phase)), line_(line) {}
    int line() const { return line_; }
    const std::string& phase() const { return phase_; }

private:
    std::string phase_;
    int line_;
};

inline std::string tokenTypeName(TokenType type) {
    switch (type) {
        case TokenType::DEKHAO:       return "DEKHAO";
        case TokenType::NAO:          return "NAO";
        case TokenType::DHORO:        return "DHORO";
        case TokenType::DRUBOK:       return "DRUBOK";
        case TokenType::JOG:          return "JOG";
        case TokenType::BIYOG:        return "BIYOG";
        case TokenType::GUNON:        return "GUNON";
        case TokenType::VAGFOL:       return "VAGFOL";
        case TokenType::VAGSHESH:     return "VAGSHESH";
        case TokenType::GHAT:         return "GHAT";
        case TokenType::MULOK:        return "MULOK";
        case TokenType::JODI:         return "JODI";
        case TokenType::NAHOLE:       return "NAHOLE";
        case TokenType::ONNOTHAYJODI: return "ONNOTHAYJODI";
        case TokenType::GHURO:        return "GHURO";
        case TokenType::CHOLTETHAKO:  return "CHOLTETHAKO";
        case TokenType::KAJ:          return "KAJ";
        case TokenType::FEROT:        return "FEROT";
        case TokenType::THAMO:        return "THAMO";
        case TokenType::CHALIYEJAO:   return "CHALIYEJAO";
        case TokenType::TALIKA:       return "TALIKA";
        case TokenType::LEKHA:        return "LEKHA";
        case TokenType::PURNO:        return "PURNO";
        case TokenType::DOSOMIK:      return "DOSOMIK";
        case TokenType::SOTTOMITHA:   return "SOTTOMITHA";
        case TokenType::EBONG:        return "EBONG";
        case TokenType::OTHOBA:       return "OTHOBA";
        case TokenType::NOI:          return "NOI";
        case TokenType::SOMOMAN:      return "SOMOMAN";
        case TokenType::BORO:         return "BORO";
        case TokenType::CHOTO:        return "CHOTO";
        case TokenType::BARAO:        return "BARAO";
        case TokenType::KOMAO:        return "KOMAO";
        case TokenType::SHESH:        return "SHESH";
        case TokenType::THEKE:        return "THEKE";
        case TokenType::SOTTO:        return "SOTTO";
        case TokenType::MITTHA:       return "MITTHA";
        case TokenType::INT_LIT:      return "INT_LIT";
        case TokenType::FLOAT_LIT:    return "FLOAT_LIT";
        case TokenType::STRING_LIT:   return "STRING_LIT";
        case TokenType::IDENTIFIER:   return "IDENTIFIER";
        case TokenType::MINUS:        return "MINUS";
        case TokenType::LPAREN:       return "LPAREN";
        case TokenType::RPAREN:       return "RPAREN";
        case TokenType::LBRACE:       return "LBRACE";
        case TokenType::RBRACE:       return "RBRACE";
        case TokenType::LBRACKET:     return "LBRACKET";
        case TokenType::RBRACKET:     return "RBRACKET";
        case TokenType::SEMICOLON:    return "SEMICOLON";
        case TokenType::COMMA:        return "COMMA";
        case TokenType::END_OF_FILE:  return "END_OF_FILE";
    }
    return "UNKNOWN";
}

#endif

// ===================== ast.h =====================
#ifndef OMAR_AST_H
#define OMAR_AST_H

#include <memory>
#include <string>
#include <vector>


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

// ===================== lexer.h =====================
#ifndef OMAR_LEXER_H
#define OMAR_LEXER_H

#include <string>
#include <vector>


// Phase 1: converts Omar++ source text into a list of tokens.
class Lexer {
public:
    explicit Lexer(std::string source);
    std::vector<Token> tokenize();

private:
    std::string src;
    size_t pos = 0;
    int line = 1;

    bool atEnd() const;
    char peek(size_t offset = 0) const;
    char advance();
    OmarError error(int atLine, const std::string& message) const;

    Token number();
    Token text();
    Token word();
    void skipComment();
};

#endif

// ===================== lexer.cpp =====================

#include <cctype>
#include <unordered_map>

// Keyword table: a word found here is a keyword, otherwise it is an IDENTIFIER.
// Keywords are case-sensitive (cholteThako and sottoMitha have capital letters).
static const std::unordered_map<std::string, TokenType> keywords = {
    {"dekhao",       TokenType::DEKHAO},
    {"nao",          TokenType::NAO},
    {"dhoro",        TokenType::DHORO},
    {"drubok",       TokenType::DRUBOK},
    {"jog",          TokenType::JOG},
    {"biyog",        TokenType::BIYOG},
    {"gunon",        TokenType::GUNON},
    {"vagfol",       TokenType::VAGFOL},
    {"vagshesh",     TokenType::VAGSHESH},
    {"ghat",         TokenType::GHAT},
    {"mulok",        TokenType::MULOK},
    {"jodi",         TokenType::JODI},
    {"nahole",       TokenType::NAHOLE},
    {"onnothayjodi", TokenType::ONNOTHAYJODI},
    {"ghuro",        TokenType::GHURO},
    {"cholteThako",  TokenType::CHOLTETHAKO},
    {"kaj",          TokenType::KAJ},
    {"ferot",        TokenType::FEROT},
    {"thamo",        TokenType::THAMO},
    {"chaliyejao",   TokenType::CHALIYEJAO},
    {"talika",       TokenType::TALIKA},
    {"lekha",        TokenType::LEKHA},
    {"purno",        TokenType::PURNO},
    {"dosomik",      TokenType::DOSOMIK},
    {"sottoMitha",   TokenType::SOTTOMITHA},
    {"ebong",        TokenType::EBONG},
    {"othoba",       TokenType::OTHOBA},
    {"noi",          TokenType::NOI},
    {"somoman",      TokenType::SOMOMAN},
    {"boro",         TokenType::BORO},
    {"choto",        TokenType::CHOTO},
    {"barao",        TokenType::BARAO},
    {"komao",        TokenType::KOMAO},
    {"shesh",        TokenType::SHESH},
    // extras
    {"theke",        TokenType::THEKE},
    {"sotto",        TokenType::SOTTO},
    {"mittha",       TokenType::MITTHA},
};

Lexer::Lexer(std::string source) : src(std::move(source)) {}

bool Lexer::atEnd() const { return pos >= src.size(); }

char Lexer::peek(size_t offset) const {
    return (pos + offset < src.size()) ? src[pos + offset] : '\0';
}

char Lexer::advance() { return src[pos++]; }

OmarError Lexer::error(int atLine, const std::string& message) const {
    return OmarError("Lexical Error", atLine, message);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;

    while (!atEnd()) {
        char c = peek();

        if (c == '\n') { line++; advance(); continue; }
        if (c == ' ' || c == '\t' || c == '\r') { advance(); continue; }

        if (std::isdigit(static_cast<unsigned char>(c))) {
            tokens.push_back(number());
            continue;
        }

        if (c == '"') {
            tokens.push_back(text());
            continue;
        }

        if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
            Token t = word();
            if (t.lexeme == "tika") {   // 'tika' starts a comment that runs to the end of the line
                skipComment();
                continue;
            }
            tokens.push_back(t);
            continue;
        }

        switch (c) {
            case '(': tokens.push_back({TokenType::LPAREN,    "(", line}); advance(); break;
            case ')': tokens.push_back({TokenType::RPAREN,    ")", line}); advance(); break;
            case '{': tokens.push_back({TokenType::LBRACE,    "{", line}); advance(); break;
            case '}': tokens.push_back({TokenType::RBRACE,    "}", line}); advance(); break;
            case '[': tokens.push_back({TokenType::LBRACKET,  "[", line}); advance(); break;
            case ']': tokens.push_back({TokenType::RBRACKET,  "]", line}); advance(); break;
            case ';': tokens.push_back({TokenType::SEMICOLON, ";", line}); advance(); break;
            case ',': tokens.push_back({TokenType::COMMA,     ",", line}); advance(); break;
            case '-': tokens.push_back({TokenType::MINUS,     "-", line}); advance(); break;
            default:
                throw error(line, std::string("unexpected character '") + c + "'");
        }
    }

    tokens.push_back({TokenType::END_OF_FILE, "", line});
    return tokens;
}

Token Lexer::number() {
    size_t start = pos;
    while (std::isdigit(static_cast<unsigned char>(peek()))) advance();

    bool isFloat = false;
    if (peek() == '.' && std::isdigit(static_cast<unsigned char>(peek(1)))) {
        isFloat = true;
        advance();
        while (std::isdigit(static_cast<unsigned char>(peek()))) advance();
    }
    return {isFloat ? TokenType::FLOAT_LIT : TokenType::INT_LIT,
            src.substr(start, pos - start), line};
}

Token Lexer::text() {
    int startLine = line;
    advance();  // opening quote
    std::string value;

    while (!atEnd() && peek() != '"') {
        char c = advance();
        if (c == '\n') {
            throw error(startLine, "unterminated text (missing closing '\"')");
        }
        if (c == '\\') {
            if (atEnd()) break;
            char e = advance();
            switch (e) {
                case 'n':  value += '\n'; break;
                case 't':  value += '\t'; break;
                case '"':  value += '"';  break;
                case '\\': value += '\\'; break;
                default:
                    throw error(line, std::string("unknown escape sequence '\\") + e + "'");
            }
        } else {
            value += c;
        }
    }

    if (atEnd()) {
        throw error(startLine, "unterminated text (missing closing '\"')");
    }
    advance();  // closing quote
    return {TokenType::STRING_LIT, value, startLine};
}

Token Lexer::word() {
    size_t start = pos;
    while (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_') advance();
    std::string lexeme = src.substr(start, pos - start);

    auto it = keywords.find(lexeme);
    TokenType type = (it != keywords.end()) ? it->second : TokenType::IDENTIFIER;
    return {type, lexeme, line};
}

void Lexer::skipComment() {
    while (!atEnd() && peek() != '\n') advance();
}

// ===================== parser.h =====================
#ifndef OMAR_PARSER_H
#define OMAR_PARSER_H

#include <string>
#include <vector>


// Phase 2: recursive descent parser. Builds the AST from the token list.
//
// program      -> statement* "shesh"
// statement    -> varDecl | funcDecl | ifStmt | forStmt | whileStmt | printStmt
//               | inputStmt | returnStmt | breakStmt | continueStmt | incDecStmt
//               | exitStmt | block | assignOrCall
// varDecl      -> ["drubok"] type IDENT ["dhoro" expression] ";"
// type         -> "purno" | "dosomik" | "lekha" | "sottoMitha" | "talika"
// funcDecl     -> "kaj" [type] IDENT "(" [param ("," param)*] ")" block
// param        -> type IDENT
// ifStmt       -> "jodi" expression block ("onnothayjodi" expression block)* ["nahole" block]
// forStmt      -> "ghuro" IDENT "dhoro" expression "theke" expression block
// whileStmt    -> "cholteThako" expression block
// printStmt    -> "dekhao" expression ("," expression)* ";"
// inputStmt    -> "nao" [STRING ","] IDENT ";"
// returnStmt   -> "ferot" [expression] ";"
// breakStmt    -> "thamo" ";"
// continueStmt -> "chaliyejao" ";"
// incDecStmt   -> ("barao" | "komao") postfix ";"
// exitStmt     -> "shesh" [";"]            (only inside a block)
// block        -> "{" statement* "}"
// assignOrCall -> expression ["dhoro" expression] ";"
//
// expression   -> logic_or
// logic_or     -> logic_and ("othoba" logic_and)*
// logic_and    -> logic_not ("ebong" logic_not)*
// logic_not    -> "noi" logic_not | equality
// equality     -> comparison ("somoman" comparison)*
// comparison   -> term (("boro" | "choto") term)*
// term         -> factor (("jog" | "biyog") factor)*
// factor       -> unary (("gunon" | "vagfol" | "vagshesh") unary)*
// unary        -> "-" unary | power
// power        -> postfix ("ghat" unary)?
// postfix      -> primary ("[" expression "]")*
// primary      -> INT | FLOAT | STRING | "sotto" | "mittha"
//               | IDENT | IDENT "(" [expression ("," expression)*] ")"
//               | "(" expression ")" | "mulok" "(" expression ")"
//               | "[" [expression ("," expression)*] "]"
class Parser {
public:
    explicit Parser(std::vector<Token> tokens);
    Program parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;
    int depth = 0;  // how many blocks deep we are (0 = top level)

    const Token& peek() const;
    const Token& previous() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    const Token& advance();
    const Token& consume(TokenType type, const std::string& message);
    void consumeSemicolon(const std::string& after);
    std::string describe(const Token& token) const;
    OmarError error(int line, const std::string& message) const;

    Block block();
    StmtPtr statement();
    StmtPtr varDecl();
    StmtPtr funcDecl();
    StmtPtr ifStatement();
    StmtPtr forStatement();
    StmtPtr whileStatement();
    StmtPtr printStatement();
    StmtPtr inputStatement();
    StmtPtr returnStatement();
    StmtPtr incDecStatement();
    StmtPtr simpleStatement();

    ExprPtr expression();
    ExprPtr logicOr();
    ExprPtr logicAnd();
    ExprPtr logicNot();
    ExprPtr equality();
    ExprPtr comparison();
    ExprPtr term();
    ExprPtr factor();
    ExprPtr unary();
    ExprPtr power();
    ExprPtr postfix();
    ExprPtr primary();
    std::vector<ExprPtr> arguments();
};

#endif

// ===================== parser.cpp =====================

namespace {

bool isTypeToken(TokenType t) {
    return t == TokenType::PURNO || t == TokenType::DOSOMIK || t == TokenType::LEKHA ||
           t == TokenType::SOTTOMITHA || t == TokenType::TALIKA;
}

Type toType(TokenType t) {
    switch (t) {
        case TokenType::PURNO:      return Type::INT;
        case TokenType::DOSOMIK:    return Type::FLOAT;
        case TokenType::LEKHA:      return Type::STRING;
        case TokenType::SOTTOMITHA: return Type::BOOL;
        case TokenType::TALIKA:     return Type::LIST;
        default:                    return Type::UNKNOWN;
    }
}

}  // namespace

Parser::Parser(std::vector<Token> toks) : tokens(std::move(toks)) {}

// ---------------------------------------------------------------
// helpers
// ---------------------------------------------------------------
const Token& Parser::peek() const { return tokens[current]; }
const Token& Parser::previous() const { return tokens[current - 1]; }
bool Parser::check(TokenType type) const { return peek().type == type; }

const Token& Parser::advance() {
    if (check(TokenType::END_OF_FILE)) return peek();
    return tokens[current++];
}

bool Parser::match(TokenType type) {
    if (check(type)) { advance(); return true; }
    return false;
}

OmarError Parser::error(int line, const std::string& message) const {
    return OmarError("Syntax Error", line, message);
}

std::string Parser::describe(const Token& token) const {
    switch (token.type) {
        case TokenType::END_OF_FILE: return "end of file";
        case TokenType::STRING_LIT:  return "a text value";
        default:                     return "'" + token.lexeme + "'";
    }
}

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek().line, message + ", but found " + describe(peek()));
}

// A missing ';' is reported on the line where the statement ended.
void Parser::consumeSemicolon(const std::string& after) {
    if (!check(TokenType::SEMICOLON)) {
        throw error(previous().line,
                    "expected ';' at the end of " + after + ", but found " + describe(peek()));
    }
    advance();
}

// ---------------------------------------------------------------
// program and statements
// ---------------------------------------------------------------
Program Parser::parse() {
    Program program;

    while (!check(TokenType::SHESH) && !check(TokenType::END_OF_FILE)) {
        program.statements.push_back(statement());
    }

    if (!check(TokenType::SHESH)) {
        throw error(peek().line, "missing 'shesh' at the end of the program");
    }
    advance();  // consume 'shesh'

    if (!check(TokenType::END_OF_FILE)) {
        throw error(peek().line, "unexpected code after 'shesh'");
    }
    return program;
}

Block Parser::block() {
    consume(TokenType::LBRACE, "expected '{' to start the block");
    depth++;
    Block statements;
    while (!check(TokenType::RBRACE) && !check(TokenType::END_OF_FILE)) {
        statements.push_back(statement());
    }
    consume(TokenType::RBRACE, "expected '}' to close the block");
    depth--;
    return statements;
}

StmtPtr Parser::statement() {
    switch (peek().type) {
        case TokenType::DRUBOK:
        case TokenType::PURNO:
        case TokenType::DOSOMIK:
        case TokenType::LEKHA:
        case TokenType::SOTTOMITHA:
        case TokenType::TALIKA:      return varDecl();
        case TokenType::KAJ:         return funcDecl();
        case TokenType::JODI:        return ifStatement();
        case TokenType::GHURO:       return forStatement();
        case TokenType::CHOLTETHAKO: return whileStatement();
        case TokenType::DEKHAO:      return printStatement();
        case TokenType::NAO:         return inputStatement();
        case TokenType::FEROT:       return returnStatement();
        case TokenType::BARAO:
        case TokenType::KOMAO:       return incDecStatement();
        case TokenType::THAMO: {
            int line = advance().line;
            consumeSemicolon("the 'thamo' statement");
            return std::make_unique<BreakStmt>(line);
        }
        case TokenType::CHALIYEJAO: {
            int line = advance().line;
            consumeSemicolon("the 'chaliyejao' statement");
            return std::make_unique<ContinueStmt>(line);
        }
        case TokenType::SHESH: {  // early exit (only reachable inside a block)
            int line = advance().line;
            match(TokenType::SEMICOLON);
            return std::make_unique<ExitStmt>(line);
        }
        case TokenType::LBRACE: {
            auto stmt = std::make_unique<BlockStmt>(peek().line);
            stmt->body = block();
            return stmt;
        }
        default:
            return simpleStatement();
    }
}

// [drubok] type name [dhoro expression] ;
StmtPtr Parser::varDecl() {
    int line = peek().line;
    bool isConst = match(TokenType::DRUBOK);

    if (!isTypeToken(peek().type)) {
        throw error(peek().line,
                    "expected a type (purno, dosomik, lekha, sottoMitha or talika) after 'drubok', but found " +
                    describe(peek()));
    }
    Type type = toType(advance().type);
    Token name = consume(TokenType::IDENTIFIER, "expected a variable name");

    auto decl = std::make_unique<VarDeclStmt>(line);
    decl->type = type;
    decl->name = name.lexeme;
    decl->isConst = isConst;

    if (match(TokenType::DHORO)) {
        decl->init = expression();
    } else if (isConst) {
        throw error(name.line, "constant '" + name.lexeme + "' must be given a value with 'dhoro'");
    }
    consumeSemicolon("the variable declaration");
    return decl;
}

// kaj [type] name ( type a, type b ) { ... }
StmtPtr Parser::funcDecl() {
    int line = advance().line;  // 'kaj'
    auto fn = std::make_unique<FuncDeclStmt>(line);

    if (isTypeToken(peek().type)) fn->returnType = toType(advance().type);
    fn->name = consume(TokenType::IDENTIFIER, "expected a function name after 'kaj'").lexeme;
    consume(TokenType::LPAREN, "expected '(' after the function name");

    if (!check(TokenType::RPAREN)) {
        do {
            if (!isTypeToken(peek().type)) {
                throw error(peek().line, "expected a parameter type, but found " + describe(peek()));
            }
            Type pt = toType(advance().type);
            Token pn = consume(TokenType::IDENTIFIER, "expected a parameter name");
            fn->params.push_back({pt, pn.lexeme, pn.line});
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RPAREN, "expected ')' after the parameters");
    fn->body = block();
    return fn;
}

// jodi cond { } onnothayjodi cond { } nahole { }
StmtPtr Parser::ifStatement() {
    int line = advance().line;  // 'jodi'
    auto stmt = std::make_unique<IfStmt>(line);

    stmt->conditions.push_back(expression());
    stmt->blocks.push_back(block());

    while (match(TokenType::ONNOTHAYJODI)) {
        stmt->conditions.push_back(expression());
        stmt->blocks.push_back(block());
    }
    if (match(TokenType::NAHOLE)) {
        stmt->hasElse = true;
        stmt->elseBlock = block();
    }
    return stmt;
}

// ghuro i dhoro 1 theke 10 { }
StmtPtr Parser::forStatement() {
    int line = advance().line;  // 'ghuro'
    auto stmt = std::make_unique<ForStmt>(line);

    stmt->var = consume(TokenType::IDENTIFIER, "expected a loop variable name after 'ghuro'").lexeme;
    consume(TokenType::DHORO, "expected 'dhoro' after the loop variable");
    stmt->from = expression();
    consume(TokenType::THEKE, "expected 'theke' between the start and end values");
    stmt->to = expression();
    stmt->body = block();
    return stmt;
}

StmtPtr Parser::whileStatement() {
    int line = advance().line;  // 'cholteThako'
    auto stmt = std::make_unique<WhileStmt>(line);
    stmt->condition = expression();
    stmt->body = block();
    return stmt;
}

// dekhao a, b, c;
StmtPtr Parser::printStatement() {
    int line = advance().line;  // 'dekhao'
    auto stmt = std::make_unique<PrintStmt>(line);
    do {
        stmt->values.push_back(expression());
    } while (match(TokenType::COMMA));
    consumeSemicolon("the 'dekhao' statement");
    return stmt;
}

// nao x;   or   nao "Enter x: ", x;
StmtPtr Parser::inputStatement() {
    int line = advance().line;  // 'nao'
    auto stmt = std::make_unique<InputStmt>(line);

    if (check(TokenType::STRING_LIT)) {
        stmt->hasPrompt = true;
        stmt->prompt = advance().lexeme;
        consume(TokenType::COMMA, "expected ',' after the prompt text");
    }
    stmt->name = consume(TokenType::IDENTIFIER, "expected a variable name after 'nao'").lexeme;
    consumeSemicolon("the 'nao' statement");
    return stmt;
}

StmtPtr Parser::returnStatement() {
    int line = advance().line;  // 'ferot'
    auto stmt = std::make_unique<ReturnStmt>(line);
    if (!check(TokenType::SEMICOLON)) stmt->value = expression();
    consumeSemicolon("the 'ferot' statement");
    return stmt;
}

// barao x;   komao arr[2];
StmtPtr Parser::incDecStatement() {
    Token word = advance();
    auto stmt = std::make_unique<IncDecStmt>(word.line);
    stmt->increment = (word.type == TokenType::BARAO);

    stmt->target = postfix();
    if (stmt->target->kind != ExprKind::VARIABLE && stmt->target->kind != ExprKind::INDEX) {
        throw error(word.line, "'" + word.lexeme + "' must be followed by a variable");
    }
    consumeSemicolon("the '" + word.lexeme + "' statement");
    return stmt;
}

// x dhoro 5;    arr[0] dhoro 7;    greet("Omar");
StmtPtr Parser::simpleStatement() {
    int line = peek().line;
    ExprPtr expr = expression();

    if (match(TokenType::DHORO)) {
        if (expr->kind != ExprKind::VARIABLE && expr->kind != ExprKind::INDEX) {
            throw error(line, "the left side of 'dhoro' must be a variable or a talika element");
        }
        auto stmt = std::make_unique<AssignStmt>(line);
        stmt->target = std::move(expr);
        stmt->value = expression();
        consumeSemicolon("the assignment");
        return stmt;
    }

    if (expr->kind != ExprKind::CALL) {
        throw error(line,
                    "invalid statement: expected a declaration, an assignment ('x dhoro value;') or a function call");
    }
    auto stmt = std::make_unique<ExprStmt>(line);
    stmt->expr = std::move(expr);
    consumeSemicolon("the function call");
    return stmt;
}

// ---------------------------------------------------------------
// expressions (lowest precedence first)
// ---------------------------------------------------------------
ExprPtr Parser::expression() { return logicOr(); }

ExprPtr Parser::logicOr() {
    ExprPtr left = logicAnd();
    while (check(TokenType::OTHOBA)) {
        Token op = advance();
        ExprPtr right = logicAnd();
        left = std::make_unique<BinaryExpr>(op.type, op.lexeme, std::move(left), std::move(right), op.line);
    }
    return left;
}

ExprPtr Parser::logicAnd() {
    ExprPtr left = logicNot();
    while (check(TokenType::EBONG)) {
        Token op = advance();
        ExprPtr right = logicNot();
        left = std::make_unique<BinaryExpr>(op.type, op.lexeme, std::move(left), std::move(right), op.line);
    }
    return left;
}

ExprPtr Parser::logicNot() {
    if (check(TokenType::NOI)) {
        Token op = advance();
        ExprPtr operand = logicNot();
        return std::make_unique<UnaryExpr>(op.type, op.lexeme, std::move(operand), op.line);
    }
    return equality();
}

ExprPtr Parser::equality() {
    ExprPtr left = comparison();
    while (check(TokenType::SOMOMAN)) {
        Token op = advance();
        ExprPtr right = comparison();
        left = std::make_unique<BinaryExpr>(op.type, op.lexeme, std::move(left), std::move(right), op.line);
    }
    return left;
}

ExprPtr Parser::comparison() {
    ExprPtr left = term();
    while (check(TokenType::BORO) || check(TokenType::CHOTO)) {
        Token op = advance();
        ExprPtr right = term();
        left = std::make_unique<BinaryExpr>(op.type, op.lexeme, std::move(left), std::move(right), op.line);
    }
    return left;
}

ExprPtr Parser::term() {
    ExprPtr left = factor();
    while (check(TokenType::JOG) || check(TokenType::BIYOG)) {
        Token op = advance();
        ExprPtr right = factor();
        left = std::make_unique<BinaryExpr>(op.type, op.lexeme, std::move(left), std::move(right), op.line);
    }
    return left;
}

ExprPtr Parser::factor() {
    ExprPtr left = unary();
    while (check(TokenType::GUNON) || check(TokenType::VAGFOL) || check(TokenType::VAGSHESH)) {
        Token op = advance();
        ExprPtr right = unary();
        left = std::make_unique<BinaryExpr>(op.type, op.lexeme, std::move(left), std::move(right), op.line);
    }
    return left;
}

ExprPtr Parser::unary() {
    if (check(TokenType::MINUS)) {
        Token op = advance();
        ExprPtr operand = unary();
        return std::make_unique<UnaryExpr>(op.type, "-", std::move(operand), op.line);
    }
    return power();
}

ExprPtr Parser::power() {
    ExprPtr base = postfix();
    if (check(TokenType::GHAT)) {
        Token op = advance();
        ExprPtr exponent = unary();  // right-associative: 2 ghat 3 ghat 2 = 2 ghat 9
        return std::make_unique<BinaryExpr>(op.type, op.lexeme, std::move(base), std::move(exponent), op.line);
    }
    return base;
}

ExprPtr Parser::postfix() {
    ExprPtr expr = primary();
    while (check(TokenType::LBRACKET)) {
        int line = advance().line;
        ExprPtr index = expression();
        consume(TokenType::RBRACKET, "expected ']' after the index");
        expr = std::make_unique<IndexExpr>(std::move(expr), std::move(index), line);
    }
    return expr;
}

std::vector<ExprPtr> Parser::arguments() {
    std::vector<ExprPtr> args;
    if (!check(TokenType::RPAREN)) {
        do {
            args.push_back(expression());
        } while (match(TokenType::COMMA));
    }
    consume(TokenType::RPAREN, "expected ')' after the arguments");
    return args;
}

ExprPtr Parser::primary() {
    switch (peek().type) {
        case TokenType::INT_LIT: {
            Token tok = advance();
            try {
                return std::make_unique<IntExpr>(std::stoll(tok.lexeme), tok.line);
            } catch (const std::out_of_range&) {
                throw error(tok.line, "the number " + tok.lexeme + " is too large for purno");
            }
        }
        case TokenType::FLOAT_LIT: {
            Token tok = advance();
            return std::make_unique<FloatExpr>(std::stod(tok.lexeme), tok.line);
        }
        case TokenType::STRING_LIT: {
            Token tok = advance();
            return std::make_unique<StringExpr>(tok.lexeme, tok.line);
        }
        case TokenType::SOTTO: {
            Token tok = advance();
            return std::make_unique<BoolExpr>(true, tok.line);
        }
        case TokenType::MITTHA: {
            Token tok = advance();
            return std::make_unique<BoolExpr>(false, tok.line);
        }
        case TokenType::IDENTIFIER: {
            Token tok = advance();
            if (match(TokenType::LPAREN)) {
                auto call = std::make_unique<CallExpr>(tok.lexeme, tok.line);
                call->args = arguments();
                return call;
            }
            return std::make_unique<VarExpr>(tok.lexeme, tok.line);
        }
        case TokenType::LPAREN: {
            advance();
            ExprPtr inner = expression();
            consume(TokenType::RPAREN, "expected ')' to close the expression");
            return inner;
        }
        case TokenType::MULOK: {
            Token tok = advance();
            consume(TokenType::LPAREN, "expected '(' after 'mulok'");
            ExprPtr operand = expression();
            consume(TokenType::RPAREN, "expected ')' after the value of 'mulok'");
            return std::make_unique<UnaryExpr>(tok.type, tok.lexeme, std::move(operand), tok.line);
        }
        case TokenType::LBRACKET: {
            Token tok = advance();
            auto list = std::make_unique<ListExpr>(tok.line);
            if (!check(TokenType::RBRACKET)) {
                do {
                    list->elements.push_back(expression());
                } while (match(TokenType::COMMA));
            }
            consume(TokenType::RBRACKET, "expected ']' to close the list");
            return list;
        }
        default:
            throw error(peek().line,
                        "expected a value (number, text, variable, '(' or '['), but found " + describe(peek()));
    }
}

// ===================== semantic.h =====================
#ifndef OMAR_SEMANTIC_H
#define OMAR_SEMANTIC_H

#include <string>
#include <unordered_map>
#include <vector>


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

// ===================== semantic.cpp =====================

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

// ===================== interpreter.h =====================
#ifndef OMAR_INTERPRETER_H
#define OMAR_INTERPRETER_H

#include <cstdint>
#include <deque>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


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

// ===================== interpreter.cpp =====================

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

// ===================== main.cpp =====================
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>


static void printUsage() {
    std::cerr << "Usage: omar [options] <file.omar>\n"
              << "  --version  show the Omar++ version\n"
              << "  --tokens   print the token stream (lexer output) before running\n"
              << "  --check    only check the program (lexer, parser, semantic analysis); do not run it\n";
}

int main(int argc, char* argv[]) {
    bool showTokens = false;
    bool checkOnly = false;
    std::string path;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--version") {
            std::cout << "Omar++ 1.0 (full language: lexer, parser, semantic analysis, interpreter)\n";
            return 0;
        }
        if (arg == "--tokens") showTokens = true;
        else if (arg == "--check") checkOnly = true;
        else path = arg;
    }

    if (path.empty()) {
        printUsage();
        return 1;
    }

    std::ifstream file(path);
    if (!file) {
        std::cerr << "Error: cannot open file '" << path << "'\n";
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();

    try {
        // Phase 1: lexical analysis
        Lexer lexer(buffer.str());
        std::vector<Token> tokens = lexer.tokenize();

        if (showTokens) {
            for (const Token& t : tokens) {
                std::cout << "[line " << t.line << "] " << tokenTypeName(t.type);
                if (!t.lexeme.empty()) std::cout << "  '" << t.lexeme << "'";
                std::cout << "\n";
            }
            std::cout << "----\n";
        }

        // Phase 2: syntax analysis (builds the AST)
        Parser parser(tokens);
        Program program = parser.parse();

        // Phase 3: semantic analysis
        SemanticAnalyzer analyzer;
        analyzer.analyze(program);

        if (checkOnly) {
            std::cout << "No errors found.\n";
            return 0;
        }

        // Phase 4: execution
        Interpreter interpreter;
        interpreter.run(program);
    } catch (const OmarError& e) {
        std::cout.flush();
        std::cerr << e.phase() << " [line " << e.line() << "]: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
