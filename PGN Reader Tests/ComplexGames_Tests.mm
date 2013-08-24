//
//  ComplexGames_Tests.m
//  PGN Reader
//
//  Created by Santhosbaala RS on 24/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "CPPTest.h"

#import "PGNFile.h"

#define __COMPLEX_GAMES_FILE_PATH__ std::string("/Users/santhosbaala/Desktop/Projects/PGN Reader/PGN Reader Tests/complex games/")

@interface ComplexGames_Tests : XCTestCase

@end

@implementation ComplexGames_Tests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testBali02 {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "bali02.pgn");
		XCTAssertTrue(f.getGameCount() == 16, @"Game count should be equal");
	} catch(std::exception & e) {
		XCTFail(@"Unable to read from database bali02.pgn");
	}
}

@end
