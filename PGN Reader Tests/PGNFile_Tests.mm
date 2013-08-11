//
//  PGNFile_Tests.m
//  PGN Reader
//
//  Created by Santhosbaala RS on 11/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#import <XCTest/XCTest.h>

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
	
}

- (void)testDoesNotThrowForEmptyFile {
	
}

- (void)testReturnsGameCountZeroForEmptyFile {
	
}

- (void)testReturnsGameCountNonZeroForNonEmptyFile {
	
}

- (void)testThrowsWhenEOFEncounteredBeforeGameTerminationMarker {
	
}

- (void)testRecognizesAllGameTerminationMarkers {
	
}


@end
