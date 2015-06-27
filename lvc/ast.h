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
#include "binopcode.h"
#include "primitivetypecode.h"

class INodeVisitor;

namespace ast {
    
    class INode {
    protected:
        // The protected constructor prevents this class from being constructed
        // (it should only be used as a base class for a derived class).
        INode() {}
        
    public:
        virtual std::ostream& dump(std::ostream& o) const = 0;
        virtual void accept(INodeVisitor& visitor) = 0;
        
        // Making the virtual destructor public allows the derived classes
        // to be destructed polymorphically (i.e. through their base class).
        virtual ~INode() {}
    };
    
    inline std::ostream& operator<<(std::ostream& o, const INode& n) { return n.dump(o); }
    
    ///////////////////////////////
    
    class IDecl : public INode {
    protected:
        IDecl() {};
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
    
    ///////////////////////////////
    
    struct PrimitiveType : IType {
        PrimitiveTypeCode code;
        
        PrimitiveType(PrimitiveTypeCode code) : code(code) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "("  << debugStringForPrimitiveType(code) << ")";
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct IntegerLiteralExp : public IExp {
        std::string valueStr;
        IntegerLiteralExp(std::string valueStr) :
        valueStr(valueStr) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "(IntegerLiteralExp: " << valueStr << ")";
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct RealLiteralExp : public IExp {
        std::string valueStr;
        RealLiteralExp(std::string valueStr) :
        valueStr(valueStr) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "(RealLiteralExp: " << valueStr << ")";
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct VariableDecl : public IDecl {
        std::unique_ptr<IType> type_ptr;
        std::string identifier;
        VariableDecl(std::unique_ptr<IType> type_ptr, std::string identifier) :
        type_ptr(std::move(type_ptr)),
        identifier(identifier) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "(VariableDecl: " << *type_ptr << " " << identifier << ")";
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct VariableExp : public IExp {
        VariableDecl *decl;
        VariableExp(VariableDecl *decl) :
        decl(decl) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "(VariableExp: " << *decl << ")";
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct ArgumentDecl : public IDecl {
        VariableDecl variableDecl;
        boost::optional<std::unique_ptr<IExp>> defaultValue_ptr;
        ArgumentDecl(VariableDecl variableDecl, boost::optional<std::unique_ptr<IExp>> defaultValue_ptr = boost::none) :
        variableDecl(std::move(variableDecl)), defaultValue_ptr(std::move(defaultValue_ptr)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "(ArgumentDecl: " << variableDecl;
            if (defaultValue_ptr)
                o << ", " << **defaultValue_ptr;
            o << ")";
            return o;
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct FunctionDecl : public IDecl {
        std::unique_ptr<IType> returnType_ptr;
        std::string identifier;
        std::vector<ArgumentDecl> arguments;
        FunctionDecl(std::unique_ptr<IType> returnType_ptr, std::string identifier, std::vector<ArgumentDecl> arguments) :
        returnType_ptr(std::move(returnType_ptr)),
        identifier(identifier), arguments(std::move(arguments)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "(FunctionDecl: " << *returnType_ptr << ", " << identifier << ", (Arguments: " ;
            for (const ArgumentDecl &decl : arguments) {
                o << decl;
            }
            o << "))";
            return o;
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct VariableDeclStmt : public IStmt {
        boost::optional<std::unique_ptr<IExp>> initialValue_ptr;
        VariableDecl decl;
        
        VariableDeclStmt(VariableDecl decl, boost::optional<std::unique_ptr<IExp>> initialValue_ptr = boost::none) :
        initialValue_ptr(std::move(initialValue_ptr)),
        decl(std::move(decl))  {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            if (initialValue_ptr)
                return o << "(VariableDeclStmt: " << decl << ", " << **initialValue_ptr << ")";
            else
                return o << "(VariableDeclStmt: " << decl << ")";
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct ReturnStmt : public IStmt {
        boost::optional<std::unique_ptr<IExp>> returnedExpression_ptr;
        ReturnStmt(boost::optional<std::unique_ptr<IExp>> returnedExpression_ptr = boost::none) :
        returnedExpression_ptr(std::move(returnedExpression_ptr)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            if (returnedExpression_ptr)
                return o << "(ReturnStmt: " << **returnedExpression_ptr << ")";
            else
                return o << "(ReturnStmt)";
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct BinopExp : public IExp {
        std::unique_ptr<IExp> lhs;
        BinopCode code;
        std::unique_ptr<IExp> rhs;
        BinopExp(std::unique_ptr<IExp> lhs, BinopCode code, std::unique_ptr<IExp> rhs) :
        lhs(std::move(lhs)), code(code), rhs(std::move(rhs)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "(BinopExp: " << *lhs << ", " << debugStringForBinop(code) << ", " << *rhs << ")";
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct FunctionCallExp : public IExp {
        FunctionDecl* calleeDeclRef;
        std::vector<std::unique_ptr<IExp>> passedArguments;
        FunctionCallExp(FunctionDecl* calleeDeclRef, std::vector<std::unique_ptr<IExp>> passedArguments) :
        calleeDeclRef(calleeDeclRef), passedArguments(std::move(passedArguments)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "(FunctionCallExp: " << *calleeDeclRef << ", (Arguments: ";
            for (const std::unique_ptr<IExp>& exp_ptr : passedArguments) {
                o << *exp_ptr;
            }
            o << "))";
            return o;
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct FunctionCallStmt : public IStmt {
        FunctionCallExp functionCallExp;
        FunctionCallStmt(FunctionCallExp functionCallExp) :
        functionCallExp(std::move(functionCallExp)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "(FunctionCallStmt: " << functionCallExp << ")";
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct BlockStmt : public IStmt {
        std::vector<std::unique_ptr<IStmt>> statements;
        BlockStmt(std::vector<std::unique_ptr<IStmt>> statements) :
        statements(std::move(statements)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "(Block: ";
            for (const std::unique_ptr<IStmt>& statement_ptr : statements) {
                o << *statement_ptr;
                if (statement_ptr != statements[statements.size() - 1]) {
                    o << ", ";
                }
            }
            o << ")";
            return o;
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct IfStmt : public IStmt {
        std::unique_ptr<IExp> condition;
        BlockStmt thenBlock;
        boost::optional<BlockStmt> elseBlock;
        IfStmt(std::unique_ptr<IExp> condition, BlockStmt thenBlock, boost::optional<BlockStmt> elseBlock = boost::none) :
        condition(std::move(condition)), thenBlock(std::move(thenBlock)), elseBlock(std::move(elseBlock)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "(IfStmt: " << *condition << ", ";
            o << thenBlock;
            if (elseBlock) {
                o << ", " << *elseBlock;
            }
            o << ")";
            return o;
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct Function : public INode {
        FunctionDecl decl;
        BlockStmt block;
        Function(FunctionDecl decl, BlockStmt block) :
        decl(std::move(decl)), block(std::move(block)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "(Function: " << decl << ", " << block;
            return o;
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct Module : public INode {
        std::string moduleName;
        std::vector<Function> functions;
        Module(std::string moduleName, std::vector<Function> functions) :
        moduleName(moduleName), functions(std::move(functions)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "Module: " << moduleName << std::endl;
            for (const Function& function : functions) {
                o << function << std::endl;
            }
            return o;
        }
        
        virtual void accept(INodeVisitor& visitor) override;
    };
    
}
