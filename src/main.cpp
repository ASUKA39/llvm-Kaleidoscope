#include "include/ast.h"
#include "include/lexer.h"
#include "include/logerr.h"
#include "include/parser.h"
#include "include/token.h"

static void HandleDefinition() {
  if (ParseDefinition()) {
    fprintf(stderr, "Parsed a function definition.\n");
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleExtern() {
  if (ParseExtern()) {
    fprintf(stderr, "Parsed an extern\n");
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void HandleTopLevelExpression() {
  // Evaluate a top-level expression into an anonymous function.
  if (ParseTopLevelExpr()) {
    fprintf(stderr, "Parsed a top-level expr\n");
  } else {
    // Skip token for error recovery.
    getNextToken();
  }
}

static void mainLoop() {
    while (true) {
        fprintf(stderr, "ready> ");
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

int main() {
    BinopPrecedence['<'] = 10;
    BinopPrecedence['+'] = 20;
    BinopPrecedence['-'] = 20;
    BinopPrecedence['*'] = 40;

    // Prime the first token.
    fprintf(stderr, "ready> ");
    getNextToken();

    // Run the main "interpreter loop" now.
    mainLoop();

    return 0;
}