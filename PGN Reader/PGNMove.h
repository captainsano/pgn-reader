//
//  PGNMove.h
//  PGN Reader
//
//  Created by Santhosbaala RS on 21/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#ifndef __PGN_Reader__PGNMove__
#define __PGN_Reader__PGNMove__

#include "Move.h"
#include <vector>

class PGNVariation;

class PGNMove : public sfc::cfw::Move {
	std::string textAnnotation;
	std::vector<unsigned int> NAGs;
	
	std::vector<PGNVariation> variations;
	
public:
	PGNMove() = default;
	PGNMove(const Move & aMove, std::vector<PGNVariation> aVariations = {}, std::vector<unsigned int> aNAGs = {}, const std::string & aTextAnnotation = "");
	
	void addVariation(const PGNVariation & aVariation);
	
	void addNAG(const unsigned int & aNAG);
	void appendTextAnnotation(const std::string & aTextAnnotation);
	
	inline bool hasVariations() const { return !(this->variations.empty()); }
	inline std::vector<PGNVariation> getVariations() const { return this->variations; }
	
	std::string getSANString() const;
};

#endif /* defined(__PGN_Reader__PGNMove__) */
