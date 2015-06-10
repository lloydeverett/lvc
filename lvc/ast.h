//
//  ast.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/17.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "primitivetypename.h"
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
public:
    PrimitiveTypename name;
    
    PrimitiveType(PrimitiveTypename name) :
    name(name) {}
};

class VariableDecl : public IDecl {
    std::unique_ptr<IType> type_ptr;
public:
    std::string identifier;
    IType &type;
    
    VariableDecl(std::unique_ptr<IType> type_ptr, std::string identifier) :
    type_ptr(std::move(type_ptr)),
    type(*type_ptr.get()), identifier(identifier) {}
};

class VariableExp : public IExp {
public:
    VariableDecl &decl;
    
    VariableExp(VariableDecl &decl) :
    decl(decl) {}
};

class ArgumentDecl : public IDecl {
    std::unique_ptr<IType> type_ptr;
    std::unique_ptr<IExp> defaultValue_ptr;
public:
    IType &type;
    std::string identifier;
    IExp &defaultValue;
    
    ArgumentDecl(std::unique_ptr<IType> type_ptr, std::string identifier, std::unique_ptr<IExp> defaultValue) :
    type_ptr(std::move(type_ptr)), defaultValue_ptr(std::move(defaultValue_ptr)),
    type(*type_ptr.get()), identifier(identifier), defaultValue(*defaultValue_ptr.get()) {}
};

class FunctionDecl : public IDecl {
    std::unique_ptr<IType> returnType_ptr;
public:
    std::string identifier;
    std::vector<ArgumentDecl> arguments;
    IType &returnType;
    
    FunctionDecl(std::unique_ptr<IType> returnType_ptr, std::string identifier, std::vector<ArgumentDecl> arguments) :
    returnType_ptr(std::move(returnType_ptr)),
    identifier(identifier), arguments(std::move(arguments)), returnType(*returnType_ptr.get()) {}
};

class VariableDeclStmt : public IStmt {
    std::unique_ptr<IExp> initialValue_ptr;
public:
    VariableDecl decl;
    IExp &initialValue;
    
    VariableDeclStmt(VariableDecl decl, std::unique_ptr<IExp> initialValue_ptr) :
    initialValue_ptr(std::move(initialValue_ptr)),
    initialValue(*initialValue_ptr.get()), decl(std::move(decl))  {}
};

class ReturnStmt : public IStmt {
    std::unique_ptr<IExp> returnedExpression_ptr;
public:
    IExp &returnedExpression;
    
    ReturnStmt(std::unique_ptr<IExp> returnedExpression_ptr) :
    returnedExpression_ptr(std::move(returnedExpression_ptr)),
    returnedExpression(*returnedExpression_ptr.get()) {}
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
    
    std::string toString() {
        std::string s;
        s+= "FunctionCall: ";
        
    }
};

class FunctionCallStmt : public IStmt {
    FunctionCallExp functionCallExp;
public:
    FunctionCallStmt(FunctionCallExp functionCallExp) :
    functionCallExp(std::move(functionCallExp)) {}
    
    std::string toString() {
        std::string s;
        s += "FunctionCallExpStmt ";
        s += functionCallExp.toString();
        return s;
    }
};

///////////////////////////////

class Function : public INode {
    FunctionDecl decl;
    std::vector<std::unique_ptr<IStmt>> statements;
public:
    Function(FunctionDecl decl, std::vector<std::unique_ptr<IStmt>> statements) :
    decl(std::move(decl)), statements(std::move(statements)) {}
    
    std::string toString() {
        std::string s;
        s += decl.toString() + "\n";
        for (std::unique_ptr<IStmt> &statement_ptr : statements)
            s += "    " + statement_ptr.get()->toString() + "\n";
        return s;
    }
    
    IStmt &getStatement(size_t index) {
        return *statements.at(index).get();
    }
    
    size_t numStatements() {
        return statements.size();
    }
};

class Module : public INode {
public:
    std::vector<Function> functions;
    
    Module(std::vector<Function> functions) :
    functions(std::move(functions)) {}
    
    std::string toString() {
        std::string s;
        for (Function &f : functions) s += f.toString() + "\n";
        return s;
    }
    
};
