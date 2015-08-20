//
//  sminf.h
//  lvc
//
//  Created by Lloyd Everett on 2015/08/01.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once

namespace ast {
    class IType;
}

class SMInf {
public:
    ast::IType* from;
    enum SMInfCastKind {
        SMInfCastKindInvalid,
        SMInfCastKindNone,
        SMInfCastKindIntegerUpcast,
    } kind;
    ast::IType* to;
private:
    SMInf(ast::IType* from, SMInfCastKind kind, ast::IType* to) : from(from), kind(kind), to(to) {}
public:
    SMInf() {
        this->kind = SMInfCastKindInvalid;
    }
    static SMInf SMInfWithNoCast(ast::IType* t) {
        return SMInf(t, SMInfCastKindNone, t);
    }
    static SMInf SMInfWithIntegerUpcast(ast::IType* from, ast::IType* to) {
        return SMInf(from, SMInfCastKindIntegerUpcast, to);
    }
};
