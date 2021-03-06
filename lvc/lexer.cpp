//
//  lexer.cpp
//  lvc
//
//  Created by Lloyd Everett on 2015/06/04.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#include <cassert>
#include "lexer.h"
#include "lexerexceptions.h"

bool isAlpha(char c) {
    return c >= 'A' && c <= 'z';
}

bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

Lexer::Lexer(IReader &reader, IIssueReporter& issueReporter) : reader(reader), issueReporter(issueReporter) {
    indentStack.push(0);
    hasProducedEof = false;
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

bool Lexer::tryToSkipComment() {
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
                throw LexerErrorExceptionUnterminatedBlockComment();
            }
        }
    }
    else {
        return false;
    }
}

void Lexer::skipCommentsAndNonIndentWhitespace() {
    bool didSkipComment;
    bool didConsumeWhitespace;
    do {
        didSkipComment = tryToSkipComment();

        didConsumeWhitespace = false;
        if (reader.atWhitespace() && !reader.atStartOfRow()) {
            reader.consumeWhitespace();
            didConsumeWhitespace = true;
        }
    } while (didSkipComment || didConsumeWhitespace);
}

bool Lexer::isFinished() {
    return hasProducedEof;
}

Token lexStrBegginningWithAlpha(rownumber row, colnumber startCol, const std::string &str) {
    Token t;
    t.setRow(row);
    t.setStartCol(startCol);
    t.setLength(str.length());

    if (str == "return") t.setKind(Return);
    else if (str == "true") t.setKind(True);
    else if (str == "false") t.setKind(False);
    else if (str == "const") t.setKind(Const);
    else if (str == "for") t.setKind(For);
    else if (str == "if") t.setKind(If);
    else if (str == "else") t.setKind(Else);
    else if (str == "struct") t.setKind(Struct);
    else if (str == "void") t.setKind(Void);
    else if (str == "char") t.setKind(Char);
    else if (str == "short") t.setKind(Short);
    else if (str == "int") t.setKind(Int);
    else if (str == "long") t.setKind(Long);
    else if (str == "uchar") t.setKind(Uchar);
    else if (str == "ushort") t.setKind(Ushort);
    else if (str == "uint") t.setKind(Uint);
    else if (str == "ulong") t.setKind(Ulong);
    else if (str == "float") t.setKind(Float);
    else if (str == "double") t.setKind(Double);
    else if (str == "bool") t.setKind(Bool);
    else if (str == "extern") t.setKind(Extern);
    else {
        t.setKind(Identifier);
        t.setStr(str);
    }
    return t;
}

boost::optional<Token> Lexer::tryHandleNewline() {
    Token t;
    t.setStartCol(reader.getCol());
    t.setRow(reader.getRow());
    t.setKind(Newline);
    if (reader.peekChar() == '\n') {
        reader.readChar();
        t.setLength(1);
        return t;
    }
    else if (reader.peekChar() == '\r') {
        reader.readChar();
        if (reader.peekChar() == '\n') {
            reader.readChar();
            t.setLength(2);
        }
        else {
            t.setLength(1);
        }
        return t;
    }
    else {
        return boost::none;
    }
}

