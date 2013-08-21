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

class PGNGame {
	bool __metaParsed		= false;
	bool __moveTextParsed	= false;
	
	std::string		gameString;
	unsigned int	moveTextSectionBeginOffset;
	
	// Data members to be populated by parsing gameString
	std::map<std::string, std::string>	meta;
	std::string							firstComment;
	
	void parseMetaSection();
	void parseMoveTextSection();
public:
	PGNGame() = default;
	PGNGame(const std::string & pgnString);
	
	std::string		getMeta(std::string key);
	unsigned int	getHalfMoveCount();
	std::string		getFirstComment();
};

#endif /* defined(__PGN_Reader__PGNGame__) */
