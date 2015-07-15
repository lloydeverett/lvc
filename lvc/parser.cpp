//
//  parser.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "parser.h"
#include "lexerexceptions.h"
#include "itokeninputstream.h"
#include <memory>

using namespace ast;

Parser::Parser(ITokenInputStream& inputStream, IIssueReporter& issueReporter) :
issueReporter(issueReporter), inputStream(inputStream) {
    
}

void Parser::readTokenIntoCurrent() {
    currentToken = inputStream.readToken();
}

void Parser::reportOnCurrentToken(const std::string& message) {
    issueReporter.report(currentToken.getRow(), currentToken.getStartCol(), message, SubsystemParser);
}

Module Parser::parseModule(std::string name) {
    std::vector<Function> functions;
    std::vector<VariableDeclStmt> variableDecls;
    
    readTokenIntoCurrent();

    while (true) {
        try {
            if (currentToken.is(Eof)) break;

            if (currentToken.is(Newline)) {
                readTokenIntoCurrent();
                continue;
            }
            
            if (inputStream.peekNextNext().is(OpenParenthesis)) {
                functions.push_back(parseFunction());
            }
            else {
                boost::optional<std::unique_ptr<IType>> optType(tryParseType());
                if (!optType) {
                    reportOnCurrentToken("Did not expect token at beginning of line when parsing at module level.");
                    throw ParserErrorExceptionUnknownLineBeginningAtModuleLevel();
                }
                variableDecls.push_back(parseVariableDeclStmt(std::move(*optType)));
            }
        }
        catch (LexerException &e) {
            #warning TODO: implement this
            assert(false);
        }
        catch (LexerFinishedException &e) {
            assert(false);
        }
    }

    return Module(name, std::move(functions), std::move(variableDecls));
}

Function Parser::parseFunction() {
    FunctionDecl decl(parseFunctionDecl());
    BlockStmt block(parseBlock());

    return Function(std::move(decl), std::move(block));
}

FunctionDecl Parser::parseFunctionDecl() {
    std::unique_ptr<IType> returnType(parseType());
    std::string identifier(parseIdentifier());

    if (currentToken.isNot(OpenParenthesis)) {
        reportOnCurrentToken("Expected '(' after identifier in function declaration.");
        throw ParserErrorExceptionExpectedOpenParenthesis();
    }
    readTokenIntoCurrent();

    std::vector<ArgumentDecl> arguments;
    if (currentToken.isNot(CloseParenthesis)) {
        while (true) {
            std::unique_ptr<IType> argType(parseType());
            std::string argIdentifier(parseIdentifier());

            boost::optional<std::unique_ptr<IExp>> optDefaultValue = boost::none;
            if (currentToken.is(Equals)) {
                readTokenIntoCurrent();
                optDefaultValue = parseExpression();
            }

            VariableDecl varDecl = VariableDecl(std::move(argType), argIdentifier);
            arguments.emplace_back(std::move(varDecl), std::move(optDefaultValue));

            if (currentToken.is(CloseParenthesis)) break;
            if (currentToken.is(Comma)) {
                readTokenIntoCurrent();
                continue;
            }

            reportOnCurrentToken("Badly formed function declaration. Expected comma or ')'.");
            throw ParserErrorExceptionExpectedCommaOrCloseParenthesis();
        }
    }
    readTokenIntoCurrent(); // read token after the close parenthesis into currentToken

    return FunctionDecl(std::move(returnType), identifier, std::move(arguments));
}

boost::optional<std::unique_ptr<IType>> Parser::tryParseType() {
    Constantness isConst;
    if (currentToken.is(Const)) {
        isConst = Constant;
        readTokenIntoCurrent();
    }
    else {
        isConst = NotConstant;
    }
    
    boost::optional<std::unique_ptr<IType>> optRet;
    switch (currentToken.getKind()) {
        case Void:
            optRet = std::make_unique<VoidType>(isConst); break;
        case Char:
            optRet = std::make_unique<IntegerType>(8, ast::IntegerType::Signed, isConst); break;
        case Short:
            optRet = std::make_unique<IntegerType>(16, ast::IntegerType::Signed, isConst); break;
        case Int:
            optRet = std::make_unique<IntegerType>(32, ast::IntegerType::Signed, isConst); break;
        case Long:
            optRet = std::make_unique<IntegerType>(64, ast::IntegerType::Signed, isConst); break;
        case Uchar:
            optRet = std::make_unique<IntegerType>(8, ast::IntegerType::Unsigned, isConst); break;
        case Ushort:
            optRet = std::make_unique<IntegerType>(16, ast::IntegerType::Unsigned, isConst); break;
        case Uint:
            optRet = std::make_unique<IntegerType>(32, ast::IntegerType::Unsigned, isConst); break;
        case Ulong:
            optRet = std::make_unique<IntegerType>(64, ast::IntegerType::Unsigned, isConst); break;
        case Float:
            optRet = std::make_unique<FloatingPointType>(ast::FloatingPointType::VariationFloat, isConst);
            break;
        case Double:
            optRet = std::make_unique<FloatingPointType>(ast::FloatingPointType::VariationDouble, isConst);
            break;
        default:
            if (isConst) {
                reportOnCurrentToken("Const specifier is not followed by a typename.");
                throw ParserErrorExceptionQualifierWithoutTypename();
            }
            else {
                return boost::none;
            }
    }
    readTokenIntoCurrent();
    return std::move(optRet);
}

