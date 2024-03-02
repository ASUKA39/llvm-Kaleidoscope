#include "include/logerr.h"

std::unique_ptr<ExprAST> LogError(const char *Str) {
    std::cerr << "LogError: " << Str << std::endl;
    return nullptr;
}

std::unique_ptr<PrototypeAST> LogErrorP(const char *Str) {
    LogError(Str);
    return nullptr;
}

llvm::Value *LogErrorV(const char *Str) {
    LogError(Str);
    return nullptr;
}