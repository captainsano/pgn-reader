//
//  PGNGame_nextToken_Tests.m
//  PGN Reader
//
//  Created by Santhosbaala RS on 19/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#import <XCTest/XCTest.h>
#import "CPPTest.h"
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

- (void)testGameTerminationTokenLine {
	std::string testString = "*";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGameTermination, @"Token must be accepted as game termination - line");
		XCTAssertTrue(t.charactersConsumed == 1, @"Number of characters consumed should be equal to the input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as game termination");
	}
}

- (void)testGameTerminationTokenWhiteWon {
	std::string testString = "1-0";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGameTermination, @"Token must be accepted as game termination - white won");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to the input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as game termination");
	}
}

- (void)testGameTerminationTokenBlackWon {
	std::string testString = "0-1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGameTermination, @"Token must be accepted as game termination - black won");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to the input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as game termination");
	}
}

- (void)testGameTerminationTokenDraw {
	std::string testString = "0-1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGameTermination, @"Token must be accepted as game termination - draw");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to the input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as game termination");
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
		XCTFail(@"Token must be accepted as NAG");
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
		XCTFail(@"Token must be accepted as NAG");
	}
}

- (void)testReturnsCorrectNAGCodeForGoodMoveSuffix {
	std::string testString = "!";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenNAG, @"Token must be accepted as a NAG");
		XCTAssertTrue(t.contents == "1", @"Contents must match the given input, without $");
		XCTAssertTrue(t.charactersConsumed == 1, @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as NAG");
	}
}

- (void)testReturnsCorrectNAGCodeForPoorMoveSuffix {
	std::string testString = "?";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenNAG, @"Token must be accepted as a NAG");
		XCTAssertTrue(t.contents == "2", @"Contents must match the given input, without $");
		XCTAssertTrue(t.charactersConsumed == 1, @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as NAG");
	}
}

- (void)testReturnsCorrectNAGCodeForVeryGoodMoveSuffix {
	std::string testString = "!!";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenNAG, @"Token must be accepted as a NAG");
		XCTAssertTrue(t.contents == "3", @"Contents must match the given input, without $");
		XCTAssertTrue(t.charactersConsumed == 2, @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as NAG");
	}
}

- (void)testReturnsCorrectNAGCodeForVeryPoorMoveSuffix {
	std::string testString = "??";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenNAG, @"Token must be accepted as a NAG");
		XCTAssertTrue(t.contents == "4", @"Contents must match the given input, without $");
		XCTAssertTrue(t.charactersConsumed == 2, @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as NAG");
	}
}

- (void)testReturnsCorrectNAGCodeForSpeculativeMoveSuffix {
	std::string testString = "!?";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenNAG, @"Token must be accepted as a NAG");
		XCTAssertTrue(t.contents == "5", @"Contents must match the given input, without $");
		XCTAssertTrue(t.charactersConsumed == 2, @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as NAG");
	}
}

- (void)testReturnsCorrectNAGCodeForQuestionableMoveSuffix {
	std::string testString = "?!";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenNAG, @"Token must be accepted as a NAG");
		XCTAssertTrue(t.contents == "6", @"Contents must match the given input, without $");
		XCTAssertTrue(t.charactersConsumed == 2, @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as NAG");
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
		XCTFail(@"Token must be accepted as text commentary");
	}
}

- (void)testAcceptsContinuousTextCommentary {
	std::string testString = "{This is a comment.}{This is also a comment.}";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenTextAnnotation, @"Token must be accepted as text annotation");
		XCTAssertTrue(t.contents == "This is a comment.", @"Contents must match the given input, without braces");
		XCTAssertTrue(t.charactersConsumed == std::string("{This is a comment.}").length(), @"Characters consumed must match input length");
		
		t = nextToken(testString.cbegin() + t.charactersConsumed, testString.cend());
		XCTAssertTrue(t.type == TokenTextAnnotation, @"Token must be accepted as text annotation");
		XCTAssertTrue(t.contents == "This is also a comment.", @"Contents must match the given input, without braces");
		XCTAssertTrue(t.charactersConsumed == std::string("{This is also a comment.}").length(), @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as text commentary");
	}
}

