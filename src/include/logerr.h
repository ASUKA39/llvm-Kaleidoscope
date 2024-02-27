#ifndef __LOGERR_H__
#define __LOGERR_H__

#include "ast.h"

#include <iostream>
#include <memory>

std::unique_ptr<ExprAST> LogError(const char *Str);
std::unique_ptr<PrototypeAST> LogErrorP(const char *Str);

#endif