std::unique_ptr<IType> Parser::parseType() {
    boost::optional<std::unique_ptr<IType>> opt = tryParseType();
    if (!opt) {
        reportOnCurrentToken("Expected typename.");
        throw ParserErrorExceptionExpectedType();
    }
    return std::move(*opt);
}

// This function parses a BlockStmt, which appears in source as either:
//  - a newline followed by an indent, and then a series of statements (possibly one) each ending in a newline, and then a terminating dedent.
//  - a single statement found on the same line that prompted the parsing of a block, terminated by a newline.
// currentToken will be the token after the terminating token in each case after the call.
BlockStmt Parser::parseBlock() {
    std::vector<std::unique_ptr<IStmt>> statements;
    if (currentToken.is(Newline)) {
        readTokenIntoCurrent();
        
        if (currentToken.isNot(Indent)) {
            reportOnCurrentToken("Expected indent after newline when parsing block.");
            throw ParserErrorExceptionExpectedIndent();
        }
        readTokenIntoCurrent();
        
        assert(currentToken.isNot(Dedent)); // dedent directly after indent should be impossible
        do {
            statements.push_back(parseStatement());
        } while (currentToken.isNot(Dedent));
        readTokenIntoCurrent(); // consume the dedent
    }
    else {
        statements.push_back(parseStatement());
    }
    return BlockStmt(std::move(statements));
}

// After this function is called, currentToken will be the token after the newline that terminates the statement.
std::unique_ptr<IStmt> Parser::parseStatement() {
    // Block statements
    if (currentToken.is(If)) {
        readTokenIntoCurrent();
        std::unique_ptr<IExp> condition(parseParenExpression());
        BlockStmt thenBlock(parseBlock());
        boost::optional<BlockStmt> elseBlock = boost::none;
        if (currentToken.is(Else)) {
            readTokenIntoCurrent();
            elseBlock = parseBlock();
        }
        return std::make_unique<IfStmt>(std::move(condition), std::move(thenBlock), std::move(elseBlock));
    }
    // Single line (we read the newline afterwards in this case).
    std::unique_ptr<IStmt> stmt;
    if (currentToken.is(Return)) {
        readTokenIntoCurrent();
        std::unique_ptr<IExp> exp(parseExpression());
        stmt = std::make_unique<ReturnStmt>(std::move(exp));
    }
    else {
        boost::optional<std::unique_ptr<IType>> optType = tryParseType();
        if (optType) {
            VariableDeclStmt vds(parseVariableDeclStmt(std::move(*optType)));
            stmt = std::make_unique<VariableDeclStmt>(std::move(vds));
        }
        else {
            reportOnCurrentToken("Could not parse statement begginning with this token.");
            throw ParserErrorExceptionUnknownStatementBeginning();
        }
    }
    if (currentToken.isNot(Newline)) {
        reportOnCurrentToken("Expected newline after statement.");
        throw ParserErrorExceptionExpectedNewline();
    }
    readTokenIntoCurrent();
    return std::move(stmt);
}

VariableDeclStmt Parser::parseVariableDeclStmt(std::unique_ptr<IType> type) {
    std::string identifier(parseIdentifier());
    boost::optional<std::unique_ptr<IExp>> optInitialValue = boost::none;
    if (currentToken.is(Equals)) {
        readTokenIntoCurrent();
        optInitialValue = parseExpression();
    }
    VariableDecl decl(std::move(type), identifier);
    return VariableDeclStmt(std::move(decl), std::move(optInitialValue));
}

std::unique_ptr<IExp> Parser::parseParenExpression() {
    if (currentToken.isNot(OpenParenthesis)) {
        reportOnCurrentToken("Expected '('");
        throw ParserErrorExceptionExpectedOpenParenthesis();
    }
    readTokenIntoCurrent();

    std::unique_ptr<IExp> exp(parseExpression());

    if (currentToken.isNot(CloseParenthesis)) {
        reportOnCurrentToken("Expected ')'");
        throw ParserErrorExceptionExpectedCloseParenthesis();
    }
    readTokenIntoCurrent();

    return std::move(exp);
}

std::unique_ptr<IExp> Parser::parseExpression() {
    std::unique_ptr<IExp> leftmostPrimary(parsePrimaryExpression());
    boost::optional<BinopCode> optNextBinopCode(tryParseBinopCode());
    if (!optNextBinopCode) {
        return std::move(leftmostPrimary);
    }
    else {
        return parseBinopRhs(std::move(leftmostPrimary), *optNextBinopCode, 0);
    }
}

