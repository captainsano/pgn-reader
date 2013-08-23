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
#include "PGNException.h"

#define __TEST_FILE_PATH__ std::string("/Users/santhosbaala/Desktop/Projects/PGN Reader/PGN Reader Tests/")

int main(int argc, const char * argv[]) {
	PGNFile f(__TEST_FILE_PATH__ + "game_termination_inside_comment.pgn");
	std::shared_ptr<PGNGame> g = f.getGame(0);
	
	try {
		std::cout << "Half Move Count: " << g->getHalfMoveCount() << std::endl;
	} catch (illegal_move & e) {
		std::cout << "\n Illegal move: " << e.what() << std::endl;
	} catch (parse_error & e) {
		std::cout << "\n Parse Error: " << e.what() << std::endl;
	} catch (std::exception & e) {
		std::cout << "\n Unknown Error: " << e.what() << std::endl;
	}
	
	for (PGNGame::const_iterator m = g->cbegin(); m != g->cend(); m++) {
		std::cout << m->getSANString() << " ";
	}
	
	std::cout << std::endl;
}