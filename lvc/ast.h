//
//  ast.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/17.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <vector>
#include <memory>
#include <string>
#include <iostream>
#include <boost/optional.hpp>
#include "boperator.h"

class INodeVisitor;

namespace ast {
    
    class INode {
    protected:
        // Protected constructor prevents this class from being constructed
        // (it should only be used as a base class for a derived class)
        INode() {}
        
    public:
        virtual std::ostream& dump(std::ostream& o) const = 0;
        virtual void accept(INodeVisitor &visitor) = 0;
        
        // Making the virtual destructor public allows the derived classes
        // to be destructed polymorphically (i.e. through their base class)
        virtual ~INode() {}
    };
    
    inline std::ostream& operator<<(std::ostream& o, const INode& n) { return n.dump(o); }
    
    ///////////////////////////////
    
    class IDecl : public INode {
    protected:
        IDecl() {};
    public:
        virtual const std::string& getIdentifier() const = 0;
    };
    
    class IStmt : public INode {
    protected:
        IStmt() {};
    };
    
    class IType : public INode {
    protected:
        IType() {};
    };
    
    class IExp : public INode {
    protected:
        IExp() {};
    };
    
    class IPrimType : public IType {
    protected:
        IPrimType() {}
    };
    
    ///////////////////////////////
    
    struct PrimitiveType : IType {
        static const int CHAR_CODE   = 10;
        static const int SHORT_CODE  = 11;
        static const int INT_CODE    = 12;
        static const int LONG_CODE   = 13;
        static const int UCHAR_CODE  = 20;
        static const int USHORT_CODE = 21;
        static const int UINT_CODE   = 22;
        static const int ULONG_CODE  = 23;
        static const int FLOAT_CODE  = 50;
        static const int DOUBLE_CODE = 51;
        static const int BOOL_CODE   = 52;
        
        int code;
        
        PrimitiveType(int code) : code(code) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << "(PrimitiveType"  << code << ")";
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct IntegerLiteralExp : public IExp {
        std::string value;
        IntegerLiteralExp(std::string value) :
        value(value) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << "(IntegerLiteralExp: " << value << ")";
        }
        
        virtual std::string dump() const {
            return value;
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct VariableDecl : public IDecl {
        std::unique_ptr<IType> type_ptr;
        std::string identifier;
        VariableDecl(std::unique_ptr<IType> type_ptr, std::string identifier) :
        type_ptr(std::move(type_ptr)),
        identifier(identifier) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << "(VariableDecl: " << *type_ptr << " " << identifier << ")";
        }
        
        virtual void accept(INodeVisitor &visitor) override;
        
        virtual const std::string& getIdentifier() const {
            return identifier;
        }
    };
    
    struct VariableExp : public IExp {
        VariableDecl *decl;
        VariableExp(VariableDecl *decl) :
        decl(decl) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << "(VariableExp: " << *decl << ")";
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct ArgumentDecl : public IDecl {
        VariableDecl variableDecl;
        boost::optional<std::unique_ptr<IExp>> defaultValue_ptr;
        ArgumentDecl(VariableDecl variableDecl, boost::optional<std::unique_ptr<IExp>> defaultValue_ptr = boost::none) :
        variableDecl(std::move(variableDecl)), defaultValue_ptr(std::move(defaultValue_ptr)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            o << "(ArgumentDecl: " << variableDecl;
            if (defaultValue_ptr)
                o << ", " << **defaultValue_ptr;
            o << ")";
            return o;
        }
        
        virtual void accept(INodeVisitor &visitor) override;
        
        virtual const std::string& getIdentifier() const {
            return variableDecl.identifier;
        }
    };
    
    struct FunctionDecl : public IDecl {
        std::unique_ptr<IType> returnType_ptr;
        std::string identifier;
        std::vector<ArgumentDecl> arguments;
        FunctionDecl(std::unique_ptr<IType> returnType_ptr, std::string identifier, std::vector<ArgumentDecl> arguments) :
        returnType_ptr(std::move(returnType_ptr)),
        identifier(identifier), arguments(std::move(arguments)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            o << "(FunctionDecl: " << *returnType_ptr << ", " << identifier << ", (Arguments: " ;
            for (const ArgumentDecl &decl : arguments) {
                o << decl;
            }
            o << "))";
            return o;
        }
        
