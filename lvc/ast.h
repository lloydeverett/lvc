//
//  ast.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/17.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "primitivetypeenum.h"
#include <cstdint>
#include <vector>
#include <memory>
#include <string>

class INodeVisitor {
    
};

class INode {
protected:
    // Protected constructor prevents this class from being constructed
    // (it should only be used as a base class for a derived class)
    INode() {}
    
public:
    virtual std::string toString() const = 0;
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

struct PrimitiveType : public IType {
    PrimitiveTypeEnum name;
    PrimitiveType(PrimitiveTypeEnum name) :
    name(name) {}
    
    virtual std::string toString() const {
        return stringFromPrimitiveTypeEnum(name);
    }
};

struct VariableDecl : public IDecl {
    std::unique_ptr<IType> type_ptr;
    std::string identifier;
    VariableDecl(std::unique_ptr<IType> type_ptr, std::string identifier) :
    type_ptr(std::move(type_ptr)),
    identifier(identifier) {}
    
    virtual std::string toString() const {
        std::string s;
        s += "(VariableDecl) ";
        s += type_ptr->toString();
        s += identifier;
        return s;
    }
};

struct VariableExp : public IExp {
    VariableDecl &decl;
    VariableExp(VariableDecl &decl) :
    decl(decl) {}
    
    virtual std::string toString() const {
        std::string s;
        s += "(VariableExp) ";
        s += decl.toString();
        return s;
    }
};

struct ArgumentDecl : public IDecl {
    std::unique_ptr<IType> type_ptr;
    std::unique_ptr<IExp> defaultValue_ptr;
    std::string identifier;
    ArgumentDecl(std::unique_ptr<IType> type_ptr, std::string identifier, std::unique_ptr<IExp> defaultValue) :
    type_ptr(std::move(type_ptr)), defaultValue_ptr(std::move(defaultValue_ptr)),
    identifier(identifier) {}
    
    virtual std::string toString() const {
        std::string s;
        s += "(ArgumentDecl) ";
        s += type_ptr->toString();
        s += " "; s += identifier;
        return s;
    }
};

struct FunctionDecl : public IDecl {
    std::unique_ptr<IType> returnType_ptr;
    std::string identifier;
    std::vector<ArgumentDecl> arguments;
    FunctionDecl(std::unique_ptr<IType> returnType_ptr, std::string identifier, std::vector<ArgumentDecl> arguments) :
    returnType_ptr(std::move(returnType_ptr)),
    identifier(identifier), arguments(std::move(arguments)) {}
    
    virtual std::string toString() const {
        std::string s;
        s += returnType_ptr->toString();
        s += " ";
        s += identifier;
        s += "(";
        if (arguments.size() > 0) {
            for (size_t i = 0; i < arguments.size() - 1; i++) {
                s += arguments[i].toString();
                s += ", ";
            }
            s += arguments[arguments.size() - 1].toString();
        }
        s += ")";
        return s;
    }
};

struct VariableDeclStmt : public IStmt {
    std::unique_ptr<IExp> initialValue_ptr;
    VariableDecl decl;
    
    VariableDeclStmt(VariableDecl decl, std::unique_ptr<IExp> initialValue_ptr) :
    initialValue_ptr(std::move(initialValue_ptr)),
    decl(std::move(decl))  {}
    
    virtual std::string toString() const {
        std::string s;
        s += "(VariableDeclStmt) ";
        s += decl.toString() + " = ";
        s += initialValue_ptr->toString();
        return s;
    }
};

struct ReturnStmt : public IStmt {
    std::unique_ptr<IExp> returnedExpression_ptr;
    ReturnStmt(std::unique_ptr<IExp> returnedExpression_ptr) :
    returnedExpression_ptr(std::move(returnedExpression_ptr)) {}
    
    virtual std::string toString() const {
        return "return " + returnedExpression_ptr->toString();
    }
};

struct IntegerLiteralExp : public IExp {
    std::string value;
    IntegerLiteralExp(std::string value) :
    value(value) {}
    
    virtual std::string toString() const {
        return value;
    }
};

struct BinOpExp : public IExp {
    char op;
    std::unique_ptr<IExp> lhs;
    std::unique_ptr<IExp> rhs;
    BinOpExp(char op, std::unique_ptr<IExp> lhs, std::unique_ptr<IExp> rhs) :
    op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
    
    virtual std::string toString() const {
        std::string s;
        s += "(";
        s += lhs->toString();
        s += ")(";
        s += rhs->toString();
        s += ")";
        s += op;
        return s;
    }
};

struct FunctionCallExp : public IExp {
    const FunctionDecl &calleeDeclRef;
    std::vector<std::unique_ptr<IExp>> passedArguments;
    FunctionCallExp(const FunctionDecl &calleeDeclRef, std::vector<std::unique_ptr<IExp>> passedArguments) :
    calleeDeclRef(calleeDeclRef), passedArguments(std::move(passedArguments)) {}
    
    std::string toString() const {
        std::string s;
        s += calleeDeclRef.toString();
        s += "(";
        if (passedArguments.size() > 0) {
            for (int i = 0; i < passedArguments.size() - 1; i++) {
                s += passedArguments[i]->toString() + ", ";
            }
            s += passedArguments[passedArguments.size() - 1]->toString();
        }
        s += ")";
        return s;
    }
};

struct FunctionCallStmt : public IStmt {
    FunctionCallExp functionCallExp;
    FunctionCallStmt(FunctionCallExp functionCallExp) :
    functionCallExp(std::move(functionCallExp)) {}
    
    std::string toString() const {
        std::string s;
        s += "(FunctionCallExpStmt) ";
        s += functionCallExp.toString();
        return s;
    }
};

///////////////////////////////

struct Function : public INode {
    FunctionDecl decl;
    std::vector<std::unique_ptr<IStmt>> statements;
    Function(FunctionDecl decl, std::vector<std::unique_ptr<IStmt>> statements) :
    decl(std::move(decl)), statements(std::move(statements)) {}
    
    std::string toString() const {
        std::string s;
        s += "(Function)\n";
        s += decl.toString() + "\n";
        for (const std::unique_ptr<IStmt> &statement_ptr : statements)
            s += "    " + statement_ptr->toString() + "\n";
        return s;
    }
    
    IStmt& getStatement(size_t index) {
        return *statements.at(index);
    }
    
    size_t numStatements() const {
        return statements.size();
    }
};

struct Module : public INode {
    std::vector<Function> functions;
    Module(std::vector<Function> functions) :
    functions(std::move(functions)) {}
    
    std::string toString() const {
        std::string s;
        for (const Function &f : functions) s += f.toString() + "\n";
        return s;
    }
};
