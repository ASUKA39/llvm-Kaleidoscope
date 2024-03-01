#ifndef __AST_H__
#define __AST_H__

#include <memory>
#include <string>
#include <vector>

class ExprAST { // AST基类
  public:
    virtual ~ExprAST() = default;
};

class NumberExprAST : public ExprAST {  // 数字常量
    double Val; // 值，仅支持double类型

  public:
    NumberExprAST(double Val) : Val(Val) {}
};

class VariableExprAST : public ExprAST {    // 变量引用
    std::string Name;

  public:
    VariableExprAST(const std::string &Name) : Name(Name) {}
};

class BinaryExprAST : public ExprAST {  // 二元操作
    char Op;  // 操作符
    std::unique_ptr<ExprAST> LHS, RHS;  // 左右操作数，类型为ExprAST（操作数也可以是表达式）

  public:
    BinaryExprAST(char Op, std::unique_ptr<ExprAST> LHS, std::unique_ptr<ExprAST> RHS)
        : Op(Op), LHS(std::move(LHS)), RHS(std::move(RHS)) {}
};

class CallExprAST : public ExprAST {    // 函数调用
    std::string Callee; // 函数名
    std::vector<std::unique_ptr<ExprAST>> Args; // 参数，类型为ExprAST（传参也可以是表达式）

  public:
    CallExprAST(const std::string &Callee, std::vector<std::unique_ptr<ExprAST>> Args)
        : Callee(Callee), Args(std::move(Args)) {}
};

class PrototypeAST {    // 函数原型
    std::string Name;  // 函数名
    std::vector<std::string> Args;  // 参数名

  public:
    PrototypeAST(const std::string &Name, std::vector<std::string> Args)
        : Name(Name), Args(std::move(Args)) {}

    const std::string &getName() const { return Name; }
};

class FunctionAST {     // 函数定义
    std::unique_ptr<PrototypeAST> Proto;  // 函数原型，类型为PrototypeAST
    std::unique_ptr<ExprAST> Body;  // 函数体，类型为ExprAST

  public:
    FunctionAST(std::unique_ptr<PrototypeAST> Proto, std::unique_ptr<ExprAST> Body)
        : Proto(std::move(Proto)), Body(std::move(Body)) {}
};

#endif