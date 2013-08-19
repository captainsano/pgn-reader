//
//  PGNGame_nextToken_Tests.m
//  PGN Reader
//
//  Created by Santhosbaala RS on 19/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "PGNGame.h"

/*
 * The below declarations and types are required for the test to run.
 * In the actual reader, these will be hidden in the .cpp file
 */
#pragma mark - Parser Data Structures and utility method declarations
/*-------- Methods for used for internal parsing, not to be exposed outside ---------*/
enum TokenType {
	TokenInvalid,
	TokenWhiteSpace,
	TokenDot,
	TokenGameTermination,
	TokenNAG,
	TokenTextAnnotation,
	TokenVariationBegin,
	TokenVariationEnd,
	TokenMoveNumber,
	// Move Tokens
	TokenGenericMove,
};

enum TokenSubType {
	TokenSubTypeNone,
	// Move Sub-Types
	TokenSubTypeMoveNullMove,
	TokenSubTypeMovePawn,
	TokenSubTypeMovePawnCapture,
	TokenSubTypeMovePawnPromotion,
	TokenSubTypeMovePawnCapturePromotion,
	TokenSubTypeMovePiece,
	TokenSubTypeMovePieceFromFile,
	TokenSubTypeMovePieceFromRank,
	TokenSubTypeMovePieceFromSquare,
	TokenSubTypeMoveQueenSideCastling,
	TokenSubTypeMoveKingSideCastling,
};

struct Token {
	TokenType		type	= TokenInvalid;
	TokenSubType	subType	= TokenSubTypeNone;			// Used only for moves
	std::string		contents = "";
	unsigned int	charactersConsumed = 0;
	
	Token() = default;
};

Token nextToken(std::string::const_iterator begin, std::string::const_iterator end);
/*----------------------------------------------------------------------------------*/

@interface PGNGame_nextToken_Tests : XCTestCase

@end

@implementation PGNGame_nextToken_Tests

- (void)setUp {
    [super setUp];
    // Put setup code here. This method is called before the invocation of each test method in the class.
}

- (void)tearDown {
    // Put teardown code here. This method is called after the invocation of each test method in the class.
    [super tearDown];
}

- (void)testWhiteSpaceToken {
	std::string testString = "        \n      \r     \v   ";
	
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenWhiteSpace, @"Token must be accepted as white space");
		XCTAssertTrue(t.charactersConsumed == testString.length(), @"Number of characters consumed should be equal to the input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept white space");
	}
}

- (void)testMoveNumberToken1 {
	std::string testString = "1";

	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenMoveNumber, @"Token must be accepted as a move number");
		XCTAssertTrue(t.charactersConsumed == testString.length(), @"Number of characters consumed should be 1");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept move numbers");
	}
}

- (void)testMoveNumberToken2 {
	std::string testString = "85";

	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenMoveNumber, @"Token must be accepted as a move number");
		XCTAssertTrue(t.charactersConsumed == testString.length(), @"Number of characters consumed should be equal to the input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept move numbers");
	}
}

- (void)testMoveNumberTokenFollowedByDot {
	std::string testString = "12.";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenMoveNumber, @"Token must be accepted as a move number");
		XCTAssertTrue(t.charactersConsumed == 2, @"Number of characters consumed should be equal to the input length");
		
		t = nextToken(testString.cbegin() + t.charactersConsumed, testString.cend());
		XCTAssertTrue(t.type == TokenDot, @"Token must be accepted as a dot");
		XCTAssertTrue(t.charactersConsumed == 1, @"Exactly 1 dot should be recognized");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept move number followed by a single dot");
	}
}

- (void)testMoveNumberTokenFollowedByMultipleDots {
	std::string testString = "24...";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenMoveNumber, @"Token must be accepted as a move number");
		XCTAssertTrue(t.charactersConsumed == 2, @"Number of characters consumed should be equal to the input length");
		
		t = nextToken(testString.cbegin() + t.charactersConsumed, testString.cend());
		XCTAssertTrue(t.type == TokenDot, @"Token must be accepted as a dot");
		XCTAssertTrue(t.charactersConsumed == 3, @"Exactly 3 dot should be recognized");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept move number followed by multiple dots");
	}
}


@end