// This function takes a left-hand-side part of an expression as input and parses until the tree
// of BinopExps is discerned according to operator precedences, and it returns the BinopExp tree.
// Generally lhs is the leftmost primary expression of an expression when this is called by other functions,
// but lhs can represent any left-hand-side part of the expression.
// currentToken should be the token that appears directly after the operator that follows lhs when this function is called.
// If afterLhsCode's precedence is lower than minPrecedence, lhs is returned.
std::unique_ptr<IExp> Parser::parseBinopRhs(std::unique_ptr<IExp> lhs, BinopCode afterLhsCode, int minPrecedence) {
    while (true) {
        int afterLhsPrec = getBinopCodePrecedence(afterLhsCode);
        
        if (afterLhsPrec < minPrecedence) {
            return std::move(lhs);
        }
        
        std::unique_ptr<IExp> rhs(parsePrimaryExpression());
        
        boost::optional<BinopCode> optAfterRhsCode(tryParseBinopCode());
        if (!optAfterRhsCode) {
            return std::make_unique<BinopExp>(std::move(lhs), afterLhsCode, std::move(rhs));
        }
        BinopCode afterRhsCode = *optAfterRhsCode;
        int afterRhsPrec = getBinopCodePrecedence(afterRhsCode);
        if (afterLhsPrec < afterRhsPrec) {
            // Let afterRhsCode take rhs as its lhs
            rhs = parseBinopRhs(std::move(rhs), afterRhsCode, afterLhsPrec + 1);
            // RHS has changed so we need to adjust afterRhsCode
            optAfterRhsCode = tryParseBinopCode();
            if (!optAfterRhsCode) {
                return std::make_unique<BinopExp>(std::move(lhs), afterLhsCode, std::move(rhs));
            }
            afterRhsCode = *optAfterRhsCode;
            // Don't need to update afterRhsPrec; it isn't used again.
        }
        
        // Now we take what we've parsed and merge it into the LHS and then
        // run this function again; we set the arguments for the next iteration.
        lhs = std::make_unique<BinopExp>(std::move(lhs), afterLhsCode, std::move(rhs)); // merge LHS and RHS into LHS
        afterLhsCode = afterRhsCode; // adjust afterLhsCode according to updated LHS
        // We don't change minPrecedence
    }
}

// A primary expression is an expression that is not a BinopExp.
// This dinstinction is important because BinopExps require operator precedence parsing.
// Note that parseExpression can still parse primary expressions that appear without BinopExps; this function
// is called by it.
// This includes parenthesis expressions.
std::unique_ptr<IExp> Parser::parsePrimaryExpression() {
    if (currentToken.is(Identifier)) {
        if (inputStream.peekNext().is(OpenParenthesis)) { // function call
            std::string identifier = parseIdentifier();
            readTokenIntoCurrent(); // read token after open parenthesis into currentToken
            std::vector<std::unique_ptr<IExp>> argExps;
            if (currentToken.isNot(CloseParenthesis)) {
                while (true) {
                    argExps.push_back(parseExpression());

                    if (currentToken.is(CloseParenthesis)) break;
                    if (currentToken.is(Comma)) {
                        readTokenIntoCurrent();
                        continue;
                    }

                    reportOnCurrentToken("Expected comma or ')' in argument list.");
                    throw ParserErrorExceptionExpectedCommaOrCloseParenthesis();
                }
            }
            readTokenIntoCurrent(); // read token after close parenthesis into currentToken
            
            return std::make_unique<CallExp>(identifier, std::move(argExps));
        }
        else { // variable expression
            return std::make_unique<VariableExp>(parseIdentifier());
        }
    }
    else if (currentToken.is(IntegerLiteral)) {
        std::string val = currentToken.getStr();
        readTokenIntoCurrent();
        return std::make_unique<NumberLiteralExp>(val);
    }
    else if (currentToken.is(RealLiteral)) {
        std::string val = currentToken.getStr();
        readTokenIntoCurrent();
        return std::make_unique<NumberLiteralExp>(val);
    }
    else if (currentToken.is(OpenParenthesis)) {
        return parseParenExpression();
    }
    
    reportOnCurrentToken("Cannot parse expression beginning with this token.");
    throw ParserErrorExceptionUnknownExpressionBeginning();
}

std::string Parser::parseIdentifier() {
    if (currentToken.isNot(Identifier)) {
        reportOnCurrentToken("Expected identifier.");
        throw ParserErrorExceptionExpectedIdentifier();
    }
    std::string s = currentToken.getStr();
    readTokenIntoCurrent();
    return s;
}

boost::optional<BinopCode> Parser::tryParseBinopCode() {
    boost::optional<BinopCode> optBinopCode = tryGetBinopCodeFromToken(currentToken);
    if (optBinopCode)
        readTokenIntoCurrent();
    return optBinopCode;
}
