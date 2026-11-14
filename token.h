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
