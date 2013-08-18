//
//  PGNGame.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 17/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include "PGNGame.h"
#include <cstdlib>

PGNGame::PGNGame(const std::string & pgnString) {
	if (pgnString.size() == 0) {
		throw std::invalid_argument("The given pgnString is blank");
	}
	
	gameString = pgnString;
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
	}
	
	if (currentState != MetaReadStateFinal) {
		throw std::invalid_argument("Parse Error");
	} else {
		this->meta = std::move(parsedMeta);
		__metaParsed = true;
	}
}

void PGNGame::parseMoveTextSection(std::string::const_iterator i) {
	
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