        virtual void accept(INodeVisitor &visitor) override;
        
        virtual const std::string& getIdentifier() const {
            return identifier;
        }
    };
    
    struct VariableDeclStmt : public IStmt {
        boost::optional<std::unique_ptr<IExp>> initialValue_ptr;
        VariableDecl decl;
        
        VariableDeclStmt(VariableDecl decl, boost::optional<std::unique_ptr<IExp>> initialValue_ptr = boost::none) :
        initialValue_ptr(std::move(initialValue_ptr)),
        decl(std::move(decl))  {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            if (initialValue_ptr)
                return o << "(VariableDeclStmt: " << decl << ", " << **initialValue_ptr << ")";
            else
                return o << "(VariableDeclStmt: " << decl << ")";
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct ReturnStmt : public IStmt {
        boost::optional<std::unique_ptr<IExp>> returnedExpression_ptr;
        ReturnStmt(boost::optional<std::unique_ptr<IExp>> returnedExpression_ptr = boost::none) :
        returnedExpression_ptr(std::move(returnedExpression_ptr)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            if (returnedExpression_ptr)
                return o << "(ReturnStmt: " << **returnedExpression_ptr << ")";
            else
                return o << "(ReturnStmt)";
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct BinOpExp : public IExp {
        BOperator op;
        std::unique_ptr<IExp> lhs;
        std::unique_ptr<IExp> rhs;
        BinOpExp(BOperator op, std::unique_ptr<IExp> lhs, std::unique_ptr<IExp> rhs) :
        op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << "(BinOpExp: " << *lhs << ", " << *rhs << ")";
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct FunctionCallExp : public IExp {
        FunctionDecl* calleeDeclRef;
        std::vector<std::unique_ptr<IExp>> passedArguments;
        FunctionCallExp(FunctionDecl* calleeDeclRef, std::vector<std::unique_ptr<IExp>> passedArguments) :
        calleeDeclRef(calleeDeclRef), passedArguments(std::move(passedArguments)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            o << "(FunctionCallExp: " << *calleeDeclRef << ", (Arguments: ";
            for (const std::unique_ptr<IExp>& exp_ptr : passedArguments) {
                o << *exp_ptr;
            }
            o << "))";
            return o;
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct FunctionCallStmt : public IStmt {
        FunctionCallExp functionCallExp;
        FunctionCallStmt(FunctionCallExp functionCallExp) :
        functionCallExp(std::move(functionCallExp)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << "(FunctionCallStmt: " << functionCallExp << ")";
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct IfStmt : public IStmt {
        std::unique_ptr<IExp> condition;
        std::vector<std::unique_ptr<IStmt>> thenStatements;
        boost::optional<std::vector<std::unique_ptr<IStmt>>> elseStatements;
        IfStmt(std::unique_ptr<IExp> condition, std::vector<std::unique_ptr<IStmt>> thenStatements,
               boost::optional<std::vector<std::unique_ptr<IStmt>>> elseStatements = boost::none) :
        condition(std::move(condition)), thenStatements(std::move(thenStatements)), elseStatements(std::move(elseStatements)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            o << "(IfStmt: (";
            for (const std::unique_ptr<IStmt>& statement_ptr : thenStatements) {
                o << *statement_ptr << std::endl;
            }
            o << ")";
            if (elseStatements) {
                o << ", (";
                for (const std::unique_ptr<IStmt>& statement_ptr : *elseStatements) {
                    o << *statement_ptr << std::endl;
                }
                o << ")";
            }
            o << ")";
            return o;
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct Function : public INode {
        FunctionDecl decl;
        std::vector<std::unique_ptr<IStmt>> statements;
        Function(FunctionDecl decl, std::vector<std::unique_ptr<IStmt>> statements) :
        decl(std::move(decl)), statements(std::move(statements)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            o << decl << std::endl;
            for (const std::unique_ptr<IStmt>& statement_ptr : statements) {
                o << "    " << *statement_ptr << std::endl;
            }
            return o;
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct Module : public INode {
        std::string moduleName;
        std::vector<Function> functions;
        Module(std::string moduleName, std::vector<Function> functions) :
        moduleName(moduleName), functions(std::move(functions)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            o << "Module: " << moduleName << std::endl;
            for (const Function& function : functions) {
                o << function;
            }
            return o;
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
}
