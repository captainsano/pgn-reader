//
//  main.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 10/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include <iostream>
#include "PGNFile.h"

#define __TEST_FILE_PATH__ std::string("/Users/santhosbaala/Desktop/Projects/PGN Reader/PGN Reader Tests/")

int main(int argc, const char * argv[]) {
	PGNFile f(__TEST_FILE_PATH__ + "1.pgn");
	
	std::cout << f.getGame(5);
	
	std::cout << std::endl;
}

