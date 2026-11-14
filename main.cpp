#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include "interpreter.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "token.h"

static void printUsage() {
    std::cerr << "Usage: omar [options] <file.omar>\n"
              << "  --tokens   print the token stream (lexer output) before running\n"
              << "  --check    only check the program (lexer, parser, semantic analysis); do not run it\n";
}

int main(int argc, char* argv[]) {
    bool showTokens = false;
    bool checkOnly = false;
    std::string path;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
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
