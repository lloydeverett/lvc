//
//  llvmirgenerator.h
//  lvc
//
//  Created by Lloyd Everett on 2015/08/10.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <stdexcept>
#include "llvmirgenconfig.h"
#include "llvmincludes.h"
#include "casting.h"
#include "inodevisitor.h"

#warning TOOD: casts.

class ILLVMIRGenerator {
public:
    virtual void genLookaheadGlobal(ast::VariableDecl& decl) = 0;
    virtual void genLookaheadFunctionDecl(ast::FunctionDecl& decl) = 0;
    virtual void genCallExpStmt(ast::CallExpStmt& stmt) = 0;
    virtual void genReturnStmt(ast::ReturnStmt& stmt) = 0;
    virtual void genVariableDeclStmt(ast::VariableDeclStmt& stmt) = 0;
    virtual void genIfStmt(ast::IfStmt& stmt) = 0;
    virtual void genBlockStmt(ast::BlockStmt& block) = 0;
    virtual void genBinopExp(ast::BinopExp& binopExp) = 0;
    virtual void genCallExp(ast::CallExp& callExp) = 0;
    virtual void genNumberLiteralExp(ast::NumberLiteralExp& numExp) = 0;
    virtual void genVariableExp(ast::VariableExp& varExp) = 0;
    virtual void genFunctionDefinition(ast::Function& function) = 0;
};

class LLVMIRGenerator;

class LLVMIRGeneratorVisitor : public INodeVisitor {
    friend class LLVMIRGenerator;
private:
    ILLVMIRGenerator& parent;
protected:
    LLVMIRGeneratorVisitor(ILLVMIRGenerator& parent) : parent(parent) {}
public:
    virtual void visit(ast::CallExpStmt& callExpStmt) {
        parent.genCallExpStmt(callExpStmt);
    }
    virtual void visit(ast::ReturnStmt& returnStmt) {
        parent.genReturnStmt(returnStmt);
    }
    virtual void visit(ast::VariableDeclStmt& variableDeclStmt) {
        parent.genVariableDeclStmt(variableDeclStmt);
    }
    virtual void visit(ast::IfStmt& ifStmt) {
        parent.genIfStmt(ifStmt);
    }
    virtual void visit(ast::BlockStmt& blockStmt) {
        parent.genBlockStmt(blockStmt);
    }
    virtual void visit(ast::BinopExp& binopExp) {
        parent.genBinopExp(binopExp);
    }
    virtual void visit(ast::CallExp& callExp) {
        parent.genCallExp(callExp);
    }
    virtual void visit(ast::NumberLiteralExp& numberLiteralExp) {
        parent.genNumberLiteralExp(numberLiteralExp);
    }
    virtual void visit(ast::VariableExp& variableExp) {
        parent.genVariableExp(variableExp);
    }
    void doExpVisit(ast::IExp& exp) {
        exp.accept(*this);
    }
    void doStmtVisit(ast::IStmt& stmt) {
        stmt.accept(*this);
    }
};

class LLVMIRGenerator : public ILLVMIRGenerator {
private:
    LLVMIRGenConfig config;
    llvm::Module* targetModule() { return config.targetModule; }
    llvm::LLVMContext& context() { return config.context(); }
    llvm::Function* currentLLVMFunction() { return builder.GetInsertBlock()->getParent(); }
    llvm::IRBuilder<> builder;
    LLVMIRGeneratorVisitor visitor;
    
