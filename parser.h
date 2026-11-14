#ifndef OMAR_PARSER_H
#define OMAR_PARSER_H

#include <string>
#include <vector>

#include "ast.h"
#include "token.h"

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
