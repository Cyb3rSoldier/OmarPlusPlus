# Omar++

A small programming language with Bangla-style keywords, built from scratch in C++ for the Compiler Lab.
It has all four compiler phases: a **lexer**, a **recursive descent parser** that builds an **AST**, a **semantic analyzer** with a symbol table, and a **tree-walking interpreter**.

```
source.omar -> Lexer -> tokens -> Parser -> AST -> Semantic Analyzer -> Interpreter -> output
               (1)                (2)             (3)                  (4)
```

## Build

Needs a C++17 compiler (g++ or clang++).

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o omar main.cpp lexer.cpp parser.cpp semantic.cpp interpreter.cpp
```

On Windows (MSYS2 / MinGW) the program is `omar.exe`. Build it in a folder that is not synced by OneDrive if the `.exe` keeps disappearing.

## Run

```bash
./omar examples/hello.omar
./omar --tokens examples/hello.omar    # also print the token stream (phase 1)
./omar --check  examples/showcase.omar # lexer + parser + semantic check only, do not run
```

## Keywords (every row of the proposal)

| Function | Omar++ | Example |
|---|---|---|
| Print | `dekhao` | `dekhao "Hi", x;` |
| Input | `nao` | `nao x;` or `nao "Enter x: ", x;` |
| Variable | `dhoro` | `purno x dhoro 5;` (declare) / `x dhoro 6;` (assign) |
| Constant | `drubok` | `drubok dosomik PI dhoro 3.14;` |
| Addition | `jog` | `a jog b` |
| Subtraction | `biyog` | `a biyog b` |
| Multiplication | `gunon` | `a gunon b` |
| Division | `vagfol` | `a vagfol b` |
| Modulus | `vagshesh` | `a vagshesh b` |
| Power | `ghat` | `2 ghat 10` |
| Square root | `mulok` | `mulok(144)` |
| If | `jodi` | `jodi x boro 5 { ... }` |
| Else if | `onnothayjodi` | `onnothayjodi x somoman 5 { ... }` |
| Else | `nahole` | `nahole { ... }` |
| For loop | `ghuro` | `ghuro i dhoro 1 theke 10 { ... }` |
| While loop | `cholteThako` | `cholteThako x choto 10 { ... }` |
| Function | `kaj` | `kaj purno add(purno a, purno b) { ... }` |
| Return | `ferot` | `ferot a jog b;` |
| Break | `thamo` | `thamo;` |
| Continue | `chaliyejao` | `chaliyejao;` |
| List | `talika` | `talika a dhoro [1, 2, 3];` |
| String | `lekha` | `lekha s dhoro "text";` |
| Integer | `purno` | `purno n dhoro 5;` |
| Float | `dosomik` | `dosomik f dhoro 2.5;` |
| Boolean | `sottoMitha` | `sottoMitha ok dhoro sotto;` |
| AND | `ebong` | `a ebong b` |
| OR | `othoba` | `a othoba b` |
| NOT | `noi` | `noi a` |
| Equal | `somoman` | `a somoman b` |
| Greater than | `boro` | `a boro b` |
| Less than | `choto` | `a choto b` |
| Increment | `barao` | `barao x;` |
| Decrement | `komao` | `komao x;` |
| Comment | `tika` | `tika this is a comment` |
| End program | `shesh` | last line of every program |

### Small extras that the proposal did not list

These were needed to make the language usable:

| Extra | Meaning |
|---|---|
| `sotto` / `mittha` | the boolean values true / false |
| `theke` | "to", used in `ghuro i dhoro 1 theke 10` |
| `dorgho(x)` | length of a `talika` or `lekha` |
| `jukto(list, value)` | add a value at the end of a `talika` |
| `{ }` `;` `,` `[ ]` `( )` | blocks, statement end, separators, list/index, grouping |
| `-x` | unary minus for negative numbers |

## Language guide

A program is a list of statements ending with `shesh`.

```
tika Hello program
lekha name dhoro "Omar";
dekhao "Hello,", name;
shesh
```

**Types:** `purno` (64-bit integer), `dosomik` (decimal), `lekha` (text), `sottoMitha` (`sotto` / `mittha`), `talika` (list).
A variable declared without a value gets a default: `0`, `0.0`, `""`, `mittha`, `[]`.

**Operators**, from lowest to highest precedence:

| Level | Operators |
|---|---|
| 1 | `othoba` |
| 2 | `ebong` |
| 3 | `noi` |
| 4 | `somoman` |
| 5 | `boro` `choto` |
| 6 | `jog` `biyog` |
| 7 | `gunon` `vagfol` `vagshesh` |
| 8 | unary `-` |
| 9 | `ghat` (right-associative: `2 ghat 3 ghat 2` is `2 ghat 9`) |

Because `noi` is below comparison, `noi a somoman b` means `noi (a somoman b)`.
There is no `>=`, `<=` or `!=`, but they are easy to build: `noi a choto b` is `a >= b`, `noi a somoman b` is `a != b`.

**Rules worth knowing**

- `purno vagfol purno` stays `purno` (`7 vagfol 2` is `3`). Use a `dosomik` for decimals (`7.0 vagfol 2` is `3.5`).
- A `purno` can be stored in a `dosomik`, but not the other way round. `mulok` always gives a `dosomik`.
- A `dosomik` prints with a decimal point, so `12.0` is a `dosomik` and `12` is a `purno`.
- `jog` joins text when either side is a `lekha`: `"Total: " jog 42` gives `Total: 42`.
- `ghuro i dhoro 1 theke 5` runs for 1, 2, 3, 4, 5 (the end value is included). If the start is bigger than the end it counts down: `ghuro i dhoro 5 theke 1`. The loop variable cannot be changed inside the loop.
- `dekhao a, b, c;` prints the values separated by a space, then a new line.
- `ebong` and `othoba` stop early: the right side is not evaluated if the left side already decides the result.
- Every `{ }` block has its own scope. A variable declared inside a block disappears after it.
- Functions are defined at the top level only. They can be called before their definition, can call themselves, and can read global variables declared above them. A function without a return type returns nothing.
- A `talika` is shared when copied: after `talika b dhoro a;` both names refer to the same list. Elements may have mixed types, and lists can be nested (`m[0][1]`).
- `shesh` as the last line ends the program. `shesh;` inside a block or function stops the program immediately.
- Recursion is limited by the stack: several hundred levels on Windows, thousands on Linux/macOS. Deeper recursion gives a clean runtime error, not a crash.

## Grammar (EBNF)

```
program      -> statement* "shesh"
statement    -> varDecl | funcDecl | ifStmt | forStmt | whileStmt | printStmt
              | inputStmt | returnStmt | breakStmt | continueStmt | incDecStmt
              | exitStmt | block | assignOrCall
