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
#include "llvmincludes.h"
#include "binopcode.h"
#include "sminf.h"

class INodeVisitor;

#warning TODO: I'm starting to think serializing this to YAML or something would be better than this printing mechanism. \
                        (and maybe we can generate the serialization code with a python script or something?)

enum Constantness {
    Constant,
    NotConstant,
};

#warning TODO: qualtype like in Clang

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
    public:
        /// Discriminator for LLVM-style RTTI (dyn_cast<> et al.)
        enum TypeKind {
            TK_VoidType,
            TK_IntegerType,
            TK_BoolType,
            TK_FloatingPointType,
        };
    private:
        Constantness constantness;
        TypeKind kind;
    protected:
        IType(Constantness constantness, TypeKind kind) : constantness(constantness), kind(kind) {};
    public:
        TypeKind getKind() const { return kind; }
        Constantness getConstant() const { return constantness; }
        void setConstant(Constantness constantness) { this->constantness = constantness; }
        virtual std::unique_ptr<IType> clone() const = 0;
    };
    
    class IDecl : public INode {
    protected:
        IDecl() {};
    public:
        virtual const std::string& getIdentifier() const = 0;
        virtual IType& getType() = 0;
        llvm::Value* genVal;
    };
    
    class IStmt : public INode {
    protected:
        IStmt() {};
    };
    
    class IExp : public INode {
    protected:
        IExp() {};
    public:
        SMInf sminf;
        llvm::Value* genVal;
    };
    
    ///////////////////////////////
    
    struct VoidType : public IType {
        VoidType(Constantness constantness) : IType(constantness, TK_VoidType) {}
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "VoidType";
        }
        virtual void accept(INodeVisitor& visitor) override;
        virtual std::unique_ptr<IType> clone() const override {
            return std::make_unique<VoidType>(*this);
        }
        static bool classof(const IType* t) {
            return t->getKind() == TK_VoidType;
        }
    };
    
    struct IntegerType : public IType {
        int numBits;
        enum Signedness {
            Signed,
            Unsigned,
        } signedness;
        IntegerType(int numBits, Signedness signedness, Constantness constantness) :
        numBits(numBits), signedness(signedness), IType(constantness, TK_IntegerType) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "IntegerType(" << numBits << ", " << (signedness == Signed ? "Signed" : "Unsigned") << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
        virtual std::unique_ptr<IType> clone() const override {
            return std::make_unique<IntegerType>(*this);
        }
        bool isSigned() {
            return signedness == Signed;
        }
        static bool classof(const IType* t) {
            return t->getKind() == TK_IntegerType;
        }
    };
    
    struct BoolType : public IType {
        BoolType(Constantness constantness) : IType(constantness, TK_BoolType) {}
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "BoolType";
        }
        virtual void accept(INodeVisitor& visitor) override;
        virtual std::unique_ptr<IType> clone() const override {
            return std::make_unique<BoolType>(*this);
        }
        static bool classof(const IType* t) {
            return t->getKind() == TK_BoolType;
        }
    };
    
    struct FloatingPointType : public IType {
        enum Variation {
            VariationFloat,
            VariationDouble,
        } variation;
        
        FloatingPointType(Variation variation, Constantness constantness) :
        variation(variation), IType(constantness, TK_FloatingPointType) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "FloatingPointType(" << (variation == VariationFloat ? "Float" : "Double") << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
        virtual std::unique_ptr<IType> clone() const override {
            return std::make_unique<FloatingPointType>(*this);
        }
        static bool classof(const IType* t) {
            return t->getKind() == TK_FloatingPointType;
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
    
    struct VariableDecl : public IDecl {
    public:
        std::unique_ptr<IType> type;
        std::string identifier;
    protected:
        VariableDecl(std::unique_ptr<IType> type, std::string identifier) :
        type(std::move(type)), identifier(identifier) {}
    };
    
    struct ArgVariableDecl : public VariableDecl {
    public:
        boost::optional<std::unique_ptr<IExp>> optInit;
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o < "ArgVariableDecl(" << identifier << ")";
        }
    };
    
    struct NonArgVariableDecl : public VariableDecl {
    public:
        boost::optional<std::unique_ptr<IExp>> optDefault;
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "NonArgVariableDecl(" << identifier << ")";
        }
    };
    
    struct VariableExp : public IExp {
        ast::VariableDecl* sminfLookup;
        
        std::string identifier;
        VariableExp(std::string identifier) :
        identifier(identifier) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            return o << "VariableExp(" << identifier << ")";
        }
        virtual void accept(INodeVisitor& visitor) override;
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
        boost::optional<std::unique_ptr<IExp>> optInit;
        VariableDecl decl;
        
        VariableDeclStmt(VariableDecl decl, boost::optional<std::unique_ptr<IExp>> optInit = boost::none) :
        optInit(std::move(optInit)),
        decl(std::move(decl))  {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            if (optInit)
                return o << "VariableDeclStmt(" << decl << ", " << **optInit << ")";
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
        ast::FunctionDecl* sminfLookup;
        
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
        boost::optional<BlockStmt> optElseBlock;
        IfStmt(std::unique_ptr<IExp> condition, BlockStmt thenBlock, boost::optional<BlockStmt> optElseBlock = boost::none) :
        condition(std::move(condition)), thenBlock(std::move(thenBlock)), optElseBlock(std::move(optElseBlock)) {}
        
        virtual std::ostream& dump(std::ostream& o) const override {
            o << "IfStmt(" << *condition << ", ";
            o << thenBlock;
            if (optElseBlock) {
                o << ", " << *optElseBlock;
            }
            o << ")";
            return o;
        }
        virtual void accept(INodeVisitor& visitor) override;
    };
    
//    struct VarOpModStmt : public IStmt {
//        std::unique_ptr<ast::VariableExp> lvalue;
//        std::unique_ptr<ast::IExp> rvalue;
//        
//    };
//    
//    struct VarAssignStmt : public IStmt {
//        std::vector<ast::VariableExp> lvalues;
//        std::unique_ptr<ast::IExp> rvalue;
//        VarAssignStmt(std::vector<ast::VariableExp> lvalues, std::unique_ptr<ast::IExp> rvalue) :
//        lvalues(std::move(lvalues)), rvalue(std::move(rvalue)) {}
//    };
//    
//    struct VarIncrementStmt : public IStmt {
//        ast::VariableExp exp;
//        VarIncrementStmt(ast::VariableExp exp) :
//        exp(std::move(exp)) {}
//    };
//    
//    struct VarDecrementStmt : public IStmt {
//        ast::VariableExp exp;
//        VarDecrementStmt(ast::VariableExp exp) :
//        exp(std::move(exp)) {}
//    };
    
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