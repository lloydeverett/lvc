//
//  integertypedefs.h
//  lvc
//
//  Created by Lloyd Everett on 2015/05/29.
//  Copyright (c) 2015 Lloyd Everett. All rights reserved.
//

#pragma once
#include <cstdint>

typedef uint64_t rownumber;

// In debug mode, we make colnumber int64_t so that the compiler will generate
// warnings and/or errors if we use the types in a way that is not logically coherent
// i.e. comparing a colnumber with a rownumber.
#ifdef DEBUG
typedef int64_t colnumber;
#else
typedef uint64_t colnumber;
#endif

typedef uint64_t charcount;
