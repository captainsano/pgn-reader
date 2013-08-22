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

class PGNMove;

class PGNVariation {
	std::string firstComment;
	std::vector<std::shared_ptr<PGNMove>> moves;
	
public:
	PGNVariation() = default;
	PGNVariation(const std::string & aFirstComment) : firstComment(aFirstComment) { }
	PGNVariation(std::vector<std::shared_ptr<PGNMove>> aMoves, const std::string & aFirstComment = "") : moves(aMoves) { }
	
	inline void addMove(std::shared_ptr<PGNMove> aMove) { this->moves.push_back(aMove); }
	
	inline std::shared_ptr<PGNMove> getFirstMove() {
		if (this->moves.size() > 0) return this->moves[0];
		return nullptr;
	}
	
	inline std::string getFirstComment() { return this->firstComment; }	
	inline auto size() -> decltype(moves.size()) { return moves.size(); }
	
	/* TODO: provide iterators */
};

#endif /* defined(__PGN_Reader__PGNVariation__) */
