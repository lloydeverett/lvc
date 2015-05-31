//
//  main.cpp
//  testingbuild
//
//  Created by Lloyd Everett on 2015/05/23.
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
    IRBuilder<> builder(context);
    Module module("helloworld", context);
    Type* i32 = Type::getInt32Ty(context);
    FunctionType* fType = FunctionType::get(i32, false);
    Function* function = Function::Create(fType, GlobalValue::LinkageTypes::ExternalLinkage, "main", &module);
    BasicBlock* basicBlock = BasicBlock::Create(context, "entry", function);
    builder.SetInsertPoint(basicBlock);
    AllocaInst* inst = builder.CreateAlloca(i32);
    builder.CreateStore(ConstantInt::get(i32, 0), inst);
    Value* instVal = builder.CreateLoad(inst, "instval");
    builder.CreateRet(instVal);
    
    module.dump();
    return 0;
}