    llvm::Type* getLLVMType(ast::IType& t) {
        switch (t.getKind()) {
            case ast::IType::TK_VoidType:
                return builder.getVoidTy();
            case ast::IType::TK_IntegerType:
            {
                ast::IntegerType& it = cast<ast::IntegerType>(t);
                return getIntLLVMType(it);
            }
            case ast::IType::TK_BoolType:
                return builder.getInt1Ty();
            case ast::IType::TK_FloatingPointType:
            {
                bool isDouble = cast<ast::FloatingPointType>(t).variation ==
                                ast::FloatingPointType::VariationDouble;
                return isDouble ? builder.getDoubleTy() : builder.getFloatTy();
            }
        }
    }
    llvm::IntegerType* getIntLLVMType(ast::IntegerType& t) {
        int numBits = cast<ast::IntegerType>(t).numBits;
        return builder.getIntNTy(numBits);
    }
    llvm::Value* createIntBinop(BinopCode code, llvm::Value* lhs, llvm::Value* rhs, bool isSigned) {
        switch (code) {
            case START_OPERAND_TYPE_BINOPS: assert(false);
            case BinopCodeAdd:
                return builder.CreateBinOp(llvm::Instruction::BinaryOps::Add, lhs, rhs);
            case BinopCodeSubtract:
                return builder.CreateBinOp(llvm::Instruction::BinaryOps::Sub, lhs, rhs);
            case BinopCodeMultiply:
                return builder.CreateBinOp(llvm::Instruction::BinaryOps::Mul, lhs, rhs);
            case BinopCodeDivide:
                if (isSigned)
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::SDiv, lhs, rhs);
                else
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::UDiv, lhs, rhs);
                break;
            case BinopCodeModulo:
                if (isSigned)
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::SRem, lhs, rhs);
                else
                    return builder.CreateBinOp(llvm::Instruction::BinaryOps::URem, lhs, rhs);
            case END_OPERAND_TYPE_BINOPS: assert(false);
            case START_BOOL_BINOPS: assert(false);
            case BinopCodeEquals:
                return builder.CreateICmpEQ(lhs, rhs);
            case BinopCodeNotEquals:
                return builder.CreateICmpNE(lhs, rhs);
            case BinopCodeLargerThan:
                if (isSigned)
                    return builder.CreateICmpSGT(lhs, rhs);
                else
                    return builder.CreateICmpUGT(lhs, rhs);
            case BinopCodeSmallerThan:
                if (isSigned)
                    return builder.CreateICmpSLT(lhs, rhs);
                else
                    return builder.CreateICmpULT(lhs, rhs);
            case BinopCodeLargerThanEquals:
                if (isSigned)
                    return builder.CreateICmpSGE(lhs, rhs);
                else
                    return builder.CreateICmpUGE(lhs, rhs);
            case BinopCodeSmallerThanEquals:
                if (isSigned)
                    return builder.CreateICmpSLE(lhs, rhs);
                else
                    return builder.CreateICmpULE(lhs, rhs);
            case END_BOOL_BINOPS: assert(false);
        }
    }
    llvm::Value* createFPBinop(BinopCode code, llvm::Value* lhs, llvm::Value* rhs) {
        switch (code) {
            case START_OPERAND_TYPE_BINOPS: assert(false);
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
            case END_OPERAND_TYPE_BINOPS: assert(false);
            case START_BOOL_BINOPS: assert(false);
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
            case END_BOOL_BINOPS: assert(false);
        }
    }
    
