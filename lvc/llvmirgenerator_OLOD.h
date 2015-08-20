//
//  llvmirgenerator.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/15.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <cassert>
#include <vector>
#include "llvmincludes.h"
#include "llvmirgenconfig.h"
#include "llvmirgentypevisitor.h"
#include "binopcode.h"
#include "ast.h"
#include "casting.h"
#include "sminf.h"

//
// This is strange, I know.
// Xcode autocomplete doesn't work when we use templates,
// so I undefine ENABLE_OPT_CODE when I need it,
// but you should note that lvc doesn't actually compile when
// it's not defined.
//
#define ENABLE_OPT_CODE
#ifdef ENABLE_OPT_CODE
#define OPT_CODE(code) code
#else
#define OPT_CODE(code)
#endif

enum PreserveNames {
    DoPreserveNames,
    DontPreserveNames,
};

OPT_CODE(template <PreserveNames preserveNames>)
class LLVMIRGenerator {
private:
    LLVMIRGenConfig config;
    llvm::IRBuilder<OPT_CODE(preserveNames == DoPreserveNames)> builder;
    llvm::Module* targetModule() { return config.targetModule; }
    llvm::LLVMContext& context() { return config.context(); }
    llvm::Function* currentFunction() { return builder.GetInsertBlock()->getParent(); }
    llvm::BasicBlock* currentBlock() { return builder.GetInsertBlock(); }
    
    LLVMIRGenTypeVisitor typeVisitor;
    
    llvm::Value* genGlobalDeclInternal(llvm::Type* llvmType, bool isConstant, llvm::Value* init) {
#warning TODO: isa
   //     return new llvm::GlobalVariable(*targetModule(), llvmType, isConstant,
  //                                      llvm::GlobalValue::PrivateLinkage, init);
        return nullptr;
    }
    bool isBlockTerminated(llvm::BasicBlock* block) {
        return block->getTerminator() != nullptr;
    }
public:
    struct LLVMWithoutElseIfRep {
        friend class LLVMIRGenerator<preserveNames>;
    protected:
        LLVMWithoutElseIfRep() {}
        llvm::BasicBlock* thenB;
        llvm::BasicBlock* mergeB;
    };
    struct LLVMWithElseIfRep {
        friend class LLVMIRGenerator<preserveNames>;
    protected:
        LLVMWithElseIfRep() {}
        llvm::BasicBlock* thenB;
        bool thenBlockHasNonBrMergeTerminator; // only filled after then block is completed
        llvm::BasicBlock* elseB;
        llvm::BasicBlock* mergeB;
    };
    
    typedef llvm::AllocaInst* VariableDeclAnnotations;
    typedef llvm::Function* FunctionDeclAnnotations;
    typedef llvm::Value* Temporary;
    typedef LLVMWithoutElseIfRep WithoutElseIfRep;
    typedef LLVMWithElseIfRep WithElseIfRep;
    typedef LLVMIRGenConfig Config;
    
    LLVMIRGenerator(LLVMIRGenConfig config) : config(config), builder(context()), typeVisitor(&this->config) {}
    llvm::Value* genGlobalDecl(const )
    
