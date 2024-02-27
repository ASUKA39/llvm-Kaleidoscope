#ifndef __LEXER_H__
#define __LEXER_H__

#include "token.h"

#include <cstdlib>
#include <iostream>
#include <string>

extern std::string IdentifierStr; // Filled in if tok_identifier
extern double NumVal;             // Filled in if tok_number
extern int CurTok;

int gettok();
int getNextToken();

#endif