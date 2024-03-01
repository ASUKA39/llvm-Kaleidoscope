#include "include/lexer.h"

std::string IdentifierStr; // Filled in if tok_identifier
double NumVal;             // Filled in if tok_number
int CurTok;

// gettok - 返回下一个标记
int gettok(){
    static int LastChar = ' ';  // 上一个字符，静态变量保持上一次调用的状态

    while (isspace(LastChar)){
        LastChar = getchar();
    }

    if (isalpha(LastChar)){ // identifier: [a-zA-Z][a-zA-Z0-9]*
        IdentifierStr = LastChar;
        while (isalnum((LastChar = getchar()))){
            IdentifierStr += LastChar;
        }

        if (IdentifierStr == "def"){
            return tok_def;
        }
        if (IdentifierStr == "extern"){
            return tok_extern;
        }
        return tok_identifier;
    }
    
    if (isdigit(LastChar) || LastChar == '.') { // Number: [0-9.]+
        std::string NumStr;
        do {
            NumStr += LastChar;
            LastChar = getchar();
        } while (isdigit(LastChar) || LastChar == '.');

        NumVal = strtod(NumStr.c_str(), 0); // string to double
        return tok_number;
    }

    if (LastChar == '#') {  // 注释行
        do {
            LastChar = getchar();
        } while (LastChar != EOF && LastChar != '\n' && LastChar != '\r');

        if (LastChar != EOF){
            return gettok();
        }
    }
    
    // 检查文件结束符
    if (LastChar == EOF){
        return tok_eof;
    }

    // 否则，返回字符本身（比如二元操作符、括号等）
    int ThisChar = LastChar;
    LastChar = getchar();
    
    return ThisChar;
}

int getNextToken() {
    return CurTok = gettok();   // 获取下一个标记
}