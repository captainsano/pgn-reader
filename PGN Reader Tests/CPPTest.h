//
//  CPPTest.h
//  ChessFramework
//
//  Created by Santhos Baala RS on 23/07/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#ifndef ChessFramework_CPPTest_h
#define ChessFramework_CPPTest_h

#include <exception>

#define CPPAssertNoThrow(expression, format...) \
{ \
BOOL exceptionTriggered = NO; \
try { \
(expression); \
} catch (const std::exception e) { \
exceptionTriggered = YES; \
} \
if (exceptionTriggered) { \
XCTFail(format); \
} \
} \

#define CPPAssertThrows(expression, format...) \
{ \
BOOL exceptionTriggered = NO; \
try { \
(expression); \
} catch (const std::exception e) { \
exceptionTriggered = YES; \
} \
if (!exceptionTriggered) { \
XCTFail(format); \
} \
} \

#endif
