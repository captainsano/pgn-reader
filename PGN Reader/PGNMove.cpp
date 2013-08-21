//
//  PGNMove.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 21/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include "PGNMove.h"
#include "PGNVariation.h"

PGNMove::PGNMove(const Move & aMove, std::vector<std::shared_ptr<PGNVariation>> aVariations, std::vector<unsigned int> aNAGs, const std::string & aTextAnnotation) : Move(aMove) {
	// Check if all the variations have the same before game state
	for (auto v : aVariations) {
		this->addVariation(v);
	}
	
	for (auto n : aNAGs) {
		this->addNAG(n);
	}
	
	this->textAnnotation = aTextAnnotation;
}

void PGNMove::addVariation(std::shared_ptr<PGNVariation> aVariation) {
	// Prevent addition of a variation in case it exists and prevent addition in case
	// the before game state is not the same as this move
	if (aVariation->getFirstMove() == nullptr) {
		throw std::invalid_argument("The new variation is empty");
	}
	
	if (*(aVariation->getFirstMove()->getGameStateBeforeMove()) == *(this->getGameStateBeforeMove())) {
		this->variations.push_back(aVariation);
	} else {
		throw std::invalid_argument("The new variation's game state before move does not match");
	}
}

void PGNMove::addVariation(const PGNVariation & aVariation) {
	std::shared_ptr<PGNVariation> newVariation = std::make_shared<PGNVariation>(aVariation);
	this->addVariation(newVariation);
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