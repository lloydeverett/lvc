//
//  astcompilerexpvisitor.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <cstdint>
#include <boost/optional.hpp>
#include <stdexcept>
#include <utility>
#include <memory>
#include "inodevisitor.h"
#include "implicitcast.h"
#include "astcompilationconfig.h"
#include "astcompilerexceptions.h"
#include "typechecking.h"

template <typename Generator>
class ASTCompilerStmtVisitor;

template <typename Generator>
class ASTCompilerExpVisitor : public INodeVisitor {
    friend class ASTCompilerStmtVisitor<Generator>;
private:
    typedef typename Generator::VariableDeclAnnotations VariableDeclAnnotations;
    typedef typename Generator::FunctionDeclAnnotations FunctionDeclAnnotations;
    typedef typename Generator::Temporary Temporary;
    ASTCompilationConfig& config;
    Generator& generator;
    SymbolTable<VariableDeclAnnotations, FunctionDeclAnnotations>& symbolTable;
    struct Visit {
        std::unique_ptr<ast::IType> resolvedType;
        Temporary val;
        
        Visit(std::unique_ptr<ast::IType> resolvedType, Temporary val) :
        resolvedType(std::move(resolvedType)), val(val) {}
        Visit(const ast::IType* resolvedTypePtr, Temporary val) :
        resolvedType(resolvedTypePtr->clone()), val(val) {}
        Visit(const ast::IType& resolvedTypeRef, Temporary val) :
        resolvedType(resolvedTypeRef.clone()), val(val) {}
    };
    boost::optional<Visit> optLastVisit; // should always be filled after visit.
    ast::IType* targetTypePtr;
    
    // This will throw an exception if the types can't be matched.
    // If the types already match, it'll return a null implicit cast.
    ImplicitCast matchTarget(const ast::IType& type) {
        if (targetTypePtr == nullptr || doTypesMatch(type, *targetTypePtr))
            return ImplicitCast(type.clone(), CastKindNullCast);
        
        // Try an integer upcast
        // (will return boost::none if they're not both integer types)
        boost::optional<ImplicitCast> optImplicitCast = tryCreateImplicitIntegerUpcast(type, *targetTypePtr);
        if (optImplicitCast) {
            return std::move(*optImplicitCast);
        }
        
        throw ASTCompilerExceptionCannotMatchTargetType();
    }
    ImplicitCast matchTarget(const std::unique_ptr<ast::IType>& type) {
        return matchTarget(*type);
    }
    
protected:
    ASTCompilerExpVisitor(ASTCompilationConfig& config, Generator& generator,
                          SymbolTable<VariableDeclAnnotations, FunctionDeclAnnotations>& symbolTable) :
    config(config), generator(generator), symbolTable(symbolTable), targetTypePtr(nullptr) {
        
    }
    
public:
    void setTargetType(ast::IType* targetTypePtr) {
        if (optLastVisit) {
            assert(targetTypePtr != optLastVisit->resolvedType.get());
        }
        this->targetTypePtr = targetTypePtr;
    }
    void setNoTargetType() { targetTypePtr = nullptr; }
    
