//
//  PGNFile_Tests.m
//  PGN Reader
//
//  Created by Santhosbaala RS on 11/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "CPPTest.h"

#define __TEST_FILE_PATH__ std::string("/Users/santhosbaala/Desktop/Projects/PGN Reader/PGN Reader Tests/")

#import "PGNFile.h"

@interface PGNFile_Tests : XCTestCase

@end

@implementation PGNFile_Tests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testThrowsForInvalidFile {
	CPPAssertThrows(PGNFile(__TEST_FILE_PATH__ + "some_invalid_file.pgn"), @"Creating a PGN file object with invalid file should throw");
}

- (void)testDoesNotThrowForEmptyFile {
	CPPAssertNoThrow(PGNFile(__TEST_FILE_PATH__ + "empty_file.pgn"), @"Creating a PGN file object with empty file contents should not throw");
}

- (void)testReturnsGameCountZeroForEmptyFile {
	PGNFile f(__TEST_FILE_PATH__ + "empty_file.pgn");
	XCTAssertTrue(f.getGameCount() == 0, @"Empty PGN file should return game count 0");
}

- (void)testReturnsGameCountNonZeroForNonEmptyFile {
	PGNFile f(__TEST_FILE_PATH__ + "test_file_1.pgn");
	XCTAssertTrue(f.getGameCount() != 0, @"PGN file with a few games should return non-zero game count");
}

- (void)testThrowsWhenEOFEncounteredBeforeGameTerminationMarker {
	CPPAssertThrows(PGNFile(__TEST_FILE_PATH__ + "premature_eof.pgn"), @"PGN file with premature EOF should throw");
}

- (void)testRecognizesAllGameTerminationMarkers {
	PGNFile f;
	CPPAssertNoThrow(f = PGNFile(__TEST_FILE_PATH__ + "all_term_markers.pgn"), @"PGN file should be able to initialize with any game termination marker");
	XCTAssertTrue(f.getGameCount() == 4, @"PGN file should be able to recognize all game termination markers");
}

- (void)testRecognizesCastlingStringWithZeros {
	PGNFile f;
	CPPAssertNoThrow(f = PGNFile(__TEST_FILE_PATH__ + "zero_castling.pgn"), @"PGN file uses non-standard castling string with 0s");
	XCTAssertTrue(f.getGameCount() == 2, @"PGN file should be able to recognize and distinguish 0s castling string");
}

- (void)testSkipsGameTerminationInsideTextAnnotation {
	PGNFile f(__TEST_FILE_PATH__ + "game_termination_inside_comment.pgn");
	XCTAssertTrue(f.getGameCount() == 1, @"Game count should be equal to 1");
}

- (void)testRecognizesSampleDatabase {
	PGNFile f(__TEST_FILE_PATH__ + "1.pgn");
	XCTAssertTrue(f.getGameCount() == 5000, @"Game count should be equal to 1000");
}


@end