- (void)testAcceptsVariationBegin {
	std::string testString = "({This begins a variation})";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenVariationBegin, @"Token must be accepted as the beginning of a variation");
		XCTAssertTrue(t.contents == "(", @"Contents must match the given input");
		XCTAssertTrue(t.charactersConsumed == 1, @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as variation beginning");
	}
}

- (void)testAcceptsVariationEnd {
	std::string testString = "({This begins a variation})";
	try {
		auto i = testString.cbegin();
		Token t = nextToken(i, testString.cend()); i += t.charactersConsumed;	// Returns variation begin
		t = nextToken(i, testString.cend()); i += t.charactersConsumed;	// Returns text annotation
		t = nextToken(i, testString.cend());	// Returns variation end
		
		XCTAssertTrue(t.type == TokenVariationEnd, @"Token must be accepted as the end of a variation");
		XCTAssertTrue(t.contents == ")", @"Contents must match the given input");
		XCTAssertTrue(t.charactersConsumed == 1, @"Characters consumed must match input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as variation end");
	}
}

- (void)testMoveNumberToken1 {
	std::string testString = "1";

	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenMoveNumber, @"Token must be accepted as a move number");
		XCTAssertTrue(t.charactersConsumed == testString.length(), @"Number of characters consumed should be 1");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as move numbers");
	}
}

- (void)testMoveNumberToken2 {
	std::string testString = "85";

	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenMoveNumber, @"Token must be accepted as a move number");
		XCTAssertTrue(t.charactersConsumed == testString.length(), @"Number of characters consumed should be equal to the input length");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as move numbers");
	}
}

- (void)testAcceptsNullMove {
	std::string testString = "1. --";
	try {
		auto i = testString.cbegin();
		Token t = nextToken(i, testString.cend()); i += t.charactersConsumed;  // Should return move number
		t = nextToken(i, testString.cend()); i += t.charactersConsumed;	// Should return dot
		t = nextToken(i, testString.cend()); i += t.charactersConsumed;	// Should return white space
		t = nextToken(i, testString.cend()); i += t.charactersConsumed;	// Should return NULL move
		
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMoveNullMove, @"Token sub type should be a null move");
		XCTAssertTrue(t.charactersConsumed == 2, @"Number of characters consumed should be equal to 2");
		XCTAssertTrue(t.contents == "--", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must be accepted as a NULL move");
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

- (void)testAcceptsPawnCapture {
	std::string testString = "ed4";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawnCapture, @"Token sub type should be a pawn capture move");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "ed4", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept pawn capture move");
	}
}

- (void)testAcceptsPawnCaptureWithX {
	std::string testString = "exd4";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawnCapture, @"Token sub type should be a pawn capture move");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "ed4", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept pawn capture move");
	}
}

- (void)testAcceptsPawnPromotionQ {
	std::string testString = "d8Q";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawnPromotion, @"Token sub type should be a pawn promotion move");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "d8Q", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept pawn promotion");
	}
}

- (void)testAcceptsPawnPromotionR {
	std::string testString = "d8R";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawnPromotion, @"Token sub type should be a pawn promotion move");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "d8R", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept pawn promotion");
	}
}

- (void)testAcceptsPawnPromotionB {
	std::string testString = "d1B";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawnPromotion, @"Token sub type should be a pawn promotion move");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "d1B", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept pawn promotion");
	}
}

- (void)testAcceptsPawnPromotionN {
	std::string testString = "d1N";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawnPromotion, @"Token sub type should be a pawn promotion move");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "d1N", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept pawn promotion");
	}
}

- (void)testAcceptsPawnPromotionWithEqual {
	std::string testString = "a1=N";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawnPromotion, @"Token sub type should be a pawn promotion move");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "a1N", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept pawn promotion with =");
	}
}

- (void)testAcceptsPawnCapturePromotion {
	std::string testString = "hg8Q";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawnCapturePromotion, @"Token sub type should be a pawn capture promotion move");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "hg8Q", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept pawn capture promotion");
	}
}

