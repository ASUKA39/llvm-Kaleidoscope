#include "include/parser.h"

std::map<char, int> BinopPrecedence;

std::unique_ptr<ExprAST> ParseNumberExpr() {
    auto Result = std::make_unique<NumberExprAST>(NumVal);  // 返回当前数字
    getNextToken();     // 转向下一个token
    return std::move(Result);
}

std::unique_ptr<ExprAST> ParseIdentifierExpr() {    // 解析标识符
    std::string IdName = IdentifierStr; // 标识符名

    getNextToken();  // 消耗掉标识符

    if (CurTok != '(') {  // 如果不是括号，意味着不是函数调用
        return std::make_unique<VariableExprAST>(IdName);   // 返回变量引用
    }

    getNextToken();  // 如果是括号，说明是函数调用，消耗掉左括号
    std::vector<std::unique_ptr<ExprAST>> Args; // 函数参数
    if (CurTok != ')') {
        while (true) {
            if (auto Arg = ParseExpression()){  // 解析参数
                Args.push_back(std::move(Arg));   // 添加参数
            } else {
                return nullptr;
            }

            if (CurTok == ')') {    // 如果遇到右括号，说明参数解析完毕
                break;
            }
            if (CurTok != ',') {
                return LogError("Expected ')' or ',' in argument list");
            }
            getNextToken();
        }        
    }

    getNextToken();  // 消耗掉右括号

    return std::make_unique<CallExprAST>(IdName, std::move(Args));
}

/*
    整体：LHS OP RHS=(LHS_ OP_ RHS_)
    1. 解析LHS
    2. 吞掉OP
    3. 解析RHS，就像解析LHS一样（递归）
    4. 合并LHS、OP、RHS作为新的LHS，继续解析它的新OP和RHS
    边界：
    1. 当前RHS的OP优先级小于原OP，返回当前的LHS
    2. 当前RHS的OP优先级大于原OP，递归解析当前RHS的OP和RHS，然后合并成一个整体，更新作为新的RHS
    3. 当前RHS的OP优先级等于原OP，无事发生，将当前RHS合并到LHS上作为新的LHS，然后继续解析下一个OP和RHS
*/

std::unique_ptr<ExprAST> ParseParenExpr() {  // 解析括号表达式  (LHS, OP, RHS)    (a * b + c)
    getNextToken();  // 消掉左括号
    auto V = ParseExpression(); // 解析括号中间的表达式
    if (!V) {
        return nullptr;
    }

    if (CurTok != ')') {    // 检查右括号
        return LogError("Expected ')'");
    }
    getNextToken();  // 消掉右括号
    return V;
}

std::unique_ptr<ExprAST> ParseExpression() {    // 解析表达式   a * b + c
    auto LHS = ParsePrimary();  // 解析主表达式，即表达式的左半部分，可以是标识符、数字或者括号表达式   a
    if (!LHS) {
        return nullptr;
    }

    return ParseBinOpRHS(0, std::move(LHS));    // 解析操作符及其右操作数，初始优先级最低设为0   [*, b + c]
}

std::unique_ptr<ExprAST> ParseBinOpRHS(int ExprPerc, std::unique_ptr<ExprAST> LHS) {    // 解析二元操作符及其右操作数   [OP, RHS]    [*, b + c]
    while (true) {
        int TokPrec = GetTokPrecedence();   // 获取当前操作符的优先级   [OP] RHS)   [*], b + c

        if (TokPrec < ExprPerc) {   // 如果当前操作符的优先级小于上一个操作符的优先级
            return LHS;     // 若解析到 a * b [+] c，那么应该返回 [a * b] 作为下一次要递归解析的左操作数，相对地 [+, c] 就是下一次要解析的操作符和右操作数
        }

        int BinOp = CurTok;
        getNextToken();  // 消耗掉操作符，现在CurTok是表达式的右半部分  b + c

        auto RHS = ParsePrimary();  // 解析右操作数，首先将其当做一个独立的表达式，递归解析其主表达式   [b] + c
        if (!RHS) {
            return nullptr;
        }

        int NextPrec = GetTokPrecedence();  // 获取下一个操作符的优先级  b [+] c
        if (TokPrec < NextPrec) {   // 如果当前操作符的优先级小于下一个操作符的优先级，说明需要继续递归解析     比如 a + b * c 解析到 * 时，* 的优先级大于 +，所以需要继续解析，让更高优先级的表达式作为一个右操作数整体
            RHS = ParseBinOpRHS(TokPrec + 1, std::move(RHS));   // 递归解析下一个操作符  比如 a + b * c 现在要解析 b * c    最终递归完返回赋值更新给RHS的是一个包含原RHS的、与原RHS同级的一整块表达式
            if (!RHS) {
                return nullptr;
            }
        }
        // 终于将一块高优先级的“高地”解析完了，现在我们需要将高地合并成一个整体，然后才能继续解析后面的表达式
        LHS = std::make_unique<BinaryExprAST>(BinOp, std::move(LHS), std::move(RHS));   // LHS合并操作符和RHS生成二元操作表达式AST作为新的LHS  (NEW_LHS, OP, RHS)  ([a * b], +, c)
    }   // 重复解析操作符及其右操作数
}

