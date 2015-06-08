//
//  primitivetypeenum.h
//  lvc
//
//  Created by Lloyd Everett on 2015/06/07.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once

enum PrimitiveTypename  {
    Int,
    Uint,
    Int16,
    Uint16,
    Int32,
    Uint32,
    Int64,
    Uint64,
    Bool,
    Float,
    Double,
    INVALID_PRIMITIVETYPENAME,
};

inline PrimitiveTypename primitiveTypenameFromStrInSource(const std::string &str) {
    if (str == "int")     return Int;
    if (str == "uint")    return Uint;
    if (str == "int16")   return Int16;
    if (str == "uint16")  return Uint16;
    if (str == "int32")   return Int32;
    if (str == "uint32")  return Uint32;
    if (str == "int64")   return Int64;
    if (str == "uint64")  return Uint64;
    if (str == "bool")    return Bool;
    if (str == "float")   return Float;
    if (str == "double")  return Double;
    return INVALID_PRIMITIVETYPENAME;
}