- (void)testAcceptsPawnCapturePromotionWithX {
	std::string testString = "hxg8Q";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawnCapturePromotion, @"Token sub type should be a pawn capture promotion move");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "hg8Q", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept pawn capture promotion with x");
	}
}

- (void)testAcceptsPawnCapturePromotionWithEqual {
	std::string testString = "hg8=Q";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawnCapturePromotion, @"Token sub type should be a pawn capture promotion move");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "hg8Q", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept pawn capture promotion with =");
	}
}

- (void)testAcceptsPawnCapturePromotionWithXAndEqual {
	std::string testString = "hxg8=Q";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePawnCapturePromotion, @"Token sub type should be a pawn capture promotion move");
		XCTAssertTrue(t.charactersConsumed == 6, @"Number of characters consumed should be equal to 6");
		XCTAssertTrue(t.contents == "hg8Q", @"Contents should match the supplied pawn move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept pawn capture promotion with x and =");
	}
}

- (void)testAcceptsPieceK {
	// TODO: Test for all the squares a1-h8
	std::string testString = "Kf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePiece, @"Token sub type should be a piece move");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "Kf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceQ {
	// TODO: Test for all the squares a1-h8
	std::string testString = "Qf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePiece, @"Token sub type should be a piece move");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "Qf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceR {
	// TODO: Test for all the squares a1-h8
	std::string testString = "Rf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePiece, @"Token sub type should be a piece move");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "Rf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceB {
	// TODO: Test for all the squares a1-h8
	std::string testString = "Bf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePiece, @"Token sub type should be a piece move");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "Bf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceN {
	// TODO: Test for all the squares a1-h8
	std::string testString = "Nf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePiece, @"Token sub type should be a piece move");
		XCTAssertTrue(t.charactersConsumed == 3, @"Number of characters consumed should be equal to 3");
		XCTAssertTrue(t.contents == "Nf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceKCapture {
	// TODO: Test for all the squares a1-h8
	std::string testString = "Kxf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePiece, @"Token sub type should be a piece move");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "Kf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceQCapture {
	// TODO: Test for all the squares a1-h8
	std::string testString = "Qxf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePiece, @"Token sub type should be a piece move");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "Qf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceRCapture {
	// TODO: Test for all the squares a1-h8
	std::string testString = "Rxf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePiece, @"Token sub type should be a piece move");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "Rf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceBCapture {
	// TODO: Test for all the squares a1-h8
	std::string testString = "Bxf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePiece, @"Token sub type should be a piece move");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "Bf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceNCapture {
	// TODO: Test for all the squares a1-h8
	std::string testString = "Nxf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePiece, @"Token sub type should be a piece move");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "Nf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testDoesNotAcceptPieceFromFileK {
	// TODO: Test for all the squares a1-h8 and all files a-h
	std::string testString = "Kdf1";
	CPPAssertThrows(nextToken(testString.cbegin(), testString.cend()), @"Should not accept king from file disambiguation");
}

- (void)testAcceptsPieceFromFileQ {
	// TODO: Test for all the squares a1-h8 and all files a-h
	std::string testString = "Qdf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromFile, @"Token sub type should be a piece move from file");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "Qdf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromFileR {
	// TODO: Test for all the squares a1-h8 and all files a-h
	std::string testString = "Rdf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromFile, @"Token sub type should be a piece move from file");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "Rdf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromFileB {
	// TODO: Test for all the squares a1-h8 and all files a-h
	std::string testString = "Bdf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromFile, @"Token sub type should be a piece move from file");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "Bdf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromFileN {
	// TODO: Test for all the squares a1-h8 and all files a-h
	std::string testString = "Ndf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromFile, @"Token sub type should be a piece move from file");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "Ndf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testDoesNotAcceptPieceFromFileKCapture {
	// TODO: Test for all the squares a1-h8 and all files a-h
	std::string testString = "Kdxf1";
	CPPAssertThrows(nextToken(testString.cbegin(), testString.cend()), @"Should not accept king from file disambiguation");
}

- (void)testAcceptsPieceFromFileQCapture {
	// TODO: Test for all the squares a1-h8 and all files a-h
	std::string testString = "Qdxf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromFile, @"Token sub type should be a piece move from file");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "Qdf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromFileRCapture {
	// TODO: Test for all the squares a1-h8 and all files a-h
	std::string testString = "Rdxf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromFile, @"Token sub type should be a piece move from file");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "Rdf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromFileBCapture {
	// TODO: Test for all the squares a1-h8 and all files a-h
	std::string testString = "Bdxf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromFile, @"Token sub type should be a piece move from file");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "Bdf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromFileNCapture {
	// TODO: Test for all the squares a1-h8 and all files a-h
	std::string testString = "Ndxf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromFile, @"Token sub type should be a piece move from file");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "Ndf1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testDoesNotAcceptPieceFromRankK {
	// TODO: Test for all the squares a1-h8 and all ranks 1-8
	std::string testString = "K1f1";
	CPPAssertThrows(nextToken(testString.cbegin(), testString.cend()), @"Should not accept king from rank disambiguation");
}

- (void)testAcceptsPieceFromRankQ {
	// TODO: Test for all the squares a1-h8 and all ranks 1-8
	std::string testString = "Q1f1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromRank, @"Token sub type should be a piece move from rank");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 2");
		XCTAssertTrue(t.contents == "Q1f1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromRankR {
	// TODO: Test for all the squares a1-h8 and all ranks 1-8
	std::string testString = "R2f1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromRank, @"Token sub type should be a piece move from rank");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "R2f1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromRankB {
	// TODO: Test for all the squares a1-h8 and all ranks 1-8
	std::string testString = "B4f1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromRank, @"Token sub type should be a piece move from rank");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "B4f1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromRankN {
	// TODO: Test for all the squares a1-h8 and all ranks 1-8
	std::string testString = "N3f1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromRank, @"Token sub type should be a piece move from rank");
		XCTAssertTrue(t.charactersConsumed == 4, @"Number of characters consumed should be equal to 4");
		XCTAssertTrue(t.contents == "N3f1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testDoesNotAcceptPieceFromRankKCapture {
	// TODO: Test for all the squares a1-h8 and all ranks 1-8
	std::string testString = "K2xf1";
	CPPAssertThrows(nextToken(testString.cbegin(), testString.cend()), @"Should not accept king from rank disambiguation");
}

- (void)testAcceptsPieceFromRankQCapture {
	// TODO: Test for all the squares a1-h8 and all ranks 1-8
	std::string testString = "Q5xf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromRank, @"Token sub type should be a piece move from rank");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "Q5f1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromRankRCapture {
	// TODO: Test for all the squares a1-h8 and all ranks 1-8
	std::string testString = "R8xf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromRank, @"Token sub type should be a piece move from rank");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "R8f1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromRankBCapture {
	// TODO: Test for all the squares a1-h8 and all ranks 1-8
	std::string testString = "B3xf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromRank, @"Token sub type should be a piece move from rank");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "B3f1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testAcceptsPieceFromRankNCapture {
	// TODO: Test for all the squares a1-h8 and all ranks 1-8
	std::string testString = "N2xf1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromRank, @"Token sub type should be a piece move from rank");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "N2f1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move");
	}
}

- (void)testDoesNotAcceptPieceFromSquareK {
	// TODO: Test for all the squares a1-h8 and all squares a1-h8
	std::string testString = "Ke1f1";
	CPPAssertThrows(nextToken(testString.cbegin(), testString.cend()), @"Should not accept king from square disambiguation");
}


- (void)testAcceptsPieceFromSquareQ {
	// TODO: Test for all the squares a1-h8 and all sqauares a1-h8
	std::string testString = "Qa1h1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromSquare, @"Token sub type should be a piece move from square");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "Qa1h1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move from square");
	}
}

- (void)testAcceptsPieceFromSquareR {
	// TODO: Test for all the squares a1-h8 and all sqauares a1-h8
	std::string testString = "Ra5h5";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromSquare, @"Token sub type should be a piece move from square");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "Ra5h5", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move from square");
	}
}

- (void)testAcceptsPieceFromSquareB {
	// TODO: Test for all the squares a1-h8 and all sqauares a1-h8
	std::string testString = "Ba1h8";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromSquare, @"Token sub type should be a piece move from square");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "Ba1h8", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move from square");
	}
}

- (void)testAcceptsPieceFromSquareN {
	// TODO: Test for all the squares a1-h8 and all sqauares a1-h8
	std::string testString = "Nd4f5";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromSquare, @"Token sub type should be a piece move from square");
		XCTAssertTrue(t.charactersConsumed == 5, @"Number of characters consumed should be equal to 5");
		XCTAssertTrue(t.contents == "Nd4f5", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move from square");
	}
}

- (void)testDoesNotAcceptPieceFromSquareKCapture {
	// TODO: Test for all the squares a1-h8 and all squares a1-h8
	std::string testString = "Ke1xf1";
	CPPAssertThrows(nextToken(testString.cbegin(), testString.cend()), @"Should not accept king from square disambiguation");
}

- (void)testAcceptsPieceFromSquareQCapture {
	// TODO: Test for all the squares a1-h8 and all sqauares a1-h8
	std::string testString = "Qa1xh1";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromSquare, @"Token sub type should be a piece move from square");
		XCTAssertTrue(t.charactersConsumed == 6, @"Number of characters consumed should be equal to 6");
		XCTAssertTrue(t.contents == "Qa1h1", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move from square");
	}
}

- (void)testAcceptsPieceFromSquareRCapture {
	// TODO: Test for all the squares a1-h8 and all sqauares a1-h8
	std::string testString = "Ra5xh5";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromSquare, @"Token sub type should be a piece move from square");
		XCTAssertTrue(t.charactersConsumed == 6, @"Number of characters consumed should be equal to 6");
		XCTAssertTrue(t.contents == "Ra5h5", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move from square");
	}
}

- (void)testAcceptsPieceFromSquareBCapture {
	// TODO: Test for all the squares a1-h8 and all sqauares a1-h8
	std::string testString = "Ba1xh8";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromSquare, @"Token sub type should be a piece move from square");
		XCTAssertTrue(t.charactersConsumed == 6, @"Number of characters consumed should be equal to 6");
		XCTAssertTrue(t.contents == "Ba1h8", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move from square");
	}
}

- (void)testAcceptsPieceFromSquareNCapture {
	// TODO: Test for all the squares a1-h8 and all sqauares a1-h8
	std::string testString = "Nd4xf5";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenGenericMove, @"Token must be accepted as a move");
		XCTAssertTrue(t.subType == TokenSubTypeMovePieceFromSquare, @"Token sub type should be a piece move from square");
		XCTAssertTrue(t.charactersConsumed == 6, @"Number of characters consumed should be equal to 6");
		XCTAssertTrue(t.contents == "Nd4f5", @"Contents should match the supplied move");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept normal piece move from square");
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

- (void)testTreatsCheckAsWhiteSpace {
	std::string testString = "+";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenWhiteSpace, @"Token must be accepted as white space");
		XCTAssertTrue(t.charactersConsumed == 1, @"Number of characters consumed should be 1");
		XCTAssertTrue(t.contents == " ", @"Contents should match ' '");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept check as white space");
	}
}

- (void)testTreatsCheckMateAsWhiteSpace {
	std::string testString = "#";
	try {
		Token t = nextToken(testString.cbegin(), testString.cend());
		XCTAssertTrue(t.type == TokenWhiteSpace, @"Token must be accepted as white space");
		XCTAssertTrue(t.charactersConsumed == 1, @"Number of characters consumed should be 1");
		XCTAssertTrue(t.contents == " ", @"Contents should match ' '");
	} catch (std::exception & e) {
		XCTFail(@"Token must accept check as white space");
	}
}

@end