Token Lexer::lexToken() {
    // Return a queued dedent if there are any.
    // This should always get priority over other tokens.
    if (queuedDedents.size() > 0) {
        QueuedDedent q = queuedDedents.front();
        queuedDedents.pop();
        return getTokenFromQueuedDedent(q);
    }
    
start_after_checking_queued_dedents:
    
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
                throw LexerErrorExceptionInvalidDedent();
            }
            // Pop one off the queue
            QueuedDedent q = queuedDedents.front();
            queuedDedents.pop();
            return getTokenFromQueuedDedent(q);
        }
    }
    
    skipCommentsAndNonIndentWhitespace();
    
    boost::optional<Token> optNewlineToken(tryHandleNewline());
    if (optNewlineToken) {
        if (wasAtStartOfRowBeforeConsumingWhitespace) {
            // We found a line with just an indent, meaning we skip it.
            goto start_after_checking_queued_dedents;
        }
        else {
            return *optNewlineToken;
        }
    }
    
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
            if (!hasProducedEof) {
                hasProducedEof = true;
                Token t;
                t.setKind(Eof);
                t.setRow(reader.getRow());
                t.setStartCol(reader.getCol());
                t.setLength(0);
                return t;
            }
            else {
                throw LexerFinishedException();
            }
        }
    }
    
    colnumber rowOfC = reader.getRow();
    colnumber colOfC = reader.getCol();
    char c = reader.readChar();
    
    Token t;
    t.setRow(rowOfC);
    t.setStartCol(colOfC);
    t.setLength(1);
    
    switch (c) {
        case '(': t.setKind(OpenParenthesis); return t;
        case ')': t.setKind(CloseParenthesis); return t;
        case '*':
            if (reader.peekChar() == '=') {
                reader.readChar();
                t.setLength(2);
                t.setKind(AsteriskEquals);
            }
            else
                t.setKind(Asterisk);
            return t;
        case '/':
            if (reader.peekChar() == '=') {
                reader.readChar();
                t.setLength(2);
                t.setKind(SlashEquals);
            }
            else
                t.setKind(Slash);
            return t;
        case '.': t.setKind(Dot); return t;
        case ',': t.setKind(Comma); return t;
        case '%':
            if (reader.peekChar() == '=') {
                reader.readChar();
                t.setLength(2);
                t.setKind(PercentEquals);
            }
            else
                t.setKind(Percent);
            return t;
        case '+':
            if (reader.peekChar() == '+') {
                reader.readChar();
                t.setLength(2);
                t.setKind(PlusPlus);
            }
            else if (reader.peekChar() == '=') {
                reader.readChar();
                t.setLength(2);
                t.setKind(PlusEquals);
            }
            else
                t.setKind(Plus);
            return t;
        case '-':
            if (reader.peekChar() == '-') {
                reader.readChar();
                t.setLength(2);
                t.setKind(MinusMinus);
            }
            else if (reader.peekChar() == '=') {
                reader.readChar();
                t.setLength(2);
                t.setKind(MinusEquals);
            }
            else
                t.setKind(Minus);
            return t;
        case '=':
            if (reader.peekChar() == '=') {
                reader.readChar();
                t.setLength(2);
                t.setKind(EqualsEquals);
            }
            else
                t.setKind(Equals);
            return t;
        case '<':
            if (reader.peekChar() == '=') {
                reader.readChar();
                t.setLength(2);
                t.setKind(SmalerThanEquals);
            }
            else
                t.setKind(SmallerThan);
            return t;
        case '>':
            if (reader.peekChar() == '=') {
                reader.peekChar();
                t.setLength(2);
                t.setKind(LargenThanEquals);
            }
            else
                t.setKind(LargerThan);
            return t;
        case '!':
            if (reader.peekChar() == '=') {
                reader.readChar();
                t.setKind(ExclamationEquals);
            }
            else
                t.setKind(Exclamation);
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
        unsigned int numDecimalPoints = 0;
        while (isDigit(reader.peekChar()) || reader.peekChar() == '.') {
            if (reader.peekChar() == '.')
                numDecimalPoints++;
            str += reader.readChar();
        }
        if (numDecimalPoints > 1) {
            issueReporter.report(reader.getSourcePosition(), "Multiple decimal points in number literal.", SubsystemLexer);
            throw LexerErrorExceptionInvalidNumberLiteral();
        }
        if (isAlpha(reader.peekChar())) {
            issueReporter.report(reader.getSourcePosition(), "Did not expect alphabet character in number literal.", SubsystemLexer);
            throw LexerErrorExceptionInvalidNumberLiteral();
        }
        t.setKind(NumberLiteral);
        t.setNumberLiteralHasDecimalPoint(numDecimalPoints == 1);
        t.setLength(str.length());
        t.setStr(str);
        return t;
    }
    
    issueReporter.report(reader.getSourcePosition(), "Did not expect character.", SubsystemLexer);
    throw LexerErrorExceptionUnknownCharacter();
}
