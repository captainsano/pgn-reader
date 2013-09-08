//
//  PGNMove.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 21/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include "PGNMove.h"
#include "PGNVariation.h"

PGNMove::PGNMove(const Move & aMove, std::vector<PGNVariation> aVariations, std::vector<unsigned int> aNAGs, const std::string & aTextAnnotation) : Move(aMove) {
	// Check if all the variations have the same before game state
	for (auto v : aVariations) {
		this->addVariation(v);
	}
	
	for (auto n : aNAGs) {
		this->addNAG(n);
	}
	
	this->textAnnotation = aTextAnnotation;
}

void PGNMove::addVariation(const PGNVariation & aVariation) {
	// Prevent addition of a variation in case it exists and prevent addition in case
	// the before game state is not the same as this move
	if (aVariation.size() == 0) {
		throw std::invalid_argument("The new variation is empty");
	}
	
	if (*(aVariation[0].getGameStateBeforeMove()) == *(this->getGameStateBeforeMove())) {
		this->variations.push_back(aVariation);
	} else {
		throw std::invalid_argument("The new variation's game state before move does not match");
	}
}

void PGNMove::addNAG(const unsigned int & aNAG) {
	for (auto n : this->NAGs) {
		if (n == aNAG) return;
	}
	
	this->NAGs.push_back(aNAG);
}

void PGNMove::appendTextAnnotation(const std::string & aTextAnnotation) {
	if (this->textAnnotation.length() > 0) {
		this->textAnnotation += " ";
	}
	
	this->textAnnotation += aTextAnnotation;
}

std::string PGNMove::getSANString() const {
	std::string SANString = sfc::cfw::Move::getSANString();
	std::string NAGMoveAnnotation = "";	// We'll take the best of all matches
	
	for (auto NAGCode : this->NAGs) {
		switch (NAGCode) {
			case 1: {
				NAGMoveAnnotation = "!";
				break;
			}
				
			case 2: {
				NAGMoveAnnotation = "?";
				break;
			}
				
			case 3: {
				NAGMoveAnnotation = "!!";
				break;
			}
				
			case 4: {
				NAGMoveAnnotation = "??";
				break;
			}
				
			case 5: {
				NAGMoveAnnotation = "!?";
				break;
			}
				
			case 6: {
				NAGMoveAnnotation = "?!";
				break;
			}
		}
	}
	
	return SANString + NAGMoveAnnotation;
}