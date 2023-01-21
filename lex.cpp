#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

// 0-255 (ASCII char) returned for unknown characters, otherwise negative num as below
enum Token {
  tok_eof = -1,

  // keywords
  tok_def = -2,
  tok_extern = -3,

  // names + numbers
  tok_identifier = -4,
  tok_number = -5,
};

// global values for names and number values for some reason
static std::string IdentifierStr; // Filled in if tok_identifier
static double NumVal;             // Filled in if tok_number

// Return the next token from standard input
static int gettok() {
  static int LastChar = ' ';

  // skip whitespace
  while (std::isspace(LastChar))
    LastChar = getchar();

  if (std::isalpha(LastChar)) { // identifier: [a-zA-Z][a-zA-Z0-9]*
    IdentifierStr = LastChar;
    while (std::isalnum((LastChar = getchar())))
      IdentifierStr += LastChar;

    if (IdentifierStr == "def")
      return tok_def;
    if (IdentifierStr == "extern")
      return tok_extern;
    return tok_identifier;
  }

  if (isdigit(LastChar) || LastChar == '.') { // number: [0-9.]+
    std::string NumStr;
    do {
      NumStr += LastChar;
      LastChar = getchar();
    } while (isdigit(LastChar) || LastChar == '.');

    NumVal = std::strtod(NumStr.c_str(), 0);
    return tok_number;
  }

  // comment until end of line
  if (LastChar == '#') {
    do
      LastChar = getchar();
    while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

    if (LastChar != EOF)
      return gettok();
  }

  if (LastChar == EOF)
    return tok_eof;

  // if nothing else, return char as ascii value
  int ThisChar = LastChar;
  LastChar = getchar();
  return ThisChar;
}

/// ---------------------------------------------------------------------
/// AST
/// ---------------------------------------------------------------------
// ExprAST - Base class for all expression nodes
class ExprAST {
public:
  virtual ~ExprAST() {}
};

// NumberExprtAST - expression class for numerical literals
class NumberExprtAST : public ExprAST {
  double Val;

public:
  NumberExprtAST(double Val) : Val(Val) {}
};

class VariableExprtAST : public ExprAST {
  std::string Name;

public:
  VariableExprtAST(const std::string &Name) : Name(Name) {}
};

// BinaryExprtAST - Expression class for dyadic ops
class BinaryExprtAST : public ExprAST {
  char Op;
  std::unique_ptr<ExprAST> LHS, RHS;

public:
  BinaryExprtAST(char op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
    : Op(op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

// CallExprAST - Expression class for function calls
class CallExprAST : public ExprAST {
  std::string Callee;
  std::vector<std::unique_ptr<ExprAST>> Args;

public:
  CallExprAST(const std::string &Callee,
      std::vector<std::unique_ptr<ExprAST>> Args)
    : Callee(Callee), Args(std::move(Args)) {}
};

/// PrototypeAST - Represents the prototype for a function,
/// which captures its name, its argument names (+ implicitly, number of arguments)
class PrototypeAST {
  std::string Name;
  std::vector<std::string> Args;

public:
  PrototypeAST(const std::string &name, std::vector<std::string> Args)
    : Name(name), Args(std::move(Args)) {}

  const std::string &getName() const { return Name; }
};

// FunctionAST - Represents a function definition itself
class FunctionAST {
  std::unique_ptr<PrototypeAST> Proto;
  std::unique_ptr<ExprAST> Body;

public:
  FunctionAST(std::unique_ptr<PrototypeAST> Proto,
      std::unique_ptr<ExprAST> Body)
    : Proto(std::move(Proto)), Body(std::move(Body)) {}
};