public:
    LLVMIRGenerator(LLVMIRGenConfig config) : config(config), builder(context()), visitor(*this) {}
    
    virtual void genLookaheadGlobal(ast::VariableDecl& decl) override {
        #warning TODO: check usage of new
        llvm::Type* t = getLLVMType(*decl.type);
        bool isConstant = decl.type->getConstant() == Constant;
        decl.genVal = new llvm::GlobalVariable(*targetModule(), t, isConstant,
                                                llvm::GlobalValue::ExternalLinkage,
                                                /*initializer not set here*/ nullptr,
                                                decl.identifier);
    }
    virtual void genLookaheadFunctionDecl(ast::FunctionDecl& decl) override {
        std::vector<llvm::Type*> llvmArgs;
        for (auto& argDecl : decl.arguments) {
            llvmArgs.push_back(getLLVMType(*argDecl.variableDecl.type));
        }
        llvm::Type* llvmReturnType = getLLVMType(*decl.returnType);
        llvm::FunctionType* llvmFTy = llvm::FunctionType::get(llvmReturnType, llvmArgs, /*no varargs*/ false);
        decl.genVal = llvm::Function::Create(llvmFTy, llvm::GlobalValue::ExternalLinkage, decl.identifier, targetModule());
    }
    virtual void genCallExpStmt(ast::CallExpStmt& stmt) override {
        genCallExp(stmt.callExp);
    }
    virtual void genReturnStmt(ast::ReturnStmt& stmt) override {
        if (stmt.optReturnedExpression) {
            std::unique_ptr<ast::IExp>& returnedExpression = *stmt.optReturnedExpression;
            visitor.doExpVisit(*returnedExpression);
            builder.CreateRet(returnedExpression->genVal);
        }
        else {
            builder.CreateRetVoid();
        }
    }
    virtual void genVariableDeclStmt(ast::VariableDeclStmt& stmt) override {
        stmt.decl.genVal = builder.CreateAlloca(getLLVMType(*stmt.decl.type));
        if (stmt.optInit) {
            std::unique_ptr<ast::IExp>& init = *stmt.optInit;
            visitor.doExpVisit(*init);
            builder.CreateStore(init->genVal, stmt.decl.genVal);
        }
    }
    virtual void genIfStmt(ast::IfStmt& stmt) override {
        visitor.doExpVisit(*stmt.condition);
        llvm::Value* condVal = stmt.condition->genVal;
        
        llvm::BasicBlock* thenB = llvm::BasicBlock::Create(context(), "then", currentLLVMFunction());
        
        if (!stmt.optElseBlock) {
            llvm::BasicBlock* mergeB = llvm::BasicBlock::Create(context(), "merge");
            builder.CreateCondBr(condVal, thenB, mergeB);
            builder.SetInsertPoint(thenB);
            genBlockStmt(stmt.thenBlock);
            if (builder.GetInsertBlock()->getTerminator() == nullptr) {
                builder.CreateBr(mergeB);
            }
            currentLLVMFunction()->getBasicBlockList().push_back(mergeB);
            builder.SetInsertPoint(mergeB);
        }
        else {
            ast::BlockStmt& elseBlock = *stmt.optElseBlock;
            llvm::BasicBlock* elseB = llvm::BasicBlock::Create(context(), "else");
            
            llvm::BasicBlock* mergeB = nullptr; // We may never set this; there could be no merge block.
            
            builder.CreateCondBr(condVal, thenB, elseB);
            
            builder.SetInsertPoint(thenB);
            genBlockStmt(stmt.thenBlock);
            if (builder.GetInsertBlock()->getTerminator() == nullptr) {
                mergeB = llvm::BasicBlock::Create(context(), "merge");
                builder.CreateBr(mergeB);
            }
            
            currentLLVMFunction()->getBasicBlockList().push_back(elseB);
            builder.SetInsertPoint(elseB);
            genBlockStmt(elseBlock);
            if (builder.GetInsertBlock()->getTerminator() == nullptr) {
                if (mergeB == nullptr)
                    mergeB = llvm::BasicBlock::Create(context(), "merge");
                builder.CreateBr(mergeB);
            }
            
            if (mergeB != nullptr) {
                currentLLVMFunction()->getBasicBlockList().push_back(mergeB);
                builder.SetInsertPoint(mergeB);
            }
            else {
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
            }
        }
    }
    virtual void genBlockStmt(ast::BlockStmt& block) override {
        for (std::unique_ptr<ast::IStmt>& p : block.statements) {
            visitor.doStmtVisit(*p);
        }
    }
    
    ///////////////////////////////////////
    
    virtual void genBinopExp(ast::BinopExp& binopExp) override {
        visitor.doExpVisit(*binopExp.lhs);
        visitor.doExpVisit(*binopExp.rhs);
        
        switch (binopExp.sminf.from->getKind()) {
            case ast::IType::TK_VoidType:
                assert(false);
            case ast::IType::TK_IntegerType:
            {
                ast::IntegerType& it = cast<ast::IntegerType>(*binopExp.lhs->sminf.from);
                binopExp.genVal = createIntBinop(binopExp.code, binopExp.lhs->genVal, binopExp.rhs->genVal,
                                                 it.isSigned());
                return;
            }
            case ast::IType::TK_BoolType:
                // Bools are just 1-bit ints.
                #warning TODO: check signedness
                binopExp.genVal = createIntBinop(binopExp.code, binopExp.lhs->genVal, binopExp.rhs->genVal,
                                                 /*not signed*/false);
                return;
            case ast::IType::TK_FloatingPointType:
                binopExp.genVal = createFPBinop(binopExp.code, binopExp.lhs->genVal, binopExp.rhs->genVal);
                return;
        }
    }
    virtual void genCallExp(ast::CallExp& callExp) override {
        std::vector<llvm::Value*> llvmArgs;
        for (std::unique_ptr<ast::IExp>& e : callExp.passedArguments) {
            visitor.doExpVisit(*e);
            llvmArgs.push_back(e->genVal);
        }
        
        callExp.genVal = builder.CreateCall(callExp.sminfLookup->genVal, llvmArgs);
    }
    virtual void genNumberLiteralExp(ast::NumberLiteralExp& numExp) {
        switch (numExp.sminf.from->getKind()) {
            case ast::IType::TK_VoidType:
                assert(false);
            case ast::IType::TK_IntegerType:
            {
                ast::IntegerType& it = cast<ast::IntegerType>(*numExp.sminf.from);
                numExp.genVal = llvm::ConstantInt::get(getIntLLVMType(it), numExp.value, 10);
                return;
            }
            case ast::IType::TK_BoolType:
                assert(false);
            case ast::IType::TK_FloatingPointType:
            {
                numExp.genVal = llvm::ConstantFP::get(getLLVMType(*numExp.sminf.from), numExp.value);
                return;
            }
        }
    }
    virtual void genVariableExp(ast::VariableExp& varExp) override {
        varExp.genVal = builder.CreateLoad(varExp.sminfLookup->genVal);
    }
    
    ///////////////////////////////////////
    
    virtual void genFunctionDefinition(ast::Function& function) override {
        llvm::Function* f = targetModule()->getFunction(function.decl.identifier);
        llvm::BasicBlock* entryB = llvm::BasicBlock::Create(context(), "entry", f);
        builder.SetInsertPoint(entryB);
        genBlockStmt(function.block);
        
        // check terminator; auto return from void.
        if (builder.GetInsertBlock()->getTerminator() == nullptr) {
            if (isa<ast::VoidType>(*function.decl.returnType)) {
                builder.CreateRetVoid();
            }
            else {
                assert(false);
            }
        #warning TODO: check in sma
        }
    }
};