   // llvm::Value* genGlobalDecl(const ast::VariableDecl& variableDecl, llvm::Value* init) {
   //     llvm::Type* llvmType = typeVisitor.getLLVMType(*variableDecl.type);
    //    return genGlobalDeclInternal(llvmType, variableDecl.type->isConstant(), init);
   // }
   // llvm::Value* genGlobalDecl(const ast::VariableDecl& variableDecl) {
    //    llvm::Type* llvmType = typeVisitor.getLLVMType(*variableDecl.type);
     //   return genGlobalDeclInternal(llvmType, variableDecl.type->isConstant(),
      //                               llvm::Constant::getNullValue(llvmType));
    //}
    bool isCurrentBlockTerminated() {
        return isBlockTerminated(currentBlock());
    }
    void beginFunctionDefinition(ast::FunctionDecl& functionDecl) {
        llvm::Function* f = targetModule()->getFunction(functionDecl.identifier);
        llvm::BasicBlock* bb = llvm::BasicBlock::Create(context(), "entry", f);
        builder.SetInsertPoint(bb);
    }
    void endFunctionDefinition() {
        assert(isCurrentBlockTerminated());
    }
    llvm::Value* genIntLiteral(ast::IntegerType& intTy, const std::string& val) {
        return llvm::ConstantInt::get(typeVisitor.getLLVMIntegerType(intTy), val, 10);
    }
    llvm::Value* genFloatingPointLiteral(ast::IType& type, const std::string& val) {
        return llvm::ConstantFP::get(typeVisitor.getLLVMType(type), val);
    }
    llvm::Function* genFunctionDecl(ast::FunctionDecl& functionDecl) {
        #warning TODO: default args
        std::vector<llvm::Type*> llvmArgs;
        for (auto& argDecl : functionDecl.arguments) {
            llvmArgs.push_back(typeVisitor.getLLVMType(*argDecl.variableDecl.type));
        }
        llvm::Type* llvmReturnType = typeVisitor.getLLVMType(*functionDecl.returnType);
        llvm::FunctionType* llvmFTy = llvm::FunctionType::get(llvmReturnType, llvmArgs, /*no varargs*/ false);
        return llvm::Function::Create(llvmFTy, llvm::GlobalValue::ExternalLinkage, functionDecl.identifier, targetModule());
    }
    llvm::Constant* genFloatingPointLiteral(ast::FloatingPointType& type, const std::string& value) {
        llvm::Type* llvmType = typeVisitor.getLLVMType(type);
        #warning TODO: check semantics of out of bounds literals. (should ideally generate a warning, but that shouldn't be done here).
        return llvm::ConstantFP::get(llvmType, value);
    }
    llvm::Constant* genIntegerLiteral(ast::IntegerType& type, const std::string& value) {
        llvm::IntegerType* llvmType = typeVisitor.getLLVMIntegerType(type);
        return llvm::ConstantInt::get(llvmType, value, 10);
    }
    llvm::Value* genCall(FunctionDeclAnnotations annotations, std::vector<Temporary>& args) {
        return builder.CreateCall(annotations, args);
    }
    llvm::Value* genCast(llvm::Value* val, SMInf& smInf) {
        switch (smInf.implicitCast.kind) {
            case ImplicitCast::Null:
                return val;
            case ImplicitCast::IntegerUpcast:
            {
                ast::IntegerType* iType = dyn_cast<ast::IntegerType*>(sminf.fromType);
                if (iType->isSigned) {
                    return builder.CreateCast(llvm::Instruction::CastOps::SExt /*signed upcast*/, val,
                                              typeVisitor.getLLVMType(*cast.getTypePtr()));
                }
                else {
                    return builder.CreateCast(llvm::Instruction::CastOps::ZExt /*unsigned upcast*/, val,
                                              typeVisitor.getLLVMType(*cast.getTypePtr()));
                }
            }
        }
    }
    llvm::Value* genBinopExp(ast::IType& operandType, llvm::Value* lhs, BinopCode code, llvm::Value* rhs) {
        ast::IType* operandTypePtr = &operandType;
        // Note:
        // LLVM uses greater than/less than,
        // and we use larger than/smaller than.
        if (ast::IntegerType* intTyPtr = dynamic_cast<ast::IntegerType*>(operandTypePtr)) {
            switch (code) {
                case START_CONST_OPERAND_TYPE_BINOPS: assert(false);
                case BinopCodeAdd:
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::Add, lhs, rhs);
                case BinopCodeSubtract:
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::Sub, lhs, rhs);
                case BinopCodeMultiply:
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::Mul, lhs, rhs);
                case BinopCodeDivide:
                    if (intTyPtr->isSigned)
                        return builder.CreateBinOp(llvm::Instruction::BinaryOps::SDiv, lhs, rhs);
                    else
                        return builder.CreateBinOp(llvm::Instruction::BinaryOps::UDiv, lhs, rhs);
                    break;
                case BinopCodeModulo:
                    if (intTyPtr->isSigned)
                        return builder.CreateBinOp(llvm::Instruction::BinaryOps::SRem, lhs, rhs);
                    else
                        return builder.CreateBinOp(llvm::Instruction::BinaryOps::URem, lhs, rhs);
                case END_CONST_OPERAND_TYPE_BINOPS: assert(false);
                case START_CONST_BOOL_BINOPS: assert(false);
                case BinopCodeEquals:
                    return builder.CreateICmpEQ(lhs, rhs);
                case BinopCodeNotEquals:
                    return builder.CreateICmpNE(lhs, rhs);
                case BinopCodeLargerThan:
                    if (intTyPtr->isSigned)
                        return builder.CreateICmpSGT(lhs, rhs);
                    else
                        return builder.CreateICmpUGT(lhs, rhs);
                case BinopCodeSmallerThan:
                    if (intTyPtr->isSigned)
                        return builder.CreateICmpSLT(lhs, rhs);
                    else
                        return builder.CreateICmpULT(lhs, rhs);
                case BinopCodeLargerThanEquals:
                    if (intTyPtr->isSigned)
                        return builder.CreateICmpSGE(lhs, rhs);
                    else
                        return builder.CreateICmpUGE(lhs, rhs);
                case BinopCodeSmallerThanEquals:
                    if (intTyPtr->isSigned)
                        return builder.CreateICmpSLE(lhs, rhs);
                    else
                        return builder.CreateICmpULE(lhs, rhs);
                case END_CONST_BOOL_BINOPS: assert(false);
            }
        }
        else if (dynamic_cast<ast::FloatingPointType*>(operandTypePtr) != nullptr) {
            switch (code) {
                case START_CONST_OPERAND_TYPE_BINOPS: assert(false);
                case BinopCodeAdd:
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::FAdd, lhs, rhs);
                case BinopCodeSubtract:
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::FSub, lhs, rhs);
                case BinopCodeMultiply:
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::FMul, lhs, rhs);
                case BinopCodeDivide:
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::FDiv, lhs, rhs);
                case BinopCodeModulo:
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::FRem, lhs, rhs);
                case END_CONST_OPERAND_TYPE_BINOPS: assert(false);
                case START_CONST_BOOL_BINOPS: assert(false);
                case BinopCodeEquals:
                    return builder.CreateFCmpOEQ(lhs, rhs);
                case BinopCodeNotEquals:
                    return builder.CreateFCmpONE(lhs, rhs);
                case BinopCodeLargerThan:
                    return builder.CreateFCmpOGT(lhs, rhs);
                case BinopCodeSmallerThan:
                    return builder.CreateFCmpOLT(lhs, rhs);
                case BinopCodeLargerThanEquals:
                    return builder.CreateFCmpOGE(lhs, rhs);
                case BinopCodeSmallerThanEquals:
                    return builder.CreateFCmpOLE(lhs, rhs);
                case END_CONST_BOOL_BINOPS: assert(false);
            }
        }
        assert(false);
    }
    llvm::AllocaInst* genLocalWithoutInit(ast::VariableDecl& decl) {
        return builder.CreateAlloca(typeVisitor.getLLVMType(*decl.type));
    }
    llvm::AllocaInst* genLocalWithInit(ast::VariableDecl& decl, llvm::Value* val) {
        llvm::AllocaInst* allocaInst = genLocalWithoutInit(decl);
        builder.CreateStore(val, allocaInst);
        return allocaInst;
    }
    llvm::Value* genLocalEval(llvm::AllocaInst* allocaInst) {
        return builder.CreateLoad(allocaInst);
    }
    void genReturn(llvm::Value* val) {
        builder.CreateRet(val);
    }
    void genVoidReturn() {
        builder.CreateRetVoid();
    }
    LLVMWithoutElseIfRep startWithoutElseIf(llvm::Value* cond) {
        LLVMWithoutElseIfRep rep;
        rep.thenB = llvm::BasicBlock::Create(context(), "then", currentFunction());
        rep.mergeB = llvm::BasicBlock::Create(context(), "merge");
        // Even though we added the then block, our insertion point hasn't actually changed
        builder.CreateCondBr(cond, rep.thenB, rep.mergeB);
        builder.SetInsertPoint(rep.thenB);
        return rep;
    }
    void endWithoutElseIf(LLVMWithoutElseIfRep& rep) {
        if (!isCurrentBlockTerminated()) {
            builder.CreateBr(rep.mergeB);
        }
        // Insert mergeB into the function
        currentFunction()->getBasicBlockList().push_back(rep.mergeB);
        builder.SetInsertPoint(rep.mergeB);
    }
    LLVMWithElseIfRep startWithElseIf(llvm::Value* cond) {
        LLVMWithElseIfRep rep;
        rep.thenB = llvm::BasicBlock::Create(context(), "then", currentFunction());
        rep.elseB = llvm::BasicBlock::Create(context(), "else");
        rep.mergeB = llvm::BasicBlock::Create(context(), "merge");
        // Insertion point hasn't changed
        builder.CreateCondBr(cond, rep.thenB, rep.elseB);
        builder.SetInsertPoint(rep.thenB);
        return rep;
    }
    void endThenStartElse(LLVMWithElseIfRep& rep) {
        if (!isCurrentBlockTerminated()) {
            rep.thenBlockHasNonBrMergeTerminator = false;
            builder.CreateBr(rep.mergeB);
        }
        else {
            rep.thenBlockHasNonBrMergeTerminator = true;
        }
        // Insert else block
        currentFunction()->getBasicBlockList().push_back(rep.elseB);
        builder.SetInsertPoint(rep.elseB);
    }
    void endWithElseIf(LLVMWithElseIfRep& rep) {
        if (!isCurrentBlockTerminated()) {
            builder.CreateBr(rep.mergeB);
        }
        else {
            if (rep.thenBlockHasNonBrMergeTerminator) {
                // Both the then block and the else block already
                // have terminators. This means we don't have to create
                // the merge block, and in fact we shouldn't, because
                // we have cases like:
                //
                // if (cond)
                //     return 3
                // else
                //     return 4
                //
                // Where if we created a merge block, there'd be no terminator for
                // it and the IR wouldn't be valid.
                return;
            }
        }
        // Insert merge block
        currentFunction()->getBasicBlockList().push_back(rep.mergeB);
        builder.SetInsertPoint(rep.mergeB);
    }
};
