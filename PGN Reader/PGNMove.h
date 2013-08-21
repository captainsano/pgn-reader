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
	
	std::vector<std::shared_ptr<PGNVariation>> variations;
	
public:
	PGNMove() = default;
	PGNMove(const Move & aMove, std::vector<std::shared_ptr<PGNVariation>> aVariations = {}, std::vector<unsigned int> aNAGs = {}, const std::string & aTextAnnotation = "");
	
	void addVariation(std::shared_ptr<PGNVariation> aVariation);
	void addVariation(const PGNVariation & aVariation);
	
	void addNAG(const unsigned int & aNAG);
	void appendTextAnnotation(const std::string & aTextAnnotation);
	
	inline bool hasVariations() const { return !(this->variations.empty()); }
	inline std::vector<std::shared_ptr<PGNVariation>> getVariations() const { return this->variations; }
};

#endif /* defined(__PGN_Reader__PGNMove__) */