std::unique_ptr<FunctionAST> ParseDefinition() {    // 解析函数定义
    getNextToken(); // 消耗掉def
    auto Proto = ParsePrototype();  // 解析函数原型（函数名和参数）
    if (!Proto) {
        return nullptr;
    }

    if (auto E = ParseExpression()) {   // 解析函数体（函数体是表达式，因为kaleidoscope是一种约束式（或函数式）语言，其函数体设计为纯表达式）
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));   // 函数AST，包含原型AST（函数名和参数）和函数体AST（表达式）
    }
    return nullptr;
}

std::unique_ptr<PrototypeAST> ParseExtern() {   // 解析extern声明
    getNextToken();  // 消耗掉extern
    return ParsePrototype();    // 解析函数原型
}

std::unique_ptr<PrototypeAST> ParsePrototype() {    // 解析函数原型
    if (CurTok != tok_identifier) { // 如果不是标识符，说明当前指向的不是函数名，返回错误
        return LogErrorP("Expected function name in prototype");
    }

    std::string FnName = IdentifierStr; // 函数名
    getNextToken(); // 消耗掉函数名

    if (CurTok != '(') {    // 如果不是左括号，说明当前指向的不是参数列表的左括号，返回错误
        return LogErrorP("Expected '(' in prototype");
    }

    std::vector<std::string> ArgNames;
    while (getNextToken() == tok_identifier) {  // 解析参数列表，直到遇到右括号
        ArgNames.push_back(IdentifierStr);  // 按顺序保存参数名
    }
    if (CurTok != ')') {    // 如果不是右括号，说明参数列表解析完毕，返回错误
        return LogErrorP("Expected ')' in prototype");
    }

    getNextToken();  // 消耗掉右括号

    return std::make_unique<PrototypeAST>(FnName, std::move(ArgNames)); // 返回函数原型AST
}

std::unique_ptr<FunctionAST> ParseTopLevelExpr() {  // 解析顶层表达式，即匿名函数，表现形式是一个没头没尾的纯表达式，比如1+1
    if (auto E = ParseExpression()) {   // 匿名函数没有def和原型，函数体是表达式，所以直接解析表达式即可
        auto Proto = std::make_unique<PrototypeAST>("", std::vector<std::string>());    // 匿名函数没有函数名和参数，所以这里创建一个空的函数原型
        return std::make_unique<FunctionAST>(std::move(Proto), std::move(E));   // 返回匿名函数AST，包含空的函数原型AST和作为函数体的表达式AST
    }
    return nullptr;
}

int GetTokPrecedence() {    // 获取操作符优先级
    if (!isascii(CurTok)) {
        return -1;
    }

    int TokPrec = BinopPrecedence[CurTok];
    if (TokPrec <= 0) {
        return -1;
    }
    return TokPrec;
}

std::unique_ptr<ExprAST> ParsePrimary() {   // 解析主表达式，即一个标识符、数字或者一个括号表达式
    switch (CurTok) {
        default:
            return LogError("unknown token when Exprcting an expression");
        case tok_identifier:    // 标识符
            return ParseIdentifierExpr();
        case tok_number:    // 数字
            return ParseNumberExpr();
        case '(':   // 括号，即括号表达式
            return ParseParenExpr();
    }
}