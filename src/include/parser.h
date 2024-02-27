#ifndef __PARSER_H__
#define __PARSER_H__

#include "ast.h"
#include "lexer.h"
#include "token.h"
#include "logerr.h"

#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <vector>

extern std::map<char, int> BinopPrecedence;

int GetTokPrecedence();
std::unique_ptr<ExprAST> ParseNumberExpr();
std::unique_ptr<ExprAST> ParseParenExpr();
std::unique_ptr<ExprAST> ParseIdentifierExpr();
std::unique_ptr<ExprAST> ParsePrimary();
std::unique_ptr<ExprAST> ParseExpression();
std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPrec, std::unique_ptr<ExprAST> LHS);
std::unique_ptr<PrototypeAST> ParsePrototype();
std::unique_ptr<PrototypeAST> ParseExtern();
std::unique_ptr<FunctionAST> ParseDefinition();
std::unique_ptr<FunctionAST> ParseTopLevelExpr();

#endif