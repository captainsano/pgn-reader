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

- (void)test_Bali02 {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "bali02.pgn");
		XCTAssertTrue(f.getGameCount() == 16, @"Game count should be equal");
	} catch(std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_d00_chess_informant {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "d00_chess_informant.pgn");
		XCTAssertTrue(f.getGameCount() == 103, @"Game count should be equal");
	} catch(std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_electronic_campfire {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "electronic_campfire.pgn");
		XCTAssertTrue(f.getGameCount() == 6, @"Game count should be equal");
	} catch(std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_europe_echecs {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "europe_echecs.pgn");
		XCTAssertTrue(f.getGameCount() == 307, @"Game count should be equal");
	} catch(std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_exeter_lessons_from_tal {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "exeter_lessons_from_tal.pgn");
		XCTAssertTrue(f.getGameCount() == 8, @"Game count should be equal");
	} catch (std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_exeter_famous_games {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "famous_games.pgn");
		XCTAssertTrue(f.getGameCount() == 500, @"Game count should be equal");
	} catch (std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_GM_games {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "GM_games.pgn");
		XCTAssertTrue(f.getGameCount() == 20, @"Game count should be equal");
	} catch (std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_great_masters {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "great_masters.pgn");
		XCTAssertTrue(f.getGameCount() == 13, @"Game count should be equal");
	} catch (std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_hartwig {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "hartwig.pgn");
		XCTAssertTrue(f.getGameCount() == 29, @"Game count should be equal");
	} catch (std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_hayes {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "hayes.pgn");
		XCTAssertTrue(f.getGameCount() == 9, @"Game count should be equal");
	} catch (std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_human_computer {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "human_computer.pgn");
		XCTAssertTrue(f.getGameCount() == 29, @"Game count should be equal");
	} catch (std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_kasp_top {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "kasp_top.pgn");
		XCTAssertTrue(f.getGameCount() == 62, @"Game count should be equal");
	} catch (std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

- (void)test_kk {
	try {
		PGNFile f(__COMPLEX_GAMES_FILE_PATH__ + "kk.pgn");
		XCTAssertTrue(f.getGameCount() == 2, @"Game count should be equal");
	} catch (std::exception & e) {
		XCTFail(@"Unable to read from pgn database: %s", e.what());
	}
}

@end
