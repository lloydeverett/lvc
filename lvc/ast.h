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
#include <string>

class INode {
protected:
    // Protected constructor prevents this class from being constructed
    // (it should only be used as a base class for a derived class)
    INode() {}
    
public:
    // Making the virtual destructor public allows the derived classes
    // to be destructed polymorphically (i.e. through their base class)
    virtual ~INode() {}
};

///////////////////////////////

class IDecl : public INode {
protected:
    IDecl() {};
};

class IExp : public INode {
protected:
    IExp() {};
};

class IStmt : public INode {
protected:
    IStmt() {};
};

class IType : public INode {
protected:
    IType() {};
};

///////////////////////////////

class PrimitiveType : public IType {
private:
    PrimitiveTypename name;
public:
    PrimitiveType(PrimitiveTypename name) :
    name(name) {}
};

class VariableDecl : public IDecl {
    std::unique_ptr<IType> type;
    std::string identifier;
public:
    VariableDecl(std::unique_ptr<IType> type, std::string identifier) :
    type(std::move(type)), identifier(identifier) {}
};

class VariableExp : public IExp {
    const VariableDecl &decl;
public:
    VariableExp(VariableDecl &decl) :
    decl(decl) {}
};

class ArgumentDecl : public IDecl {
    VariableDecl variableDecl;
    std::unique_ptr<IExp> defaultValue;
public:
    ArgumentDecl(VariableDecl variableDecl, std::unique_ptr<IExp> defaultValue) :
    variableDecl(std::move(variableDecl)), defaultValue(std::move(defaultValue)) {}
};

class FunctionDecl : public IDecl {
    std::unique_ptr<IType> returnType;
    std::string identifier;
    std::vector<std::unique_ptr<ArgumentDecl>> arguments;
public:
    FunctionDecl(std::unique_ptr<IType> returnType, std::string identifier, std::vector<std::unique_ptr<ArgumentDecl>> arguments) :
    returnType(std::move(returnType)), identifier(identifier), arguments(std::move(arguments)) {}
};

class VariableDeclStmt : public IStmt {
    VariableDecl variableDecl;
    std::unique_ptr<IExp> initialValue;
public:
    VariableDeclStmt(VariableDecl variableDecl, std::unique_ptr<IExp> initialValue) :
    variableDecl(std::move(variableDecl)), initialValue(std::move(initialValue)) {}
};

class ReturnStmt : public IStmt {
    std::unique_ptr<IExp> returnedExpression;
public:
    ReturnStmt(std::unique_ptr<IExp> returnedExpression) :
    returnedExpression(std::move(returnedExpression)) {}
};

class IntegerLiteralExp : public IExp {
    std::string value;
    
public:
    IntegerLiteralExp(std::string value) :
    value(value) {}
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

///////////////////////////////

class Function : public INode {
    FunctionDecl functionDecl;
    std::vector<std::unique_ptr<IStmt>> statements;
public:
    Function(FunctionDecl functionDecl, std::vector<std::unique_ptr<IStmt>> statements) :
    functionDecl(std::move(functionDecl)), statements(std::move(statements)) {}
};

class Module : public INode {
    std::vector<Function> functions;
public:
    Module(std::vector<Function> functions) :
    functions(std::move(functions)) {}
};
