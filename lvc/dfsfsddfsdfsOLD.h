//
//  implicitcast.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/16.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include "ast.h"

enum CastKind {
    CastKindIntegerUpcast,
    CastKindNullCast,
};

struct ImplicitCast {
    CastKind kind;
    
    // Represents the target type
    // (the same as the original type for null casts).
    std::unique_ptr<ast::IType> type;
    
    ast::IType* getTypePtr() {
        return type.get();
    }
    
    ImplicitCast(std::unique_ptr<ast::IType> type, CastKind kind) : type(std::move(type)), kind(kind) {}
};