    virtual void visit(ast::BinopExp& binOpExp) {
        ast::IType* origTargetTypePtr = targetTypePtr;
        performVisit(binOpExp.lhs, nullptr);
        Temporary lhsVal = val();
        std::unique_ptr<ast::IType> lhsType = optLastVisit->resolvedType->clone();
        performVisit(binOpExp.rhs, lhsType.get());
        Temporary rhsVal = val();
        setTargetType(origTargetTypePtr);
        
        std::unique_ptr<ast::IType> ourUnmatchedType;
        if (doesBinopEvaluateToConstBoolean(binOpExp.code)) {
            ourUnmatchedType = std::make_unique<ast::BoolType>(NotConstant);
        }
        else if (doesBinopEvaluateToConstOperandType(binOpExp.code)) {
            ourUnmatchedType = lhsType->clone();
            ourUnmatchedType->setConstant(false);
        }
        else {
            assert(false);
        }
        
        ImplicitCast cast = matchTarget(ourUnmatchedType);
        Temporary val = generator.genCast(generator.genBinopExp(*lhsType, lhsVal, binOpExp.code, rhsVal), cast);
        optLastVisit = Visit(std::move(cast.type), val);
    }
    virtual void visit(ast::CallExp& callExp) {
        std::pair<ast::FunctionDecl*, FunctionDeclAnnotations> lookup(symbolTable.lookupFunction(callExp.calleeIdentifier));
        ast::FunctionDecl& decl = *lookup.first;
        FunctionDeclAnnotations declAnnotations = lookup.second;
        
        // Work out our cast now so we can overwrite
        // the target type when generating the arguments.
        ImplicitCast cast = matchTarget(decl.returnType);
        
        if (decl.arguments.size() != callExp.passedArguments.size())
            throw ASTCompilerExceptionNumArgsDoNotMatch();
        
        ast::IType* oldTargetTypePtr = targetTypePtr;
        
        std::vector<Temporary> argVals;
        for (uint64_t i = 0; i < callExp.passedArguments.size(); i++) {
            std::unique_ptr<ast::IExp>& passedArgExp = callExp.passedArguments[i];
            ast::IType& argDesiredType = decl.arguments[i].getType();
            performVisit(passedArgExp, &argDesiredType);
            argVals.push_back(val());
        }
        
        setTargetType(oldTargetTypePtr);
        
        Temporary val = generator.genCast(generator.genCall(declAnnotations, argVals), cast);
        optLastVisit = Visit(std::move(cast.type), val);
    }
    virtual void visit(ast::NumberLiteralExp& numberLiteralExp) {
        #warning TODO: SIMPLIFY ACCESS TO VAL OF TARGET TYPE
        #warning TODO: REFACTOR INTO METHODS
        std::unique_ptr<ast::IType> ourType;
        Temporary val;
        if (targetTypePtr != nullptr) {
            // Number literals are special: we never need to cast.
            // We can just directly generate whatever type we need.
            // If there's a decimal point, the type of the generated
            // expression is either a float or a double.
            // However, if there's no decimal point, we'll generate any
            // numeral target type.
            bool forceNotInteger = numberLiteralExp.hasDecimalPoint;
            ast::IntegerType* iTargetType;
            ast::FloatingPointType* fpTargetType;
            if (!forceNotInteger && (iTargetType = dynamic_cast<ast::IntegerType*>(targetTypePtr))) {
                val = generator.genIntLiteral(*iTargetType, numberLiteralExp.value);
            }
            else if ((fpTargetType = dynamic_cast<ast::FloatingPointType*>(targetTypePtr))) {
                val = generator.genFloatingPointLiteral(*fpTargetType, numberLiteralExp.value);
            }
            else {
                throw ASTCompilerExceptionCannotMatchTargetType();
            }
            ourType = targetTypePtr->clone();
        }
        else {
            // If there's decimal point, it's an integer literal.
            // If there is one, it's a floating point literal.
            // We consult the config for the details of the type.
            if (!numberLiteralExp.hasDecimalPoint) {
                auto signedness = config.assumedIntegerLiteralIsSigned ? ast::IntegerType::Signed : ast::IntegerType::Unsigned;
                auto iOurType = std::make_unique<ast::IntegerType>(config.assumedIntegerLiteralNumBits, signedness, NotConstant);
                val = generator.genIntLiteral(*iOurType, numberLiteralExp.value);
                ourType = std::move(iOurType);
            }
            else {
                auto variation = config.assumedFloatingPointLiteralIsDouble ? ast::FloatingPointType::VariationDouble :
                                                                              ast::FloatingPointType::VariationFloat;
                ourType = std::make_unique<ast::FloatingPointType>(variation, NotConstant);
                val = generator.genFloatingPointLiteral(*ourType, numberLiteralExp.value);
            }
        }
        
        optLastVisit = Visit(std::move(ourType), val);
    }
    virtual void visit(ast::VariableExp& variableExp) {
        std::pair<const ast::VariableDecl*, VariableDeclAnnotations> lookup(symbolTable.lookupVariable(variableExp.identifier));
        const ast::VariableDecl* decl = lookup.first;
        VariableDeclAnnotations declAnnotations = lookup.second;
        
        ImplicitCast cast = matchTarget(decl->type);
        Temporary value = generator.genCast(generator.genLocalEval(declAnnotations), cast);
        
        // set optlastvisit
        optLastVisit = Visit(std::move(cast.type), value);
    }
    
    void performVisit(ast::IExp& exp, ast::IType* targetType) {
        setTargetType(targetType);
        exp.accept(*this);
        setNoTargetType();
    }
    void performVisit(const std::unique_ptr<ast::IExp>& exp, ast::IType* targetType) {
        performVisit(*exp, targetType);
    }
    Temporary val() {
        assert(optLastVisit);
        return optLastVisit->val;
    }
};
