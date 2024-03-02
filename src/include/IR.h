#ifndef __IR_H__
#define __IR_H__

#include <map>

#include "IR/IRBuilder.h"
#include "IR/LLVMContext.h"
#include "IR/Module.h"

extern std::unique_ptr<llvm::LLVMContext> TheContext;
extern std::unique_ptr<llvm::IRBuilder<>> Builder;
extern std::unique_ptr<llvm::Module> TheModule;
extern std::map<std::string, llvm::Value *> NamedValues;

#endif