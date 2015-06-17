//
//  ast.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/17.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "primitivetypeenum.h"
#include <vector>
#include <memory>
#include <string>
#include <iostream>

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
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << std::string(stringFromPrimitiveTypeEnum(name)) << "wtf";
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
    };
    
    struct VariableExp : public IExp {
        VariableDecl &decl;
        VariableExp(VariableDecl &decl) :
        decl(decl) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << "(VariableExp: " << decl << ")";
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct ArgumentDecl : public IDecl {
        std::unique_ptr<IType> type_ptr;
        std::unique_ptr<IExp> defaultValue_ptr;
        std::string identifier;
        ArgumentDecl(std::unique_ptr<IType> type_ptr, std::string identifier, std::unique_ptr<IExp> defaultValue) :
        type_ptr(std::move(type_ptr)), defaultValue_ptr(std::move(defaultValue_ptr)),
        identifier(identifier) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << "(ArgumentDecl: " << *type_ptr << ", " << identifier << ")";
        }
        
        virtual void accept(INodeVisitor &visitor) override;
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
    };
    
    struct VariableDeclStmt : public IStmt {
        std::unique_ptr<IExp> initialValue_ptr;
        VariableDecl decl;
        
        VariableDeclStmt(VariableDecl decl, std::unique_ptr<IExp> initialValue_ptr) :
        initialValue_ptr(std::move(initialValue_ptr)),
        decl(std::move(decl))  {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << "(VariableDeclStmt: " << decl << ", " << *initialValue_ptr << ")";
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct ReturnStmt : public IStmt {
        std::unique_ptr<IExp> returnedExpression_ptr;
        ReturnStmt(std::unique_ptr<IExp> returnedExpression_ptr) :
        returnedExpression_ptr(std::move(returnedExpression_ptr)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << "(ReturnStmt: " << *returnedExpression_ptr << ")";
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
    
    struct BinOpExp : public IExp {
        char op;
        std::unique_ptr<IExp> lhs;
        std::unique_ptr<IExp> rhs;
        BinOpExp(char op, std::unique_ptr<IExp> lhs, std::unique_ptr<IExp> rhs) :
        op(op), lhs(std::move(lhs)), rhs(std::move(rhs)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            return o << "(BinOpExp: " << *lhs << ", " << *rhs << ")";
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
    struct FunctionCallExp : public IExp {
        const FunctionDecl &calleeDeclRef;
        std::vector<std::unique_ptr<IExp>> passedArguments;
        FunctionCallExp(const FunctionDecl &calleeDeclRef, std::vector<std::unique_ptr<IExp>> passedArguments) :
        calleeDeclRef(calleeDeclRef), passedArguments(std::move(passedArguments)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            o << "(FunctionCallExp: )" << calleeDeclRef << ", (Arguments: ";
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
    
    ///////////////////////////////
    
    struct Function : public INode {
        FunctionDecl decl;
        std::vector<std::unique_ptr<IStmt>> statements;
        Function(FunctionDecl decl, std::vector<std::unique_ptr<IStmt>> statements) :
        decl(std::move(decl)), statements(std::move(statements)) {}
        
        virtual std::ostream& dump(std::ostream& o) const {
            o << "Function" << std::endl;
            o << decl << std::endl;
            for (const std::unique_ptr<IStmt>& statement_ptr : statements) {
                o << *statement_ptr << std::endl;
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
            o << "Module " << moduleName << std::endl;
            for (const Function& function : functions) {
                o << function;
            }
            return o;
        }
        
        virtual void accept(INodeVisitor &visitor) override;
    };
    
}
