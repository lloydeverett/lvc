//
//  ast.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/17.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <cstdint>
#include <vector>
#include <memory>

namespace ast {
    
    class INode {
    protected:
        // Protected constructor prevents this class from being constructed
        // (it should only be used as a base class for a derived class)
        INode();
        
    public:
        // Making the virtual destructor public allows the derived classes
        // to be destructed polymorphically (i.e. through their base class)
        virtual ~INode() {}
    };
    
    ///////////////////////////////
    
    class IDecl : public INode {
    protected:
        IDecl();
    };
    
    class IExp : public INode {
    protected:
        IExp();
    };
    
    class IStmt : public INode {
    protected:
        IStmt();
    };
    
    class IType : public INode {
    protected:
        IType();
    };
    
    ///////////////////////////////
    
    class ArgumentDecl : public IDecl {
        std::unique_ptr<IType> type;
        std::string identifier;
        std::unique_ptr<IExp> defaultValue;
    public:
        ArgumentDecl(std::unique_ptr<IType> type, std::string identifier, std::unique_ptr<IExp> defaultValue) :
        type(std::move(type)), identifier(identifier), defaultValue(std::move(defaultValue)) {}
    };
    
    class FunctionDecl : public IDecl {
        std::unique_ptr<IType> returnType;
        std::string identifier;
        std::vector<std::unique_ptr<ArgumentDecl>> arguments;
    public:
        FunctionDecl(std::unique_ptr<IType> returnType, std::string identifier, std::vector<std::unique_ptr<ArgumentDecl>> arguments) :
        returnType(std::move(returnType)), identifier(identifier), arguments(std::move(arguments)) {}
    };
    
    class DeclareVariableStmt : public IStmt {
        std::unique_ptr<IType> type;
        std::string identifier;
        std::unique_ptr<IExp> initialValue;
    public:
        DeclareVariableStmt(std::unique_ptr<IType> type, std::string identifier, std::unique_ptr<IExp> initialValue) :
        type(std::move(type)), identifier(identifier), initialValue(std::move(initialValue)) {}
    };
    
    class Function : public INode {
        FunctionDecl functionDecl;
        std::vector<std::unique_ptr<IStmt>> statements;
    public:
        Function(FunctionDecl functionDecl, std::vector<std::unique_ptr<IStmt>> statements) :
        functionDecl(std::move(functionDecl)), statements(std::move(statements)) {}
    };
    
    class ReturnStmt : public IStmt {
        std::unique_ptr<IExp> returnedExpression;
    public:
        ReturnStmt(std::unique_ptr<IExp> returnedExpression) :
        returnedExpression(std::move(returnedExpression)) {}
    };
    
    class IntegerLiteralExp : public IExp {
        std::unique_ptr<IType> type;
        uint64_t val;
    public:
        IntegerLiteralExp(std::unique_ptr<IType> type, uint64_t val) :
        type(std::move(type)), val(val) {}
    };
    
    class BinOpExp : public IExp {
        char op;
        std::unique_ptr<IExp> lhs;
        std::unique_ptr<IExp> rhs;
    public:
        BinOpExp(char op, std::unique_ptr<IExp> lhs, std::unique_ptr<IExp> rhs) :
        op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    };
    
    class FunctionCallExp : public IExp {
        const FunctionDecl &calleeDeclRef;
        std::vector<std::unique_ptr<IExp>> passedArguments;
    public:
        FunctionCallExp(const FunctionDecl &calleeDeclRef, std::vector<std::unique_ptr<IExp>> passedArguments) :
        calleeDeclRef(calleeDeclRef), passedArguments(std::move(passedArguments)) {}
    };
    
    class FunctionCallStmt : public IStmt {
        FunctionCallExp functionCallExp;
    public:
        FunctionCallStmt(FunctionCallExp functionCallExp) :
        functionCallExp(std::move(functionCallExp)) {}
    };
    
}
