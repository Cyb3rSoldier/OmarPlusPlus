#include "lexer.h"

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
