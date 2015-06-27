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
    
    std::vector<Type*> t;
    t.push_back(builder.getInt32Ty());
    ArrayRef<Type*> arref(t.data(), t.size());
    
    FunctionType *otherT = FunctionType::get(builder.getInt32Ty(), arref, false);
    Function *other = Function::Create(otherT, llvm::GlobalValue::LinkageTypes::ExternalLinkage, "", &module);
    BasicBlock *otherb = BasicBlock::Create(getGlobalContext(), "entry", other);
    builder.SetInsertPoint(otherb);
    builder.CreateRet(builder.getInt32(40));
    
    FunctionType *ft = FunctionType::get(Type::getDoubleTy(context), false);
    Function *f = Function::Create(ft, llvm::GlobalValue::LinkageTypes::ExternalLinkage, "", &module);
    BasicBlock *b = BasicBlock::Create(getGlobalContext(), "entry", f);
    builder.SetInsertPoint(b);
    builder.CreateCall(other, builder.getInt32(349));
    Value* v = builder.CreateAdd(Constant::getAllOnesValue(Type::getInt32Ty(context)), Constant::getNullValue(Type::getInt32Ty(context)));
    builder.CreateRet(v);
    
    module.dump();
    return 0;
}
