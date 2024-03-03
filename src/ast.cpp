#include "include/ast.h"
#include "include/logerr.h"

llvm::Value *NumberExprAST::codegen() {
    return llvm::ConstantFP::get(*TheContext, llvm::APFloat(Val));   // ConstantFP::get()用于创建常量浮点数  APFloat用于表示任意精度的浮点常量
}

llvm::Value *VariableExprAST::codegen() {
    llvm::Value *V = NamedValues[Name]; // 从NamedValues中查找变量名，返回对应的Value
    if (!V) {
        return LogErrorV("Unknown variable name");
    }
    return V;
}

llvm::Value *BinaryExprAST::codegen() {
    // 实际上LLVM要求LHS和RHS的类型必须一致，只不过这里我们只支持double类型，所以不用做类型检查
    llvm::Value *L = LHS->codegen(); // 递归调用codegen()，得到左操作数的Value
    llvm::Value *R = RHS->codegen(); // 同
    if (!L || !R) {     // 要求左右操作数都不为空
        return nullptr;
    }

    switch (Op) {
        case '+':
            return Builder->CreateFAdd(L, R, "addtmp");
        case '-':
            return Builder->CreateFSub(L, R, "subtmp");
        case '*':
            return Builder->CreateFMul(L, R, "multmp");
        case '<':
            L = Builder->CreateFCmpULT(L, R, "cmptmp");  // CreateFCmpULT()用于创建比较指令，返回一个i1类型的Value（i1是LLVM中的布尔类型，是一位整数），创建结果给L
            return Builder->CreateUIToFP(L, llvm::Type::getDoubleTy(*TheContext), "booltmp"); // CreateUIToFP()用于创建整数（上面的i1类型的结果）到浮点数的转换指令
        default:
            return LogErrorV("Invalid binary operator");
    }
}

llvm::Value *CallExprAST::codegen() {
    llvm::Function *CalleeF = TheModule->getFunction(Callee); // 从全局Module中查找函数名，返回对应的Function
    if (!CalleeF) {
        return LogErrorV("Unknown function referenced");
    }
    if (CalleeF->arg_size() != Args.size()) {  // 检查参数个数是否匹配
        return LogErrorV("Incorrect # arguments passed");
    }

    std::vector<llvm::Value *> ArgsV;   // 用于存放参数的Value
    for (unsigned i = 0, e = Args.size(); i != e; ++i) {
        ArgsV.push_back(Args[i]->codegen());  // 递归调用codegen()，得到参数的Value
        if (!ArgsV.back()) {
            return nullptr;
        }
    }

    return Builder->CreateCall(CalleeF, ArgsV, "calltmp");  // CreateCall创建函数调用指令
}

llvm::Function *PrototypeAST::codegen() {   // 函数原型
    // Type::getDoubleTy(TheContext)用于创建double类型
    // Args保存了Prototype里的参数名，注意和CallExprAST的Args区分，后者表示作为传入参数的表达式
    std::vector<llvm::Type *> Doubles(Args.size(), llvm::Type::getDoubleTy(*TheContext));  // 创建一个vector，元素个数为Args.size()，每个元素都是double类型
    llvm::FunctionType *FT = llvm::FunctionType::get(llvm::Type::getDoubleTy(*TheContext), Doubles, false);  // 创建函数类型，返回值为double，参数为Doubles中创建的一串double，最后一个参数表示非可变参数
    // 注意要将std::string转换为StringRef，否则无法将std::string转换为Twine，具体原因文档未提及
    llvm::Function *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, llvm::StringRef(Name), TheModule.get());  // 创建函数，函数类型为FT，链接方式为ExternalLinkage（表示函数可以被其他Module访问），函数名为Name，函数所在的Module为TheModule

    unsigned Idx = 0;
    for (auto &Arg : F->args()) {  // 为形参命名，不是必须的，但是这样可以提高IR的可读性
        Arg.setName(llvm::StringRef(Args[Idx++]));
    }
    return F;
}

llvm::Function *FunctionAST::codegen() {    // 函数定义
    llvm::Function *TheFunction = TheModule->getFunction(Proto->getName());  // 从全局Module中查找函数名，检查函数是否已经被声明
    if (!TheFunction) {     // 检查函数是否未被声明过
        TheFunction = Proto->codegen();  // 如果函数未声明过，调用PrototypeAST的codegen()创建函数原型
        if (!TheFunction) { // 检查函数是否创建失败
            return nullptr;
        }
        // std::cout << "gen function: " << TheFunction->getName().data() << std::endl;
    }
    
    if (!TheFunction->empty()) {    // 检查函数是否已经被定义
        return (llvm::Function *)LogErrorV("Function cannot be redefined");
    }

    // 目前还未加入控制流，所以函数体只有一个基本块
    llvm::BasicBlock *BB = llvm::BasicBlock::Create(*TheContext, llvm::StringRef("entry"), TheFunction);  // 创建一个基本块，名字为entry，所属函数为TheFunction
    Builder->SetInsertPoint(BB); // 设置插入点，即下面的IR指令都会插入到BasicBlock中

    NamedValues.clear();    // 清空NamedValues，用于存放函数内的局部变量
    for (auto &Arg : TheFunction->args()) {  // 为函数的形参创建局部变量
        // std::cout << "find: " << Arg.getName().data() << std::endl;
        NamedValues[std::string(Arg.getName().data())] = &Arg;
    }

    if (llvm::Value *RetVal = Body->codegen()) {  // 递归调用codegen()，生成函数体的IR
        Builder->CreateRet(RetVal);  // 创建返回指令
        llvm::verifyFunction(*TheFunction);  // 验证函数的正确性
        return TheFunction;
    } else {
        TheFunction->eraseFromParent(); // 如果函数体生成IR失败，从Module中移除函数
        return nullptr;
    }
}