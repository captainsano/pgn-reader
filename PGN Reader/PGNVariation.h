//
//  PGNVariation.h
//  PGN Reader
//
//  Created by Santhosbaala RS on 21/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#ifndef __PGN_Reader__PGNVariation__
#define __PGN_Reader__PGNVariation__

#include <memory>
#include <string>
#include <vector>
#include "PGNMove.h"

class PGNVariation {
	std::string firstComment;
	std::vector<PGNMove> moves;
	
public:
	PGNVariation() = default;
	PGNVariation(const std::string & aFirstComment) : firstComment(aFirstComment) { }
	PGNVariation(std::vector<PGNMove> aMoves, const std::string & aFirstComment = "") : moves(aMoves) { }
	
	inline void addMove(PGNMove aMove) { this->moves.push_back(aMove); }
		
	inline std::string getFirstComment() const { return this->firstComment; }
	
	/* Iterator and Subscript access */
	using const_iterator = decltype(moves)::const_iterator;
	
	const_iterator cbegin() const { return moves.cbegin(); }
	const_iterator cend() const { return moves.cend(); }
	
	using const_reference = decltype(moves)::const_reference;
	using size_type = decltype(moves)::size_type;

	inline size_type size() const { return moves.size(); }
	const_reference operator[] (size_type idx) const { return moves[idx]; }
};

#endif /* defined(__PGN_Reader__PGNVariation__) */
