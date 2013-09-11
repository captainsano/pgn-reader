//
//  PGNGame.h
//  PGN Reader
//
//  Created by Santhosbaala RS on 17/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#ifndef __PGN_Reader__PGNGame__
#define __PGN_Reader__PGNGame__

#include <map>
#include <string>
#include "PGNVariation.h"

class PGNGame {
	bool __metaParsed		= false;
	bool __moveTextParsed	= false;
	
	unsigned int firstMoveNumber = 1;
	
	std::string		gameString;
	unsigned int	moveTextSectionBeginOffset;
	
	PGNVariation	mainVariation;
	
	// Data members to be populated by parsing gameString
	std::map<std::string, std::string>	meta;
	
	void parseMetaSection();
	void parseMoveTextSection();
public:
	PGNGame() = default;
	PGNGame(const std::string & pgnString);
	
	std::string	getMeta(std::string key);
	
	inline unsigned int getFirstMoveNumber() const { return this->firstMoveNumber; }
	std::string	getFirstComment();
	auto getHalfMoveCount() -> decltype(mainVariation.size());
	
	/* Iterator and Subscript access */
	using const_iterator = decltype(mainVariation)::const_iterator;
	const_iterator cbegin() {
		if (!__moveTextParsed) this->parseMoveTextSection();
		return mainVariation.cbegin();
	}
	const_iterator cend() {
		if (!__moveTextParsed) this->parseMoveTextSection();
		return mainVariation.cend();
	}
	
	using size_type = decltype(mainVariation)::size_type;
	using const_reference = decltype(mainVariation)::const_reference;

	const_reference operator[] (size_type idx) {
		if (!__moveTextParsed) this->parseMoveTextSection();
		return mainVariation[idx];
	}
};

#endif /* defined(__PGN_Reader__PGNGame__) */
