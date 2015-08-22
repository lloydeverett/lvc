//
//  semanticanalyzer.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/25.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <memory>
#include <stdexcept>
#include <boost/optional.hpp>
#include "symboltable.h"
#include "ast.h"
#include "casting.h"
#include "semanticanalysisconfig.h"
#include "sminf.h"
#include "inodevisitor.h"
#include "semanticanalyzerexceptions.h"

#warning TODO: casts w/o sminf
#warning TODO: default args passing

class ISemanticAnalyzer {
public:
    virtual void addLookaheadGlobalDecl(ast::VariableDecl& decl) = 0;
    virtual void addLookaheadFunctionDecl(ast::FunctionDecl& decl) = 0;
    virtual void analyzeCallExpStmt(ast::CallExpStmt& stmt) = 0;
    virtual void analyzeReturnStmt(ast::ReturnStmt& stmt) = 0;
    virtual void analyzeVariableDeclStmt(ast::VariableDeclStmt& stmt) = 0;
    virtual void analyzeIfStmt(ast::IfStmt& stmt) = 0;
    virtual void analyzeBlockStmt(ast::BlockStmt& block) = 0;
    virtual void analyzeIncrementStmt(ast::IncrementStmt& stmt) = 0;
    virtual void analyzeDecrementStmt(ast::DecrementStmt& stmt) = 0;
    virtual void analyzeVarBinopStmt(ast::VarBinopStmt& stmt) = 0;
    virtual void analyzeAssignStmt(ast::AssignStmt& stmt) = 0;
    virtual void analyzeBinopExp(ast::BinopExp& exp, boost::optional<ast::IType&> optDesiredType) = 0;
    virtual void analyzeCallExp(ast::CallExp& exp, boost::optional<ast::IType&> optDesiredType) = 0;
    virtual void analyzeNumberLiteralExp(ast::NumberLiteralExp& exp, boost::optional<ast::IType&> optDesiredType) = 0;
    virtual void analyzeVariableExp(ast::VariableExp& exp, boost::optional<ast::IType&> optDesiredType) = 0;
    virtual void analyzeFunctionDefinition(ast::Function& function) = 0;
};

class SemanticAnalyzer;

class SemanticAnalyzerVisitor : public INodeVisitor {
    friend class SemanticAnalyzer;
private:
    boost::optional<ast::IType&> optDesiredType;
    ISemanticAnalyzer& parent;
protected:
    SemanticAnalyzerVisitor(ISemanticAnalyzer& parent) : parent(parent) {}
public:
    virtual void visit(ast::CallExpStmt& callExpStmt) {
        parent.analyzeCallExpStmt(callExpStmt);
    }
    virtual void visit(ast::ReturnStmt& returnStmt) {
        parent.analyzeReturnStmt(returnStmt);
    }
    virtual void visit(ast::VariableDeclStmt& variableDeclStmt) {
        parent.analyzeVariableDeclStmt(variableDeclStmt);
    }
    virtual void visit(ast::IfStmt& ifStmt) {
        parent.analyzeIfStmt(ifStmt);
    }
    virtual void visit(ast::BlockStmt& blockStmt) {
        parent.analyzeBlockStmt(blockStmt);
    }
    virtual void visit(ast::IncrementStmt& incrementStmt) {
        parent.analyzeIncrementStmt(incrementStmt);
    }
    virtual void visit(ast::DecrementStmt& decrementStmt) {
        parent.analyzeDecrementStmt(decrementStmt);
    }
    virtual void visit(ast::VarBinopStmt& VarBinopStmt) {
        parent.analyzeVarBinopStmt(VarBinopStmt);
    }
    virtual void visit(ast::AssignStmt& assignStmt) {
        parent.analyzeAssignStmt(assignStmt);
    }
    virtual void visit(ast::BinopExp& binopExp) {
        parent.analyzeBinopExp(binopExp, optDesiredType);
    }
    virtual void visit(ast::CallExp& callExp) {
        parent.analyzeCallExp(callExp, optDesiredType);
    }
    virtual void visit(ast::NumberLiteralExp& numberLiteralExp) {
        parent.analyzeNumberLiteralExp(numberLiteralExp, optDesiredType);
    }
    virtual void visit(ast::VariableExp& variableExp) {
        parent.analyzeVariableExp(variableExp, optDesiredType);
    }
    void doStmtVisit(ast::IStmt& n) {
        n.accept(*this);
    }
    void doExpVisit(ast::IExp& n, boost::optional<ast::IType&> optDesiredType) {
        this->optDesiredType = optDesiredType;
        n.accept(*this);
    }
};

