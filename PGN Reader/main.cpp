//
//  main.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 10/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include <iostream>
#include "PGNFile.h"
#include "PGNGame.h"

#define __TEST_FILE_PATH__ std::string("/Users/santhosbaala/Desktop/Projects/PGN Reader/PGN Reader Tests/")

int main(int argc, const char * argv[]) {
	PGNFile f(__TEST_FILE_PATH__ + "test_file_1.pgn");
	PGNGame g(f.getGame(0));
	
	std::cout << "Half Move Count: " << g.getHalfMoveCount() << std::endl;
		
	std::cout << std::endl;
}