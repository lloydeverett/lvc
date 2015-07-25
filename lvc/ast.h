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

#warning TODO: I'm starting to think serializing this to YAML or something would be better than this printing mechanism. \
                        (and maybe we can generate the serialization code with a python script or something?)

enum Constantness {
    Constant = true,
    NotConstant = false,
};

namespace ast {
    
    class INode {
    protected:
        INode() {}
    public:
        virtual std::ostream& dump(std::ostream& o) const = 0;
        virtual void accept(INodeVisitor& visitor) = 0;
        virtual ~INode() {}
    };
    
    inline std::ostream& operator<<(std::ostream& o, const INode& n) { return n.dump(o); }
    
    ///////////////////////////////
    
    class IType : public INode {
    private:
        Constantness constantness;
    protected:
        IType(Constantness constantness) : constantness(constantness) {};
        static bool BaseIsEqual(const IType* a, const IType* b) {
            return a->isConstant() == b->isConstant();
        }
    public:
        bool isConstant() const { return constantness == Constant; }
        void setConstant(Constantness constantness) { this->constantness = constantness; }
        void setConstant(bool isConstant) { this->constantness = isConstant ? Constant : NotConstant; }
        virtual bool isEqual(const IType& type) const = 0;
        virtual std::unique_ptr<IType> clone() const = 0;
    };
    
    class IDecl : public INode {
    protected:
        IDecl() {};
    public:
        virtual const std::string& getIdentifier() const = 0;
        virtual IType& getType() = 0;
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
        VoidType(Constantness constantness) : IType(constantness) {}
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "VoidType";
        }
        virtual void accept(INodeVisitor& visitor) override;
        virtual bool isEqual(const IType& type) const override {
            return BaseIsEqual(this, &type) && dynamic_cast<const VoidType*>(&type) != nullptr;
        }
        virtual std::unique_ptr<IType> clone() const override {
            return std::make_unique<VoidType>(*this);
        }
    };
    
    struct IntegerType : public IType {
        int numBits;
        enum Signedness {
            Signed,
            Unsigned,
        } isSigned;
        IntegerType(int numBits, Signedness isSigned, Constantness constantness) :
        numBits(numBits), isSigned(isSigned), IType(constantness) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "IntegerType(" << numBits << ", " << (isSigned == Signed ? "Signed" : "Unsigned") << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
        virtual bool isEqual(const IType& type) const override {
            if (!BaseIsEqual(this, &type)) return false;
            const IntegerType* casted = dynamic_cast<const IntegerType*>(&type);
            if (casted)
                return numBits == casted->numBits && isSigned == casted->isSigned;
            else
                return false;
        }
        virtual std::unique_ptr<IType> clone() const override {
            return std::make_unique<IntegerType>(*this);
        }
    };
    
    struct BoolType : public IType {
        BoolType(Constantness constantness) : IType(constantness) {}
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "BoolType";
        }
        virtual void accept(INodeVisitor& visitor) override;
        virtual bool isEqual(const IType& type) const override {
            return BaseIsEqual(this, &type) && dynamic_cast<const BoolType*>(&type) != nullptr;
        }
        virtual std::unique_ptr<IType> clone() const override {
            return std::make_unique<BoolType>(*this);
        }
    };
    
    struct FloatingPointType : public IType {
        enum Variation {
            VariationFloat,
            VariationDouble,
        } variation;
        
        FloatingPointType(Variation variation, Constantness constantness) :
        variation(variation), IType(constantness) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "FloatingPointType(" << (variation == VariationFloat ? "Float" : "Double") << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
        virtual bool isEqual(const IType& type) const override {
            if (!BaseIsEqual(this, &type)) return false;
            const FloatingPointType* casted = dynamic_cast<const FloatingPointType*>(&type);
            if (casted)
                return variation == casted->variation;
            else
                return false;
        }
        virtual std::unique_ptr<IType> clone() const override {
            return std::make_unique<FloatingPointType>(*this);
        }
    };
    
    struct NumberLiteralExp : public IExp {
        std::string value;
        bool hasDecimalPoint;
        NumberLiteralExp(std::string value, bool hasDecimalPoint) :
        value(value), hasDecimalPoint(hasDecimalPoint) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "NumberLiteral(" << value << ", " << (hasDecimalPoint ? "DecimalPoint" : "NoDecimalPoint") << ")";
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
        virtual IType& getType() override {
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
        virtual IType& getType() override {
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
        virtual IType& getType() override {
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
    
    struct VarOpModStmt : public IStmt {
        std::unique_ptr<ast::VariableExp> lvalue;
        std::unique_ptr<ast::IExp> rvalue;
        
    };
    
    struct VarAssignStmt : public IStmt {
        std::vector<ast::VariableExp> lvalues;
        std::unique_ptr<ast::IExp> rvalue;
        VarAssignStmt(std::vector<ast::VariableExp> lvalues, std::unique_ptr<ast::IExp> rvalue) :
        lvalues(std::move(lvalues)), rvalue(std::move(rvalue)) {}
    };
    
    struct VarIncrementStmt : public IStmt {
        ast::VariableExp exp;
        VarIncrementStmt(ast::VariableExp exp) :
        exp(std::move(exp)) {}
    };
    
    struct VarDecrementStmt : public IStmt {
        ast::VariableExp exp;
        VarDecrementStmt(ast::VariableExp exp) :
        exp(std::move(exp)) {}
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