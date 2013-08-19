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

- (void)testThrowsExceptionForInvalidMetaKey {
	std::ifstream file(__TEST_FILE_PATH__ + "test_file_1.pgn");
	std::string inputString;
	while (file.good()) {
		inputString += file.get();
	}
	
	PGNGame g(inputString);
	
	CPPAssertThrows(g.getMeta("invalid_key"), @"Should throw for invalid meta key");
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

- (void)testAcceptsCRLFEndings {
	std::ifstream file(__TEST_FILE_PATH__ + "test_file_2_CRLF.pgn");
	std::string inputString;
	while (file.good()) {
		inputString += file.get();
	}
	
	try {
		PGNGame g(inputString);
		XCTAssertTrue(g.getMeta("Event") == "USA-ch m", @"Event should match");
		XCTAssertTrue(g.getMeta("Site") == "USA", @"Site should match");
		XCTAssertTrue(g.getMeta("Date") == "1909.??.??", @"Date should match");
		XCTAssertTrue(g.getMeta("Round") == "5", @"Round should match");
		XCTAssertTrue(g.getMeta("White") == "Showalter, Jackson Whipps", @"White should match");
		XCTAssertTrue(g.getMeta("Black") == "Marshall, Frank James", @"Black should match");
		XCTAssertTrue(g.getMeta("Result") == "1-0",	@"Result should match");
	} catch (std::exception & e) {
		XCTFail(@"Throws while accessing the meta keys. Should handle CRLF line endings");
	}
}

#pragma error Do not run the test cases related to moves without passing all nextToken tests.
/*
- (void)testAcceptsGameWithoutMoves {
	std::ifstream file(__TEST_FILE_PATH__ + "no_moves.pgn");
	std::string inputString;
	while (file.good()) {
		inputString += file.get();
	}
	
	try {
		PGNGame g(inputString);
		XCTAssertTrue(g.getHalfMoveCount() == 0, @"Move count should be equal 0");
	} catch (std::exception & e) {
		XCTFail(@"Move count should be equal to 0");
	}
}

- (void)testReturnsOrphanedComment {
	std::ifstream file(__TEST_FILE_PATH__ + "orphaned_comment.pgn");
	std::string inputString;
	while (file.good()) {
		inputString += file.get();
	}
	
	try {
		PGNGame g(inputString);
		XCTAssertTrue(g.getHalfMoveCount() == 0, @"Move count should be equal 0");
		XCTAssertTrue(g.getOrphanedComment() == "This is an orphaned comment!", @"Should contain an orphaned comment");
	} catch (std::exception & e) {
		XCTFail(@"Move count should be equal to 0 and an orphaned comment should exist");
	}
}

- (void)testReturnsCorrectHalfMoveCount {
	std::ifstream file(__TEST_FILE_PATH__ + "test_file_1.pgn");
	std::string inputString;
	while (file.good()) {
		inputString += file.get();
	}
	
	try {
		PGNGame g(inputString);
		XCTAssertTrue(g.getHalfMoveCount() == 67, @"Move count should match");
	} catch (std::exception & e) {
		XCTFail(@"Half move count should match the game");
	}
}
*/

@end
