//
//  main.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 10/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include <iostream>
#include <vector>
#include <cmath>
#include "PGNReader.h"

#define __COMPLEX_GAMES_FILE_PATH__ std::string("/Users/santhosbaala/Desktop/Projects/PGN Reader/PGN Reader Tests/complex games/")
#define __TEST_FILE_PATH__ std::string("/Users/santhosbaala/Desktop/Projects/PGN Reader/PGN Reader Tests/")

void printVariation(const PGNVariation & variation, int level = 0);

int main(int argc, const char * argv[]) {
	PGNFile f;
	std::shared_ptr<PGNGame> g = nullptr;
	try {
		// f = PGNFile(__TEST_FILE_PATH__ + "kramnik_huffington.pgn");
		f = PGNFile(__COMPLEX_GAMES_FILE_PATH__ + "bali02.pgn");
	} catch (std::exception & e) {
		std::cout << "PGNFile error: " << e.what() << std::endl;
		exit(0);
	}
	
	std::cout << "\n Number of games: " << f.getGameCount() << std::endl;
	
	for (int i = 0; i < f.getGameCount(); i++) {
		try {
			g = f.getGame(i);
			std::cout << "\n Game: " << (i + 1);
			std::cout << "\n Move Count: " << std::ceil(g->getHalfMoveCount() / 2);
			std::cout << "\n Half Moves: " << g->getHalfMoveCount() << std::endl;
		} catch (illegal_move & e) {
			std::cout << "\nIllegal move: " << e.what() << std::endl;
		} catch (parse_error & e) {
			std::cout << "\n Parse Error: " << e.what() << std::endl;
		} catch (std::exception & e) {
			std::cout << "\n Unknown Error: " << e.what() << std::endl;
		}
	}
	
	for (int i = 0; i < f.getGameCount(); i++) {
		try {
			g = f.getGame(i);
			std::cout << "Half Move Count: " << g->getHalfMoveCount() << std::endl;
		} catch (illegal_move & e) {
			std::cout << "\n Illegal move: " << e.what() << std::endl;
		} catch (parse_error & e) {
			std::cout << "\n Parse Error: " << e.what() << std::endl;
		} catch (std::exception & e) {
			std::cout << "\n Unknown Error: " << e.what() << std::endl;
		}
		 
		
		if (g->getFirstComment().length() > 0) {
			std::cout << "{" << g->getFirstComment() << "}" << " ";
		}
		for (PGNGame::const_iterator m = g->cbegin(); m != g->cend(); m++) {
			std::cout << m->getSANString() << " ";
			if (m->getTextAnnotation().length() > 0) {
				std::cout << "{" << m->getTextAnnotation() << "}" << " ";
			}
			for (auto v : m->getVariations()) {
				printVariation(v);
			}
		}
		
		std::cout << std::endl;
	}
	
	std::cout << std::endl;
}

void printVariation(const PGNVariation & variation, int level) {
	// Give Indent Space based on variation level
	std::cout << std::endl;
	for (int i = 0; i <= level; i++) { std::cout << "\t"; }
	if (variation.getFirstComment().length() > 0) {
		std::cout << "{" << variation.getFirstComment() << "} ";
	}
	for (PGNVariation::const_iterator i = variation.cbegin(); i != variation.cend(); i++) {
		std::cout << i->getSANString() << " ";
		if (i->getTextAnnotation().length() > 0) {
			std::cout << "{" << i->getTextAnnotation() << "}" << " ";
		}
		for (auto v : i->getVariations()) {
			printVariation(v, level + 1);
		}
	}
}