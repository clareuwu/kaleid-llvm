#include <iostream>

// 0-255 returned for unknown characters, otherwise negative num as below
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

