//
//  PGNGame.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 17/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include "PGNGame.h"
#include <cstdlib>
#include <regex>

/*-------- Methods for used for internal parsing, not to be exposed outside ---------*/
enum TokenType {
	TokenInvalid,
	TokenWhiteSpace,
	TokenDot,
	TokenGameTermination,
	TokenNAG,
	TokenVariationBegin,
	TokenVariationEnd,
	// Move Tokens
	TokenMoveNumber,
	TokenMoveQueenSideCastling,
	TokenMoveKingSideCastling,
	TokenMoveNullMove,
	TokenMovePawn,
	TokenMovePawnCapture,
	TokenMovePawnPromotion,
	TokenMovePawnCapturePromotion,
	TokenMovePiece,
	TokenMovePieceFromFile,
	TokenMovePieceFromRank,
	TokenMovePieceFromSquare,
};

struct Token {
	TokenType		type;
	std::string		contents;
	unsigned int	charactersConsumed;
};

Token nextToken(std::string::const_iterator begin, std::string::const_iterator end);

PGNGame::PGNGame(const std::string & pgnString) {
	if (pgnString.size() == 0) {
		throw std::invalid_argument("The given pgnString is blank");
	}
	
	gameString = std::regex_replace(pgnString, std::regex("(\r\n)|(\n\r)"), "\n");
}

unsigned int PGNGame::getHalfMoveCount() {
	if (!this->__moveTextParsed) {
		parseMoveTextSection();
	}
	
	return SHRT_MAX;
}

std::string	PGNGame::getOrphanedComment() {
	if (!this->__moveTextParsed) {
		parseMoveTextSection();
	}
	
	return this->orphanedComment;
}

void PGNGame::parseMetaSection() {
	enum {
		MetaReadStateInit,
		MetaReadStateE1,
		MetaReadStateE2,
		MetaReadStateE3,
		MetaReadStateE4,
		MetaReadStateE5,
		MetaReadStateE6,
		MetaReadStateFinal,
	} currentState = MetaReadStateInit;
	
	std::string currentKey, currentValue;
	std::map<std::string, std::string> parsedMeta;
	
	for (auto i = gameString.cbegin(); i != gameString.cend(); i++) {
		switch (currentState) {
			case MetaReadStateInit: {
				switch (*i) {
					case '[': {
						currentState = MetaReadStateE1;
						break;
					}
						
					case '\r':
					case '\n': {
						currentState = MetaReadStateFinal;
						break;
					}
						
					default: {
						throw std::out_of_range("Character was not recognizable");
						break;
					}
				}
				break;
			}
				
			case MetaReadStateE1: {
				switch (*i) {
					case '\n':
					case '\r':
					case ' ': {
						// Skip white space
						break;
					}
						
					case 'a'...'z':
					case 'A'...'Z': {
						currentKey += std::toupper(*i);
						currentState = MetaReadStateE2;
						break;
					}
						
					default: {
						throw std::out_of_range("Character was not recognizable");
						break;
					}
				}
				break;
			}
				
			case MetaReadStateE2: {
				switch (*i) {
					case '\n':
					case '\r':
					case ' ': {
						currentState = MetaReadStateE3;
						break;
					}
						
					default: {
						currentKey += *i;
						break;
					}
				}
				break;
			}
				
			case MetaReadStateE3: {
				switch (*i) {
					case '\n':
					case '\r':
					case ' ': {
						// Skip white space
						break;
					}
						
					case '"': {
						currentState = MetaReadStateE4;
						break;
					}
						
					default: {
						throw std::out_of_range("Character was not recognizable");
						break;
					}
				}
				break;
			}
				
			case MetaReadStateE4: {
				switch (*i) {
					case '"': {
						currentState = MetaReadStateE5;
						break;
					}
						
					default: {
						currentValue += *i;
						break;
					}
				}
				break;
			}
				
			case MetaReadStateE5: {
				switch (*i) {
					case '\n':
					case '\r':
					case ' ': {
						// Skip white space
						break;
					}
						
					case ']': {
						currentState = MetaReadStateE6;
						break;
					}
						
					default: {
						throw std::out_of_range("Character was not recognizable");
						break;
					}
				}
				break;
			}
				
			case MetaReadStateE6: {
				switch (*i) {
					case '\r':
					case '\n': {
						currentState = MetaReadStateInit;
						
						// Insert the key-value pair into the map
						parsedMeta[currentKey] = currentValue;
						currentKey = currentValue = "";
						break;
					}
						
					default: {
						throw std::out_of_range("Character was not recognizable");
						break;
					}
				}
				break;
			}
				
			case MetaReadStateFinal: {
				break;
			}
		}
		
		if (currentState == MetaReadStateFinal) {
			moveTextSectionBeginOffset = static_cast<unsigned int>(i - gameString.cbegin());
			break;
			
		}
	}
	
	if (currentState != MetaReadStateFinal) {
		throw std::invalid_argument("Parse Error");
	} else {
		this->meta = std::move(parsedMeta);
		__metaParsed = true;
	}
}

void PGNGame::parseMoveTextSection() {
	if (!__metaParsed) {
		parseMetaSection();
	}
	
	std::string::const_iterator i = gameString.cbegin() + moveTextSectionBeginOffset;
	
	char ch = *i;
	ch = ch;
	ch = ch;
	
	__moveTextParsed = true;
}

std::string PGNGame::getMeta(std::string key) {
	if (!__metaParsed) {
		parseMetaSection();
	}
	
	if (!meta.count(key)) {
		throw std::out_of_range("The given key does not exist in the meta");
	}
	
	return meta[key];
}

/*---------------------- Internal parser implementation -----------------------*/

Token nextToken(std::string::const_iterator begin, std::string::const_iterator end) {
	// How this method works:
	// get the longest possible string, obtain the matches based on decreasing length,
	// create the token, add the match to its contents and return the token.
	
	
	return {TokenInvalid, "", 0};
}