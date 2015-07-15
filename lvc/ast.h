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
#include <typeinfo>
#include <typeindex>
#include "binopcode.h"

class INodeVisitor;

enum Constantness {
    Constant = true,
    NotConstant = false,
};

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
    
    class IType : public INode {
    private:
        Constantness isConstantVal;
    protected:
        IType(Constantness isConstantVal) : isConstantVal(isConstantVal) {};
    public:
        virtual std::type_index typeIndex() = 0;
        bool isConstant() {
            return isConstantVal;
        }
    };
    
    class IDecl : public INode {
    protected:
        IDecl() {};
    public:
        virtual const std::string& getIdentifier() const = 0;
        virtual const IType& getType() const = 0;
    };
    
    class IStmt : public INode {
    protected:
        IStmt() {};
    };
    
    class IExp : public INode {
    protected:
        IExp() {};
    };
    
    ///////////////////////////////
    
    struct VoidType : public IType {
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "VoidType";
        }
        virtual void accept(INodeVisitor& visitor) override;
        std::type_index typeIndex() override {
            return typeid(*this);
        }
    };
    
    struct IntegerType : public IType {
        int numBits;
        enum Signedness {
            Signed,
            Unsigned,
        } isSigned;
        IntegerType(int numBits, Signedness isSigned, Constantness isConstant) :
        numBits(numBits), isSigned(isSigned), IType(isConstant) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "IntegerType(" << numBits << ", " << (isSigned == Signed ? "Signed" : "Unsigned") << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
        std::type_index typeIndex() override {
            return typeid(*this);
        }
    };
    
    struct BoolType : public IType {
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "BoolType";
        }
        virtual void accept(INodeVisitor& visitor) override;
        std::type_index typeIndex() override {
            return typeid(*this);
        }
    };
    
    struct FloatingPointType : public IType {
        enum Variation {
            VariationFloat,
            VariationDouble,
        } variation;
        
        FloatingPointType(Variation variation, Constantness isConstant) :
        variation(variation), IType(isConstant) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "FloatingPointType(" << (variation == VariationFloat ? "Float" : "Double") << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
        std::type_index typeIndex() override {
            return typeid(*this);
        }
    };
    
    struct NumberLiteralExp : public IExp {
        std::string value;
        NumberLiteralExp(std::string value) :
        value(value) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "NumberLiteral(" << value << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct VariableDecl : public IDecl {
        std::unique_ptr<IType> type;
        std::string identifier;
        VariableDecl(std::unique_ptr<IType> type, std::string identifier) :
        type(std::move(type)),
        identifier(identifier) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "VariableDecl(" << *type << ", " << identifier << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
        virtual const std::string& getIdentifier() const override {
            return identifier;
        }
        virtual const IType& getType() const override {
            return *type;
        }
    };
    
    struct VariableExp : public IExp {
        std::string identifier;
        VariableExp(std::string identifier) :
        identifier(identifier) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "VariableExp(" << identifier << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct ArgumentDecl : public IDecl {
        VariableDecl variableDecl;
        boost::optional<std::unique_ptr<IExp>> optDefaultValue;
        ArgumentDecl(VariableDecl variableDecl, boost::optional<std::unique_ptr<IExp>> optDefaultValue = boost::none) :
        variableDecl(std::move(variableDecl)), optDefaultValue(std::move(optDefaultValue)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "ArgumentDecl(" << variableDecl;
            if (optDefaultValue)
                o << ", " << **optDefaultValue;
            o << ")";
            return o;
        }
        virtual void accept(INodeVisitor& visitor) override;
        virtual const std::string& getIdentifier() const override {
            return variableDecl.identifier;
        }
        virtual const IType& getType() const override {
            return variableDecl.getType();
        }
    };
    
    struct FunctionDecl : public IDecl {
        std::unique_ptr<IType> returnType;
        std::string identifier;
        std::vector<ArgumentDecl> arguments;
        FunctionDecl(std::unique_ptr<IType> returnType, std::string identifier, std::vector<ArgumentDecl> arguments) :
        returnType(std::move(returnType)),
        identifier(identifier), arguments(std::move(arguments)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "FunctionDecl(" << *returnType << ", " << identifier;
            if (!arguments.empty()) {
                o << ", Arguments(";
                for (const ArgumentDecl &decl : arguments) {
                    o << &decl;
                    if (&decl != &arguments[arguments.size() - 1]) {
                        o << ", ";
                    }
                }
                o << ")";
            }
            o << ")";
            return o;
        }
        virtual void accept(INodeVisitor& visitor) override;
        virtual const std::string& getIdentifier() const override {
            return identifier;
        }
        virtual const IType& getType() const override {
            return *returnType;
        }
    };
    
    struct VariableDeclStmt : public IStmt {
        boost::optional<std::unique_ptr<IExp>> optInitialValue;
        VariableDecl decl;
        
        VariableDeclStmt(VariableDecl decl, boost::optional<std::unique_ptr<IExp>> optInitialValue = boost::none) :
        optInitialValue(std::move(optInitialValue)),
        decl(std::move(decl))  {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            if (optInitialValue)
                return o << "VariableDeclStmt(" << decl << ", " << **optInitialValue << ")";
            else
                return o << "VariableDeclStmt(" << decl << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct ReturnStmt : public IStmt {
        boost::optional<std::unique_ptr<IExp>> optReturnedExpression;
        ReturnStmt(boost::optional<std::unique_ptr<IExp>> optReturnedExpression = boost::none) :
        optReturnedExpression(std::move(optReturnedExpression)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            if (optReturnedExpression)
                return o << "ReturnStmt(" << **optReturnedExpression << ")";
            else
                return o << "ReturnStmt";
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
            return o << "BinopExp(" << *lhs << ", " << debugStringForBinop(code) << ", " << *rhs << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct CallExp : public IExp {
        std::string calleeIdentifier;
        std::vector<std::unique_ptr<IExp>> passedArguments;
        CallExp(std::string calleeIdentifier, std::vector<std::unique_ptr<IExp>> passedArguments) :
        calleeIdentifier(calleeIdentifier), passedArguments(std::move(passedArguments)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "CallExp(" << calleeIdentifier;
            if (!passedArguments.empty()) {
                o << ", Arguments(";
                for (const std::unique_ptr<IExp>& exp : passedArguments) {
                    o << *exp;
                    if (exp != passedArguments[passedArguments.size() - 1]) {
                        o << ", ";
                    }
                }
                o << ")";
            }
            o << ")";
            return o;
        }
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct CallExpStmt : public IStmt {
        CallExp callExp;
        CallExpStmt(CallExp callExp) :
        callExp(std::move(callExp)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "CallExpStmt(" << callExp << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct BlockStmt : public IStmt {
        std::vector<std::unique_ptr<IStmt>> statements;
        BlockStmt(std::vector<std::unique_ptr<IStmt>> statements) :
        statements(std::move(statements)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "Block(";
            for (const std::unique_ptr<IStmt>& statement : statements) {
                o << *statement;
                if (statement != statements[statements.size() - 1]) {
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
            o << "IfStmt(" << *condition << ", ";
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
            o << "Function(" << decl << ", " << block;
            return o;
        }
        virtual void accept(INodeVisitor& visitor) override;
    };
    
    struct Module : public INode {
        std::string moduleName;
        std::vector<Function> functions;
        std::vector<VariableDeclStmt> globals;
        Module(std::string moduleName, std::vector<Function> functions, std::vector<VariableDeclStmt> globals) :
        moduleName(moduleName), functions(std::move(functions)), globals(std::move(globals)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "Module " << moduleName << "\n\n";
            for (const VariableDeclStmt& global : globals) {
                o << global << "\n\n";
            }
            for (const Function& function : functions) {
                o << function << "\n\n";
            }
            return o;
        }
        virtual void accept(INodeVisitor& visitor) override;
    };
    
}