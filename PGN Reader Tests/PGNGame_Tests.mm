//
//  PGNGame_Tests.m
//  PGN Reader
//
//  Created by Santhosbaala RS on 17/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#import <XCTest/XCTest.h>
#import <fstream>
#import "CPPTest.h"

#define __TEST_FILE_PATH__ std::string("/Users/santhosbaala/Desktop/Projects/PGN Reader/PGN Reader Tests/")

#import "PGNGame.h"

@interface PGNGame_Tests : XCTestCase

@end

@implementation PGNGame_Tests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testShouldNotThrowExceptionForValidInitialization {
	std::ifstream file(__TEST_FILE_PATH__ + "test_file_1.pgn");
	std::string inputString;
	while (file.good()) {
		inputString += file.get();
	}
	
	CPPAssertNoThrow(PGNGame(inputString), @"Should not throw an exception for valid initialization");
}

- (void)testThrowsForBlankInitialization {
	CPPAssertThrows(PGNGame(""), @"Should throw for blank string initialization");
}

- (void)testMetaDataMatch1 {
	std::ifstream file(__TEST_FILE_PATH__ + "test_file_1.pgn");
	std::string inputString;
	while (file.good()) {
		inputString += file.get();
	}
	
	PGNGame g(inputString);
	
	XCTAssertTrue(g.getMeta("Event") == "100th ch-GBR 2013", @"Event should match");
	XCTAssertTrue(g.getMeta("Site") == "Torquay ENG", @"Site should match");
	XCTAssertTrue(g.getMeta("Date") == "2013.07.29", @"Date should match");
	XCTAssertTrue(g.getMeta("Round") == "1.1", @"Round should match");
	XCTAssertTrue(g.getMeta("White") == "Jones, Gawain C B", @"White should match");
	XCTAssertTrue(g.getMeta("Black") == "Reid, John", @"Black should match");
	XCTAssertTrue(g.getMeta("Result") == "1-0",	@"Result should match");
}

- (void)testThrowsExceptionForInvalidMetaKey {
	std::ifstream file(__TEST_FILE_PATH__ + "test_file_1.pgn");
	std::string inputString;
	while (file.good()) {
		inputString += file.get();
	}
	
	PGNGame g(inputString);
	
	CPPAssertThrows(g.getMeta("invalid_key"), @"Should throw for invalid meta key");
}

@end