varDecl      -> ["drubok"] type IDENT ["dhoro" expression] ";"
type         -> "purno" | "dosomik" | "lekha" | "sottoMitha" | "talika"
funcDecl     -> "kaj" [type] IDENT "(" [param ("," param)*] ")" block
param        -> type IDENT
ifStmt       -> "jodi" expression block ("onnothayjodi" expression block)* ["nahole" block]
forStmt      -> "ghuro" IDENT "dhoro" expression "theke" expression block
whileStmt    -> "cholteThako" expression block
printStmt    -> "dekhao" expression ("," expression)* ";"
inputStmt    -> "nao" [STRING ","] IDENT ";"
returnStmt   -> "ferot" [expression] ";"
breakStmt    -> "thamo" ";"
continueStmt -> "chaliyejao" ";"
incDecStmt   -> ("barao" | "komao") postfix ";"
exitStmt     -> "shesh" [";"]
block        -> "{" statement* "}"
assignOrCall -> expression ["dhoro" expression] ";"

expression   -> logic_or
logic_or     -> logic_and ("othoba" logic_and)*
logic_and    -> logic_not ("ebong" logic_not)*
logic_not    -> "noi" logic_not | equality
equality     -> comparison ("somoman" comparison)*
comparison   -> term (("boro" | "choto") term)*
term         -> factor (("jog" | "biyog") factor)*
factor       -> unary (("gunon" | "vagfol" | "vagshesh") unary)*
unary        -> "-" unary | power
power        -> postfix ("ghat" unary)?
postfix      -> primary ("[" expression "]")*
primary      -> INT | FLOAT | STRING | "sotto" | "mittha"
              | IDENT | IDENT "(" [expression ("," expression)*] ")"
              | "(" expression ")" | "mulok" "(" expression ")"
              | "[" [expression ("," expression)*] "]"
```

## Compiler phases and error messages

Every error is printed as `<Phase> [line N]: message` and the program stops with exit code 1.
The whole file goes through phases 1 to 3 **before** anything runs, so a program with a compile-time error prints no output at all.

| Phase | File | Finds | Example message |
|---|---|---|---|
| Lexical | `lexer.cpp` | bad characters, unterminated text | `Lexical Error [line 1]: unexpected character '@'` |
| Syntax | `parser.cpp` | wrong grammar, missing `;` `}` `shesh` | `Syntax Error [line 1]: expected ';' at the end of the 'dekhao' statement, but found 'dekhao'` |
| Semantic | `semantic.cpp` | undeclared / redeclared names, constants, type mismatch, `thamo` outside a loop, `ferot` outside a function, wrong arguments | `Semantic Error [line 2]: cannot change the constant 'MAX'` |
| Runtime | `interpreter.cpp` | division by zero, index out of range, overflow, bad input | `Runtime Error [line 2]: index 5 is out of range (size 3)` |

The semantic analyzer uses a stack of symbol tables (one per scope) and infers a type for every expression.
Values taken out of a `talika` have a type that is only known at runtime, so those are checked when the program runs.

## Examples

| File | Shows |
|---|---|
| `examples/hello.omar` | `dekhao`, strings, comments |
| `examples/calculator.omar` | all arithmetic operators, precedence |
| `examples/variables.omar` | types, `dhoro`, `drubok`, `barao`, `komao` |
| `examples/conditions.omar` | `jodi`, `onnothayjodi`, `nahole`, `ebong`, `othoba`, `noi` |
| `examples/loops.omar` | `ghuro`, `cholteThako`, `thamo`, `chaliyejao` |
| `examples/functions.omar` | `kaj`, `ferot`, recursion (factorial, fibonacci) |
| `examples/arrays.omar` | `talika`, indexing, `jukto`, `dorgho`, nested lists |
| `examples/input.omar` | `nao` (run it and type a name and two numbers) |
| `examples/showcase.omar` | primes, gcd, bubble sort, early `shesh` |
| `examples/errors/*.omar` | one program for each kind of error |

Try the error examples:

```bash
for f in examples/errors/*.omar; do echo "$f"; ./omar $f; done
```

## Project structure

```
OmarPlusPlus/
├── main.cpp          reads the file and runs the phases
├── token.h           token types, Token, OmarError
├── lexer.h/.cpp      phase 1: source text -> tokens
├── ast.h             AST node definitions and the Type enum
├── parser.h/.cpp     phase 2: tokens -> AST (recursive descent)
├── semantic.h/.cpp   phase 3: symbol table and type checking
├── interpreter.h/.cpp phase 4: runs the AST
├── examples/
└── README.md
```
