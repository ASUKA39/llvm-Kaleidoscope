#include "include/ast.h"
#include "include/lexer.h"
#include "include/logerr.h"
#include "include/parser.h"
#include "include/token.h"
#include "include/IR.h"

#include <cstdio>
#include <map>
#include <memory>
#include <string>

#include "IR/IRBuilder.h"
#include "IR/LLVMContext.h"
#include "IR/Module.h"

static void HandleDefinition() {
  if (auto FnAST = ParseDefinition()) {
    // fprintf(stderr, "Parsed a function definition.\n");
    if (auto *FnIR = FnAST->codegen()) {
      fprintf(stderr, "Read function definition:");
      FnIR->print(llvm::errs());
      fprintf(stderr, "\n");
    }
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleExtern() {
  if (auto ProtoAST = ParseExtern()) {
    // fprintf(stderr, "Parsed an extern\n");
    if (auto *FnIR = ProtoAST->codegen()) {
      fprintf(stderr, "Read extern: ");
      FnIR->print(llvm::errs());
      fprintf(stderr, "\n");
    }
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  // Evaluate a top-level expression into an anonymous function.
  if (auto FnAST = ParseTopLevelExpr()) {
    // fprintf(stderr, "Parsed a top-level expr\n");
    if (auto *FnIR = FnAST->codegen()) {
      fprintf(stderr, "Read top-level expression:");
      FnIR->print(llvm::errs());
      fprintf(stderr, "\n");

      FnIR->eraseFromParent();
    }
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void mainLoop() {
    while (true) {
        fprintf(stderr, ">>> ");
        switch (CurTok) {
            case tok_eof:  // 文件结束符
                return;
            case ';': // 分号，表示一行结束
                getNextToken(); // 消耗掉分号
                break;
            case tok_def:  // 函数定义
                HandleDefinition();
                break;
            case tok_extern:  // extern声明
                HandleExtern();
                break;
            default:  // 其他情况，认定为顶层表达式
                HandleTopLevelExpression();
                break;
        }
    }
}

static void InitializeModule() {
  TheContext = std::make_unique<llvm::LLVMContext>();
  TheModule = std::make_unique<llvm::Module>("my jit", *TheContext);
  Builder = std::make_unique<llvm::IRBuilder<>>(*TheContext);
}

int main() {
    BinopPrecedence['<'] = 10;
    BinopPrecedence['+'] = 20;
    BinopPrecedence['-'] = 20;
    BinopPrecedence['*'] = 40;

    // Prime the first token.
    fprintf(stderr, ">>> ");
    getNextToken();

    InitializeModule();

    // Run the main "interpreter loop" now.
    mainLoop();

    TheModule->print(llvm::errs(), nullptr);

    return 0;
}