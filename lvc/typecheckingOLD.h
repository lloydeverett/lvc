//
//  typechecking.h
//  lvc
//
//  Created by Lloyd Everett on 2015/07/17.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <utility>
#include "ast.h"
#include "astcompilerexceptions.h"
#include "implicitcast.h"

inline boost::optional<std::pair<ast::IntegerType, ast::IntegerType>>
tryCastTypePairToIntegerTypes(const ast::IType& first, const ast::IType& second) {
    if (auto integerFirstPtr = dynamic_cast<const ast::IntegerType*>(&first)) {
        if (auto integerSecondPtr = dynamic_cast<const ast::IntegerType*>(&second)) {
            return std::pair<ast::IntegerType, ast::IntegerType>(*integerFirstPtr, *integerSecondPtr);
        }
    }
    return boost::none;
}

inline bool doesConstnessInhibitTypeCompatibility(const ast::IType& from, const ast::IType& to) {
    // not constant -> constant is valid
    // not constant -> not constant is valid
    // constant -> constant is valid
    return !((!from.isConstant() && to.isConstant()) || (from.isConstant() == to.isConstant()));
}

inline bool doTypesMatch(const ast::IType& from, const ast::IType& to) {
    if (!from.isConstant() && to.isConstant()) {
        auto constFrom = from.clone();
        constFrom->setConstant(true);
        return constFrom->isEqual(to);
    }
    else {
        return from.isEqual(to);
    }
}

inline boost::optional<ImplicitCast> tryCreateImplicitIntegerUpcast(ast::IntegerType from, ast::IntegerType to) {
    if (doesConstnessInhibitTypeCompatibility(from, to))
        return boost::none;
    if (from.numBits >= to.numBits)
        return boost::none;
    if (from.isSigned != to.isSigned)
        return boost::none;
    return ImplicitCast(to.clone(), CastKindIntegerUpcast);
}

inline boost::optional<ImplicitCast> tryCreateImplicitIntegerUpcast(const ast::IType& from, const ast::IType& to) {
    auto optPair = tryCastTypePairToIntegerTypes(from, to);
    if (optPair) {
        auto pair = *optPair;
        return tryCreateImplicitIntegerUpcast(pair.first, pair.second);
    }
    return boost::none;
}