class SemanticAnalyzer : public ISemanticAnalyzer {
private:
    SemanticAnalyzerVisitor visitor;
    SemanticAnalysisConfig config;
    SymbolTable symbolTable;
    ast::IntegerType defaultInt;
    ast::FloatingPointType defaultFloatingPoint;
    ast::BoolType conditionType;
    ast::Function* currentFunction;
    #warning TODO: refactor recreated types.
public:
    SemanticAnalyzer(SemanticAnalysisConfig config) :
    visitor(*this),
    config(config),
    defaultInt(config.defaultIntBitWidth,
               config.defaultIntIsSigned ? ast::IntegerType::Signed : ast::IntegerType::Unsigned,
               Constant),
    defaultFloatingPoint(config.defaultFloatingPointIsDouble ? ast::FloatingPointType::VariationDouble :
                         ast::FloatingPointType::VariationFloat, Constant),
    conditionType(Constant),
    currentFunction(nullptr) {}
    
    ///////////////////////////////////////
    
    virtual void addLookaheadGlobalDecl(ast::VariableDecl& decl) override {
        symbolTable.registerVariable(&decl);
    }
    virtual void addLookaheadFunctionDecl(ast::FunctionDecl& decl) override {
        symbolTable.registerFunction(&decl);
    }
    
    ///////////////////////////////////////
    
    virtual void analyzeCallExpStmt(ast::CallExpStmt& stmt) override {
        analyzeCallExp(stmt.callExp, boost::none);
    }
    virtual void analyzeReturnStmt(ast::ReturnStmt& stmt) override {
        assert(currentFunction != nullptr);
        if (stmt.optReturnedExpression) {
            visitor.doExpVisit(**stmt.optReturnedExpression, *currentFunction->decl.returnType);
        }
        else {
            if (!isa<ast::VoidType>(*currentFunction->decl.returnType))
                throw SemanticAnalyzerExceptionCouldNotMatchTypes();
            // We have nothing more to do
        }
    }
    virtual void analyzeVariableDeclStmt(ast::VariableDeclStmt& stmt) override {
        if (isa<ast::VoidType>(*stmt.decl.type))
            throw SemanticAnalyzerExceptionVariableOfVoidType();
        symbolTable.registerVariable(&stmt.decl);
        if (stmt.optInit) {
            visitor.doExpVisit(**stmt.optInit, *stmt.decl.type);
        }
    }
    virtual void analyzeIfStmt(ast::IfStmt& stmt) override {
        visitor.doExpVisit(*stmt.condition, conditionType);
        analyzeBlockStmt(stmt.thenBlock);
        if (stmt.optElseBlock) {
            analyzeBlockStmt(*stmt.optElseBlock);
        }
    }
    virtual void analyzeBlockStmt(ast::BlockStmt& stmt) override {
        symbolTable.openVariableScope();
        for (std::unique_ptr<ast::IStmt>& s : stmt.statements) {
            visitor.doStmtVisit(*s);
        }
        symbolTable.closeVariableScope();
    }
private:
    void checkThatAnalyzedLvalueCanBeModified(ast::VariableExp& exp) {
        if (exp.sminf.to->getConstantness() != NotConstant || exp.sminfLookup->isArg()) {
            throw SemanticAnalyzerExceptionLvalueCannotBeModified();
        }
    }
public:
    virtual void analyzeIncrementStmt(ast::IncrementStmt& stmt) {
        analyzeVariableExp(*stmt.exp, boost::none);
        if (!isOperandTypeValidForIncrement(*stmt.exp->sminf.to)) {
            throw SemanticAnalyzerExceptionInvalidOperandTypeForVariableModification();
        }
        checkThatAnalyzedLvalueCanBeModified(*stmt.exp);
    }
    virtual void analyzeDecrementStmt(ast::DecrementStmt& stmt) {
        analyzeVariableExp(*stmt.exp, boost::none);
        if (!isOperandTypeValidForDecrement(*stmt.exp->sminf.to)) {
            throw SemanticAnalyzerExceptionInvalidOperandTypeForVariableModification();
        }
        checkThatAnalyzedLvalueCanBeModified(*stmt.exp);
    }
    virtual void analyzeVarBinopStmt(ast::VarBinopStmt& stmt) {
        analyzeVariableExp(*stmt.lvalue, boost::none);
        if (!isOperandTypeValidForOperandBinopCode(stmt.code, *stmt.lvalue->sminf.to)) {
            throw SemanticAnalyzerExceptionInvalidOperandTypeForVariableModification();
        }
        checkThatAnalyzedLvalueCanBeModified(*stmt.lvalue);
        visitor.doExpVisit(*stmt.rvalue, *stmt.lvalue->sminf.to);
    }
    virtual void analyzeAssignStmt(ast::AssignStmt& stmt) {
        assert(stmt.lvalues.size() > 0);
        analyzeVariableExp(*stmt.lvalues[0], boost::none);
        checkThatAnalyzedLvalueCanBeModified(*stmt.lvalues[0]);
        for (int i = 1; i < stmt.lvalues.size(); i++) {
            analyzeVariableExp(*stmt.lvalues[i], *stmt.lvalues[0]->sminf.to);
            checkThatAnalyzedLvalueCanBeModified(*stmt.lvalues[i]);
        }
        visitor.doExpVisit(*stmt.rvalue, *stmt.lvalues[0]->sminf.to);
    }
    
