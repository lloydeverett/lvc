//
//  main.cpp
//  llvmplayground
//
//  Created by Lloyd Everett on 2015/05/31.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <iostream>
#include "llvm/IR/Verifier.h"
#include "llvm/IR/DerivedTypes.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"

using namespace llvm;

int main(int argc, const char * argv[]) {
    LLVMContext &context = getGlobalContext();
    Module module("helloworld", context);
    IRBuilder<> builder(context);
    FunctionType *ft = FunctionType::get(Type::getDoubleTy(context), false);
    Function *f = Function::Create(ft, llvm::GlobalValue::LinkageTypes::ExternalLinkage, "", &module);
    BasicBlock *b = BasicBlock::Create(getGlobalContext(), "entry", f);
    builder.SetInsertPoint(b);
    Value* v = builder.CreateAdd(Constant::getAllOnesValue(Type::getInt32Ty(context)), Constant::getNullValue(Type::getInt32Ty(context)));
    builder.CreateMul(v, ConstantInt::get(Type::getInt32Ty(context), 30));
    builder.CreateRet(v);
    module.dump();
    return 0;
}
