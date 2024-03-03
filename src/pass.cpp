#include "include/pass.h"

std::unique_ptr<llvm::legacy::FunctionPassManager> TheFPM;  // 管理函数优化