    ///////////////////////////////////////
    
    virtual void analyzeBinopExp(ast::BinopExp& exp, boost::optional<ast::IType&> optDesiredType) override {
        visitor.doExpVisit(*exp.lhs, boost::none);
        // Match rhs to the type of lhs
        visitor.doExpVisit(*exp.rhs, *exp.lhs->sminf.to);
        
        if (!isOperandTypeValidForBinopCode(exp.code, *exp.lhs->sminf.to)) {
            throw SemanticAnalyzerExceptionInvalidOperandTypesForBinop();
        }
        
        if (doesBinopEvaluateToBoolean(exp.code)) {
            exp.sminf = matchTypes(&conditionType, optDesiredType);
        }
        else if (doesBinopEvaluateToOperandType(exp.code)) {
            exp.sminf = matchTypes(exp.lhs->sminf.to, optDesiredType);
        }
        else assert(false);
    }
    virtual void analyzeCallExp(ast::CallExp& exp, boost::optional<ast::IType&> optDesiredType) {
        ast::FunctionDecl* decl = symbolTable.lookupFunction(exp.calleeIdentifier);
        
        if (decl->arguments.size() != exp.passedArguments.size()) {
            throw SemanticAnalyzerExceptionIncorrectArguments();
        }
        
        auto nArgs = exp.passedArguments.size();
        
        for (decltype(nArgs) i = 0; i < nArgs; i++) {
            std::unique_ptr<ast::IExp>& pa = exp.passedArguments[i];
            ast::ArgVariableDecl& d = decl->arguments[i];
            visitor.doExpVisit(*pa, *d.type);
        }
        
        exp.sminf = matchTypes(decl->returnType.get(), optDesiredType);
        exp.sminfLookup = decl;
    }
    virtual void analyzeNumberLiteralExp(ast::NumberLiteralExp& exp, boost::optional<ast::IType&> optDesiredType) {
        // If the literal has a decimal point, we acknoledge that it can't
        // be an integer. If it has a decimal point and our target type is an integer, then we must fail.
        bool defNotInt = exp.hasDecimalPoint;
        
        if (optDesiredType) {
            ast::IType& desiredType = *optDesiredType;
            
            // If there's a desired type, we match it.
            if (!defNotInt) {
                if (isa<ast::IntegerType>(desiredType)) {
                    #warning TODO: analyze that the exp can fit into the bit width
                    exp.sminf = SMInf::SMInfWithNoCast(&desiredType);
                    return;
                }
            }
            if (isa<ast::FloatingPointType>(*optDesiredType)) {
                #warning TODO: analyze that the exp can fit into the variation
                exp.sminf = SMInf::SMInfWithNoCast(&desiredType);
                return;
            }
            
            throw SemanticAnalyzerExceptionCouldNotMatchTypes();
        }
        else {
            if (!defNotInt) {
                // If there's no desired type and no decimal point, then we assume it's an integer
                // of the bit width & signedness specified in the config.
                exp.sminf = SMInf::SMInfWithNoCast(&defaultInt);
            }
            else {
                // Or, if there's no desired type and there is a decimal point, then we assume it's a floating
                // point type of the variaiton specified in the config.
                exp.sminf = SMInf::SMInfWithNoCast(&defaultFloatingPoint);
            }
        }
    }
    virtual void analyzeVariableExp(ast::VariableExp& exp, boost::optional<ast::IType&> optDesiredType) override {
        // Lookup the variable
        ast::VariableDecl* decl = symbolTable.lookupVariable(exp.identifier);
        
        exp.sminf = matchTypes(decl->type.get(), optDesiredType);
        exp.sminfLookup = decl;
    }
    
