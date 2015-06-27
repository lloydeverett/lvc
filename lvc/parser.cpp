//
//  parser.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/02.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "parser.h"
#include <memory>

#warning TODO: make using this class more convenient

using namespace ast;

Parser::Parser(IReader &reader, IIssueReporter &issueReporter) :
issueReporter(issueReporter), lexerBuffer(issueReporter, reader) {
    
}

Token& Parser::readTokenIntoCurrent() {
    currentToken = lexerBuffer.readToken();
    return currentToken;
}

void Parser::reportOnCurrentToken(const std::string& message) {
    issueReporter.report(currentToken.getRow(), currentToken.getStartCol(), message, SubsystemParser);
}

Module Parser::parse(std::string nameOfModule) {

    std::vector<Function> functions;
    
    readTokenIntoCurrent();

    while (true) {
        try {
            if (currentToken.is(Eof)) break;

            if (currentToken.is(Newline)) {
                readTokenIntoCurrent();
                continue;
            }
            
            if (lexerBuffer.peekNextNext().is(OpenParenthesis)) {
                functions.push_back(parseFunction());
            }
            else {
                assert(false);
                #warning TOOD: parse global
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

    return Module(nameOfModule, std::move(functions));

}

Function Parser::parseFunction() {
    FunctionDecl decl(parseFunctionDecl());

    symbols.openVarScope();

    for (ArgumentDecl& argDecl : decl.arguments) {
        VariableDecl* varDeclPtr = &(argDecl.variableDecl);
        symbols.addVar(varDeclPtr);
    }
    
    BlockStmt block(parseBlock());
    
    symbols.closeVarScope();

    return Function(std::move(decl), std::move(block));
}

FunctionDecl Parser::parseFunctionDecl() {
    std::unique_ptr<IType> returnType(parseTypename());
    std::string identifier(parseIdentifier());

    if (currentToken.isNot(OpenParenthesis)) {
        reportOnCurrentToken("Expected '(' after identifier in function declaration.");
        throw ParserErrorException(ParserErrorExpectedOpenParenthesis);
    }
    readTokenIntoCurrent();

    std::vector<ArgumentDecl> arguments;
    if (currentToken.isNot(CloseParenthesis)) {
        while (true) {
            std::unique_ptr<IType> argType(parseTypename());
            std::string argIdentifier(parseIdentifier());

            boost::optional<std::unique_ptr<IExp>> optDefaultValue = boost::none;
            if (currentToken.is(Equals)) {
                readTokenIntoCurrent();
                optDefaultValue = parseExpression();
            }

            VariableDecl varDecl = VariableDecl(std::move(argType), argIdentifier);
            arguments.emplace_back(std::move(varDecl), std::move(*optDefaultValue));

            if (currentToken.is(CloseParenthesis)) break;
            if (currentToken.is(Comma)) {
                readTokenIntoCurrent();
                continue;
            }

            reportOnCurrentToken("Badly formed function declaration. Expected comma or ')'.");
            throw ParserErrorException(ParserErrorExpectedCommaOrCloseParenthesis);
        }
    }
    readTokenIntoCurrent(); // read token after the close parenthesis into currentToken

    return FunctionDecl(std::move(returnType), identifier, std::move(arguments));
}

boost::optional<std::unique_ptr<IType>> Parser::tryParseTypename() {
    PrimitiveTypeCode code;
    switch (currentToken.getKind()) {
        case Char: code = PrimitiveTypeCodeChar; break;
        case Short: code = PrimitiveTypeCodeShort; break;
        case Int: code = PrimitiveTypeCodeInt; break;
        case Long: code = PrimitiveTypeCodeLong; break;
        case Uchar: code = PrimitiveTypeCodeUchar; break;
        case Ushort: code = PrimitiveTypeCodeUshort; break;
        case Uint: code = PrimitiveTypeCodeUint; break;
        case Ulong: code = PrimitiveTypeCodeUlong; break;
        case Bool: code = PrimitiveTypeCodeBool; break;
        case Float: code = PrimitiveTypeCodeFloat; break;
        case Double: code = PrimitiveTypeCodeDouble; break;
        default:
            return boost::none;
    }
    readTokenIntoCurrent();
    std::unique_ptr<IType> ret = std::make_unique<PrimitiveType>(code);
    return std::move(ret);
}

std::unique_ptr<IType> Parser::parseTypename() {
    boost::optional<std::unique_ptr<IType>> opt = tryParseTypename();
    if (!opt) {
        reportOnCurrentToken("Expected typename.");
        throw ParserErrorException(ParserErrorExpectedType);
    }
    return std::move(*opt);
}

// This function parses a BlockStmt, which appears in source as either:
//  - a newline followed by an indent, and then a series of statements (possibly one) each ending in a newline, and then a terminating dedent.
//  - a single statement found on the same line that prompted the parsing of a block, terminated by a newline.
// currentToken will be the token after the terminating token in each case.
BlockStmt Parser::parseBlock() {
#warning TOOD: check this
    std::vector<std::unique_ptr<IStmt>> statements;
    if (currentToken.is(Newline)) {
        readTokenIntoCurrent();
        
        if (currentToken.isNot(Indent)) {
            reportOnCurrentToken("Expected indent after newline when parsing block.");
            throw ParserErrorException(ParserErrorExpectedIndent);
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
#warning TODO: test
    
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
    
    // Single line
    std::unique_ptr<IStmt> stmt;
    if (currentToken.is(Return)) {
        readTokenIntoCurrent();
        std::unique_ptr<IExp> exp(parseExpression());
        stmt = std::make_unique<ReturnStmt>(std::move(exp));
    }
    else {
        boost::optional<std::unique_ptr<IType>> optType = tryParseTypename();
        if (optType) { // handle declaration of local
            std::string identifier(parseIdentifier());
            boost::optional<std::unique_ptr<IExp>> optInitialValue = boost::none;
            if (currentToken.is(Equals)) {
                readTokenIntoCurrent();
                optInitialValue = parseExpression();
            }
            
            stmt = std::make_unique<VariableDeclStmt>(VariableDecl(std::move(*optType), identifier), std::move(optInitialValue));
        }
        else {
#warning TODO: impl
            assert(false);
        }
    }
    
    if (currentToken.isNot(Newline)) {
        reportOnCurrentToken("Expected newline after statement.");
        throw ParserErrorException(ParserErrorExpectedNewline);
    }
    readTokenIntoCurrent();
    return std::move(stmt);
}

std::unique_ptr<IExp> Parser::parseParenExpression() {
    if (currentToken.isNot(OpenParenthesis)) {
        reportOnCurrentToken("Expected '('");
        throw ParserErrorException(ParserErrorExpectedOpenParenthesis);
    }
    readTokenIntoCurrent();

    std::unique_ptr<IExp> exp(parseExpression());

    if (currentToken.isNot(CloseParenthesis)) {
        reportOnCurrentToken("Expected ')'");
        throw ParserErrorException(ParserErrorExpectedCloseParenthesis);
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
        return std::make_unique<BinopExp>(parseBinopRHS(std::move(leftmostPrimary), *optNextBinopCode));
    }
}

// This function takes a left-hand-side part of an expression as input and parses until the tree
// of BinopExps is discerned according to operator precedences, and it returns the BinopExp tree.
// Generally lhs is the leftmost primary expression of an expression when this is called by other functions,
// but lhs can represent any left-hand-side part of the expression (including the entire expression).
// currentToken should be the token that appears directly after the operator that follows lhs.
BinopExp Parser::parseBinopRHS(std::unique_ptr<IExp> lhs, BinopCode afterLhsBinopCode) {
    while (true) {
        int afterLhsBinopPrec = getBinopCodePrecedence(afterLhsBinopCode);
        
        // For now, just fill rhs with the primary expression after lhs.
        std::unique_ptr<IExp> rhs(parsePrimaryExpression());
        
        // We check to see whether there's a binary operator *after* the primary expression in rhs.
        // If not, we're finished.
        boost::optional<BinopCode> optNextBinopCode(tryParseBinopCode());
        if (!optNextBinopCode) {
            return BinopExp(std::move(lhs), afterLhsBinopCode, std::move(rhs));
        }
        BinopCode nextBinopCode = *optNextBinopCode;
        int nextBinopPrec = getBinopCodePrecedence(nextBinopCode);
        
        // If afterLhsBinopCode binds less tightly with RHS than the operator after LHS,
        // let the pending operator take RHS as its LHS.
        if (nextBinopPrec > afterLhsBinopPrec) {
            rhs = std::make_unique<BinopExp>(parseBinopRHS(std::move(rhs), nextBinopCode));
            
            // Now that rhs has been changed, we need to update nextBinopCode.
            // We don't have to update nextBinopPrec because that's determined in the
            // beginning of the loop body.
            // Again, if there's no remaining operator, we're finished.
            optNextBinopCode = tryParseBinopCode();
            if (!optNextBinopCode) {
                return BinopExp(std::move(lhs), afterLhsBinopCode, std::move(rhs));
            }
            nextBinopCode = *optNextBinopCode;
        }
        
        // Combine LHS and RHS into LHS and repeat the process.
        lhs = std::make_unique<BinopExp>(std::move(lhs), afterLhsBinopCode, std::move(rhs));
        afterLhsBinopCode = nextBinopCode;
    }
}

// A primary expression is an expression that is not a BinopExp.
// This dinstinction is important because BinopExps require operator precedence parsing.
// Note that parseExpression can still parse primary expressions that appear without BinopExps; this function
// is called by it.
std::unique_ptr<IExp> Parser::parsePrimaryExpression() {
    if (currentToken.is(Identifier)) {
        if (lexerBuffer.peekNext().is(OpenParenthesis)) { // function call
            readTokenIntoCurrent(); // read open parenthesis into currentToken
            readTokenIntoCurrent(); // read token after open parenthesis into currentToken
            std::vector<std::unique_ptr<IExp>> argExps;
            if (currentToken.isNot(CloseParenthesis)) {
                while (true) {
                    argExps.push_back(parseExpression());
#warning TODO: lookup symbol for function

                    if (currentToken.is(CloseParenthesis)) break;
                    if (currentToken.is(Comma)) {
                        readTokenIntoCurrent();
                        continue;
                    }

                    reportOnCurrentToken("Expected comma or ')' in argument list.");
                    throw ParserErrorException(ParserErrorExpectedCommaOrCloseParenthesis);
                }
            }
            readTokenIntoCurrent(); // read token after close parenthesis -> currentToken

        }
        else { // variable expression
            
        }
    }
    else if (currentToken.is(IntegerLiteral)) {
        std::string val = currentToken.getStr();
        readTokenIntoCurrent();
        return std::make_unique<IntegerLiteralExp>(val);
    }
    else if (currentToken.is(RealLiteral)) {
        std::string val = currentToken.getStr();
        readTokenIntoCurrent();
        return std::make_unique<RealLiteralExp>(val);
    }
    else if (currentToken.is(OpenParenthesis)) {
        return parseParenExpression();
    }
#warning TODO: impl
    assert(false);
}

std::string Parser::parseIdentifier() {
    if (currentToken.isNot(Identifier)) {
        reportOnCurrentToken("Expected identifier.");
        throw ParserErrorException(ParserErrorExpectedIdentifier);
    }
    std::string s = currentToken.getStr();
    readTokenIntoCurrent();
    return s;
}

boost::optional<BinopCode> Parser::tryParseBinopCode() {
    BinopCode code;
    switch (currentToken.getKind()) {
        case Plus: code = BinopCodeAdd; break;
        case Minus: code = BinopCodeSubtract; break;
        case Asterisk: code = BinopCodeMultiply; break;
        case Slash: code = BinopCodeDivide; break;
        default:
            return boost::none;
    }
    readTokenIntoCurrent();
    return code;
}

int Parser::getBinopCodePrecedence(BinopCode code) {
    switch (code) {
        case BinopCodeAdd:      return 10;
        case BinopCodeSubtract: return 10;
        case BinopCodeMultiply: return 20;
        case BinopCodeDivide:   return 20;
    }
    assert(false);
}
