#include "parser.h"

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