    ///////////////////////////////////////
    
    virtual void analyzeFunctionDefinition(ast::Function& function) override {
        currentFunction = &function;
        symbolTable.openVariableScope();
        for (ast::ArgVariableDecl& decl : function.decl.arguments) {
            symbolTable.registerVariable(&decl);
            if (isa<ast::VoidType>(*decl.type)) {
                throw SemanticAnalyzerExceptionVariableOfVoidType();
            }
            if (decl.optDefault) {
                visitor.doExpVisit(**decl.optDefault, *decl.type);
            }
        }
        
        visitor.doStmtVisit(function.block);
        symbolTable.closeVariableScope();
        currentFunction = nullptr;
    }
    
private:
    // When we 'match' types we ignore constness: in most cases
    // it does not matter.
    // When constness does matter (e.g. assigning a variable), we
    // explictly check it.
    SMInf matchTypes(ast::IType* a, boost::optional<ast::IType&> optB) {
        return !optB ? SMInf::SMInfWithNoCast(a) : matchTypes(a, &(*optB));
    }
    SMInf matchTypes(ast::IType* a, ast::IType* b) {
        if (doTypesMatchWithoutCast(a, b))
            return SMInf::SMInfWithNoCast(a);
        if (config.allowImplicitIntUpcast) {
            if (ast::IntegerType* ai = dyn_cast<ast::IntegerType>(a)) {
                if (ast::IntegerType* bi = dyn_cast<ast::IntegerType>(b)) {
                    if (bi->numBits > ai->numBits && ai->isSigned() == bi->isSigned()) {
                        return SMInf::SMInfWithIntegerUpcast(a, b);
                    }
                }
            }
        }
        throw SemanticAnalyzerExceptionCouldNotMatchTypes();
    }
    bool doTypesMatchWithoutCast(ast::IType* a, ast::IType* b) {
        if (a->getKind() != b->getKind()) {
            return false;
        }
        switch (a->getKind()) {
            case ast::IType::TK_VoidType:
                return true;
            case ast::IType::TK_IntegerType:
            {
                ast::IntegerType* ai = cast<ast::IntegerType>(a);
                ast::IntegerType* bi = cast<ast::IntegerType>(b);
                return ai->numBits == bi->numBits && ai->isSigned() == bi->isSigned();
            }
            case ast::IType::TK_BoolType:
                return true;
            case ast::IType::TK_FloatingPointType:
            {
                ast::FloatingPointType* af = cast<ast::FloatingPointType>(a);
                ast::FloatingPointType* bf = cast<ast::FloatingPointType>(b);
                return af->variation == bf->variation;
            }
        }
    }
    bool isOperandTypeValidForBinopCode(BinopCode code, ast::IType& type) {
        if (isa<ast::VoidType>(type))
            return false;
        if (type.isNumeralType())
            return true;
        if (code == BinopCodeEquals || code == BinopCodeNotEquals)
            return true;
        return false;
    }
    bool isOperandTypeValidForOperandBinopCode(BinopCode code, ast::IType& type) {
        assert(code > START_OPERAND_TYPE_BINOPS && code < END_OPERAND_TYPE_BINOPS);
        if (type.isNumeralType())
            return true;
        return false;
    }
    bool isOperandTypeValidForIncrement(ast::IType& type) {
        return type.isNumeralType();
    }
    bool isOperandTypeValidForDecrement(ast::IType& type) {
        return type.isNumeralType();
    }
};
