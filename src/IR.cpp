#include "include/IR.h"

std::unique_ptr<llvm::LLVMContext> TheContext;
std::unique_ptr<llvm::IRBuilder<>> Builder;   // 用于构建指令
std::unique_ptr<llvm::Module> TheModule;   // 包含函数和全局变量的容器
std::map<std::string, llvm::Value *> NamedValues;    // 跟踪当前范围中定义了哪些值以及它们的LLVM表示形式