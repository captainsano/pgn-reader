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

- (void)testAcceptsMovePawn {
	std::string testString = "e4";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawn, @"Token sub type should be a normal pawn move");
		XCTAssertTrue(t.charactersConsumed == 2, @"Number of characters consumed should be equal to 2");
		XCTAssertTrue(t.contents == "e4", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal pawn move");
	}
}

- (void)testAcceptsMovePawn2 {
	std::string testString = "e4 c5 2.";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawn, @"Token sub type should be a normal pawn move");
		XCTAssertTrue(t.charactersConsumed == 2, @"Number of characters consumed should be equal to 2");
		XCTAssertTrue(t.contents == "e4", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal pawn move");
	}
}

- (void)testAcceptsMoveKSideCastling {
	std::string testString = "O-O";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMoveKingSideCastling, @"Token sub type should be king side castling");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "O-O", @"Contents should match O-O");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept king side castling");
	}
}

- (void)testAcceptsMoveKSideCastling2 {
	std::string testString = "O-O 22. Rd5";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMoveKingSideCastling, @"Token sub type should be king side castling");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "O-O", @"Contents should match O-O");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept king side castling");
	}
}

- (void)testAcceptsMoveQSideCastling {
	std::string testString = "O-O-O";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMoveQueenSideCastling, @"Token sub type should be king side castling");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "O-O-O", @"Contents should match O-O");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept king side castling");
	}
}

- (void)testAcceptsMoveQSideCastling2 {
	std::string testString = "O-O-O 25. Nd4";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMoveQueenSideCastling, @"Token sub type should be king side castling");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "O-O-O", @"Contents should match O-O");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept king side castling");
	}
}

- (void)testAcceptsTextCommentary {
	std::string testString = "{This is a comment.}";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenTextAnnotation, @"Token must be accepted as text annotation");
		XCTAssertTrue(t.contents == "This is a comment.", @"Contents must match the given input, without braces");
		XCTAssertTrue(t.charactersConsumed == testString.length(), @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept text commentary");
	}	
}

- (void)testAcceptsNAG {
	std::string testString = "$6";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenNAG, @"Token must be accepted as a NAG");
		XCTAssertTrue(t.contents == "6", @"Contents must match the given input, without $");
		XCTAssertTrue(t.charactersConsumed == testString.length(), @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept NAG");
	}
}

- (void)testAcceptsNAG2 {
	std::string testString = "$124 ";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenNAG, @"Token must be accepted as a NAG");
		XCTAssertTrue(t.contents == "124", @"Contents must match the given input, without $");
		XCTAssertTrue(t.charactersConsumed == 4, @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept NAG");
	}
}

@end
