//
//  lexer.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/04.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include "lexer.h"
#include "integertypedefs.h"
#include <cassert>

bool isAlpha(char c) {
    return c >= 'A' && c <= 'z';
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool doesStrBeginWith(const std::string& str, const std::string& with) {
    if (str.length() < with.length())
        return false;
    
    return str.substr(0, with.length()) == with;
}

Lexer::Lexer(IReader &reader) : reader(reader) {
    indentStack.push(0);
}

Token Lexer::getTokenFromQueuedDedent(QueuedDedent q) {
    Token t;
    t.setKind(Dedent);
    t.setStartCol(q.startCol);
    t.setLength(q.length);
    t.setRow(reader.getRow());
    return t;
}

bool Lexer::addDedentsToQueueUntilColnumberIsReached(colnumber col) {
    assert(indentStack.top() > col);
    
    colnumber currentCol;
    do {
        colnumber prevCol = indentStack.top();
        indentStack.pop();
        currentCol = indentStack.top();
        
        colnumber dedentStartCol = currentCol;
        colnumber dedentEndCol = prevCol;
        queuedDedents.emplace(dedentStartCol, dedentEndCol - dedentStartCol);
    } while (currentCol > col);
    
    if (col != indentStack.top()) {
        return false;
    }
    
    return true;
}

Token Lexer::makeIndentToken(colnumber col) {
    assert(col > indentStack.top());
    
    colnumber origIndent = indentStack.top();
    indentStack.push(col);
    Token t;
    t.setKind(Indent);
    t.setRow(reader.getRow());
    t.setStartCol(origIndent);
    t.setLength(col - origIndent);
    return t;
}

bool Lexer::tryToSkipComment(IIssueReporter &issueReporter) {
    if (reader.peekChar(0) == '/' && reader.peekChar(1) == '/') {
        reader.consumeUntilPositionAtNewlineOrEof();
        return true;
    }
    else if (reader.peekChar(0) == '/' && reader.peekChar(1) == '*') {
        rownumber startRow = reader.getRow();
        colnumber startCol = reader.getCol();
        reader.readChar(); reader.readChar();
        while (true) {
            char c = reader.readChar();
            if (c == '*') {
                c = reader.readChar();
                if (c == '/') {
                    return true;
                }
            }
            if (reader.eof()) {
                issueReporter.report(startRow, startCol, "Block comment that starts here was never terminated.", SubsystemLexer);
                throw LexerErrorException(LexerErrorUnterminatedBlockComment);
            }
        }
    }
    else {
        return false;
    }
}

void Lexer::skipCommentsAndNonIndentWhitespace(IIssueReporter &issueReporter) {
    bool didSkipComment;
    bool didConsumeWhitespace;
    do {
        didSkipComment = tryToSkipComment(issueReporter);
        
        didConsumeWhitespace = false;
        if (reader.atWhitespace() && !reader.atStartOfRow()) {
            reader.consumeWhitespace();
            didConsumeWhitespace = true;
        }
    } while (didSkipComment || didConsumeWhitespace);
}

bool Lexer::isFinished(IIssueReporter &issueReporter) {
    skipCommentsAndNonIndentWhitespace(issueReporter);
    return reader.eof() && indentStack.top() == 0 && queuedDedents.size() == 0;
}

Token lexStrBegginningWithAlpha(rownumber row, colnumber startCol, const std::string &str) {
    Token t;
    t.setRow(row);
    t.setStartCol(startCol);
    t.setLength(str.length());
    
    if (str == "return") t.setKind(Return);
    else if (str == "if") t.setKind(If);
    else if (str == "struct") t.setKind(Struct);
    else if (str == "char") t.setKind(Char);
    else if (str == "short") t.setKind(Short);
    else if (str == "int") t.setKind(Int);
    else if (str == "long") t.setKind(Long);
    else if (str == "uchar") t.setKind(Uchar);
    else if (str == "ushort") t.setKind(Ushort);
    else if (str == "uint") t.setKind(Uint);
    else if (str == "ulong") t.setKind(Ulong);
    else {
        t.setKind(Identifier);
        t.setStr(str);
    }
    return t;
}

Token Lexer::lexToken(IIssueReporter &issueReporter) {
    
    // Return a queued dedent if there are any.
    // This should always get priority over other tokens
    // (and thus the code is run first).
    if (queuedDedents.size() > 0) {
        QueuedDedent q = queuedDedents.front();
        queuedDedents.pop();
        return getTokenFromQueuedDedent(q);
    }
    
    bool wasAtStartOfRowBeforeConsumingWhitespace = reader.atStartOfRow();
    charcount whitespaceCount = reader.consumeWhitespace();
    
    if (wasAtStartOfRowBeforeConsumingWhitespace) {
        if (whitespaceCount > indentStack.top()) {
            return makeIndentToken(whitespaceCount);
        }
        else if (whitespaceCount < indentStack.top()) {
            bool success = addDedentsToQueueUntilColnumberIsReached(whitespaceCount);
            if (!success) {
                issueReporter.report(reader.getSourcePosition(), "Unindent does not match any indent level.", SubsystemLexer);
                throw LexerErrorException(LexerErrorInvalidDedent);
            }
            // Pop one off the queue
            QueuedDedent q = queuedDedents.front();
            queuedDedents.pop();
            return getTokenFromQueuedDedent(q);
        }
    }
    
    skipCommentsAndNonIndentWhitespace(issueReporter);
    
    if (reader.eof()) {
        if (indentStack.top() > 0) {
            bool success = addDedentsToQueueUntilColnumberIsReached(0);
            assert(success); // should never fail
            // Pop one off the queue
            QueuedDedent q = queuedDedents.front();
            queuedDedents.pop();
            return getTokenFromQueuedDedent(q);
        }
        else {
            throw LexerFinishedException();
        }
    }
    
    colnumber rowOfC = reader.getRow();
    colnumber colOfC = reader.getCol();
    char c = reader.readChar();
    
    Token t;
    t.setRow(rowOfC);
    t.setStartCol(colOfC);
    t.setLength(1);
    
    if (c == '\n' || c == '\r') {
        if (c == '\r') {
            if (reader.peekChar() == '\n') {
                reader.readChar();
                t.setLength(2);
            }
        }
        t.setKind(Newline);
        return t;
    }
    if (c == '(') {
        t.setKind(OpenParenthesis);
        return t;
    }
    if (c == ')') {
        t.setKind(CloseParenthesis);
        return t;
    }
    if (c == '>') {
        if (reader.peekChar() == '=') {
            reader.readChar();
            t.setKind(LargenThanEquals);
        }
        else {
            t.setKind(LargerThan);
        }
        return t;
    }
    if (c == '<') {
        if (reader.peekChar() == '=') {
            reader.readChar();
            t.setKind(SmalerThanEquals);
        }
        else {
            t.setKind(SmallerThan);
        }
        return t;
    }
    if (c == '=') {
        if (reader.peekChar() == '=') {
            reader.readChar();
            t.setKind(EqualsEquals);
        }
        else {
            t.setKind(Equals);
        }
        return t;
    }
    if (c == '!') {
        if (reader.peekChar() == '=') {
            reader.readChar();
            t.setKind(ExclamationEquals);
        }
        else {
            t.setKind(Exclamation);
        }
        return t;
    }
    if (c == '.') {
        t.setKind(Dot);
        return t;
    }
    if (c == ',') {
        t.setKind(Comma);
        return t;
    }
    
    if (isAlpha(c)) {
        std::string str;
        str += c;
        while (isAlpha(reader.peekChar()) || isDigit(reader.peekChar())) {
            str += reader.readChar();
        }
        
        return lexStrBegginningWithAlpha(rowOfC, colOfC, str);
    }
    
    if (isDigit(c)) {
        std::string str;
        str += c;
        while (isDigit(reader.peekChar())) {
            str += reader.readChar();
        }
        if (isAlpha(reader.peekChar())) {
            issueReporter.report(reader.getSourcePosition(), "Did not expect alphabet character in number literal.", SubsystemLexer);
            throw LexerErrorException(LexerErrorInvalidNumberLiteral);
        }
        t.setKind(IntegerLiteral);
        t.setLength(str.length());
        t.setStr(str);
        return t;
    }
    
    issueReporter.report(reader.getSourcePosition(), "Did not expect character", SubsystemLexer);
    throw LexerErrorException(LexerErrorUnexpectedCharacter);
}

bool Lexer::attemptToRecoverBySkippingLine() {
    return (assert(false), true);
}

bool Lexer::attemptToRecoverBySkippingLinesUntilValidIndentation() {
    return (assert(false), true);
}
