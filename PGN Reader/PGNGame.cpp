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
#include <vector>
#include <stack>
#include <sstream>

#include "PGNException.h"

#include "Piece.h"
#include "PGNTokenizer.h"
#include "PGNVariationFactory.h"
#include "GameState.h"

PGNGame::PGNGame(const std::string & pgnString) {
	if (pgnString.size() == 0) {
		throw parse_error("The game string is blank");
	}
	
	gameString = std::regex_replace(pgnString, std::regex("(\r\n)|(\n\r)"), "\n");
}

std::string	PGNGame::getFirstComment() {
	if (!__moveTextParsed) {
		parseMoveTextSection();
	}
	
	return this->mainVariation.getFirstComment();
}

auto PGNGame::getHalfMoveCount() -> decltype(mainVariation.size()) {
	if (!__moveTextParsed) {
		parseMoveTextSection();
	}
	
	return this->mainVariation.size();
}

std::string PGNGame::getMeta(std::string key) {
	if (!__metaParsed) {
		parseMetaSection();
	}
	
	if (!meta.count(key)) {
		return "";
	}
	
	return meta[key];
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
	
	auto i = gameString.cbegin();
	for (; i != gameString.cend(); i++) {
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
						throw parse_error("Character was not recognizable: " + std::string({*i}));
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
						throw parse_error("Character was not recognizable: " + std::string({*i}));
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
						throw parse_error("Character was not recognizable: " + std::string({*i}));
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
						throw parse_error("Character was not recognizable: " + std::string({*i}));
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
						throw parse_error("Character was not recognizable: " + std::string({*i}));
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
			moveTextSectionBeginOffset = static_cast<unsigned int>(i - gameString.cbegin() + 1);
			break;
		}
	}
	
	if (currentState != MetaReadStateFinal) {
		throw parse_error("An error was encountered while parsing: "  + std::string({*i}));
	} else {
		this->meta = std::move(parsedMeta);
		__metaParsed = true;
	}
}

void PGNGame::parseMoveTextSection() {
	if (!__metaParsed) {
		parseMetaSection();
	}
	
	enum {
		MoveTextReadStateInit,
		MoveTextReadStateM1,
		MoveTextReadStateM2,
		MoveTextReadStateM3,
		MoveTextReadStateM4,
		MoveTextReadStateM5,
		MoveTextReadStateFinal,
	} currentState = MoveTextReadStateInit;
	
	std::shared_ptr<PGNTokenizer::TempMove> prevMove, currentMove;
	std::shared_ptr<PGNTokenizer::TempVariation> currentTempVariation = std::make_shared<PGNTokenizer::TempVariation>();
	
	// Variation stack to recurse across RAVs - Stack is empty while parsing main line
	// current variation, move and annotation.
	std::stack<std::tuple<decltype(currentTempVariation), decltype(prevMove)>> RAVStack;
		
	auto i = gameString.cbegin() + moveTextSectionBeginOffset;
	for (; i != gameString.cend(); ) {
		PGNTokenizer::Token t = PGNTokenizer::nextToken(i, gameString.cend());
		
		switch (currentState) {
			case MoveTextReadStateInit: {
				switch (t.type) {
					case PGNTokenizer::TokenWhiteSpace: {
						// Do nothing
						break;
					}
					
					case PGNTokenizer::TokenTextAnnotation: {
						// The text annotation appearing here should be appended to first comment
						if (currentTempVariation->firstComment.length() > 0) {
							currentTempVariation->firstComment += " ";
						}
						currentTempVariation->firstComment += t.contents;
						break;
					}
						
					case PGNTokenizer::TokenMoveNumber: {
						currentState = MoveTextReadStateM1;
						// Do nothing
						break;
					}
					
					case PGNTokenizer::TokenGenericMove: {
						currentState = MoveTextReadStateM2;
						// Insert a tempMove into the current variation
						currentMove = std::make_shared<PGNTokenizer::TempMove>();
						fillTempMoveWithToken(*currentMove, t);
						currentTempVariation->moves.push_back(currentMove);
						
						// Reset the current move pointer to NULL
						prevMove = currentMove;
						currentMove = nullptr;
						break;
					}
						
					case PGNTokenizer::TokenGameTermination: {
						currentState = MoveTextReadStateFinal;
						break;
					}
						
					default: {
						throw parse_error("Invalid token encountered: " + t.contents + " " + std::to_string(__LINE__));
						break;
					}
				}
				
				break;
			}
				
			case MoveTextReadStateM1: {
				switch (t.type) {
					case PGNTokenizer::TokenWhiteSpace: {
						// Do Nothing
						break;
					}
						
					case PGNTokenizer::TokenDot: {
						// Do Nothing
						break;
					}
						
					case PGNTokenizer::TokenGenericMove: {
						currentState = MoveTextReadStateM2;
						// Insert a tempMove into the current variation
						currentMove = std::make_shared<PGNTokenizer::TempMove>();
						fillTempMoveWithToken(*currentMove, t);
						currentTempVariation->moves.push_back(currentMove);
						
						// Reset the current move pointer to NULL
						prevMove = currentMove;
						currentMove = nullptr;
						break;
					}
						
					default: {
						throw parse_error("Invalid token encountered: " + t.contents + " " + std::to_string(__LINE__));
						break;
					}
				}
				
				break;
			}
				
			case MoveTextReadStateM2: {
				switch (t.type) {
					case PGNTokenizer::TokenWhiteSpace: {
						// Do Nothing
						break;
					}
											
					case PGNTokenizer::TokenTextAnnotation: {
						// Append annotation to the previous move
						if (prevMove->textAnnotation.length() > 0) {
							prevMove->textAnnotation += " ";
						}
						prevMove->textAnnotation += t.contents;
						break;
					}
						
					case PGNTokenizer::TokenNAG: {
						// Append NAG to the previous move
						prevMove->NAGs.push_back(std::atoi(t.contents.c_str()));
						break;
					}
						
					case PGNTokenizer::TokenMoveNumber: {
						currentState = MoveTextReadStateM1;
						break;
					}
						
					case PGNTokenizer::TokenGenericMove: {
						// Insert a tempMove into the current variation
						currentMove = std::make_shared<PGNTokenizer::TempMove>();
						fillTempMoveWithToken(*currentMove, t);
						currentTempVariation->moves.push_back(currentMove);
						
						// Reset the current move pointer to NULL
						prevMove = currentMove;
						currentMove = nullptr;
						break;
					}
						
					case PGNTokenizer::TokenVariationBegin: {
						currentState = MoveTextReadStateM3;
						// Push the current variation to the stack and allocate a new variation
						RAVStack.push(std::make_tuple(currentTempVariation, prevMove));
						// Clear the current temp variation.
						// Add the new temp variation to the previous move's variations
						// Clear the previous move.
						currentTempVariation = std::make_shared<PGNTokenizer::TempVariation>();
						prevMove->variations.push_back(currentTempVariation);
						prevMove = nullptr;
						break;
					}
						
					case PGNTokenizer::TokenGameTermination: {
						currentState = MoveTextReadStateFinal;
						break;
					}
						
					default: {
						throw parse_error("Invalid token encountered: " + t.contents + " " + std::to_string(__LINE__));
						break;
					}
				}
				
				break;
			}
				
			case MoveTextReadStateM3: {
				switch (t.type) {
					case PGNTokenizer::TokenWhiteSpace: {
						// Do nothing
						break;
					}

					case PGNTokenizer::TokenTextAnnotation: {
						if (currentTempVariation->firstComment.length() > 0) {
							currentTempVariation->firstComment += " ";
						}
						currentTempVariation->firstComment += t.contents;
						break;
					}
						
					case PGNTokenizer::TokenMoveNumber: {
						currentState = MoveTextReadStateM4;
						break;
					}
						
					case PGNTokenizer::TokenGenericMove: {
						currentState = MoveTextReadStateM5;
						// Insert a tempMove into the current variation
						currentMove = std::make_shared<PGNTokenizer::TempMove>();
						fillTempMoveWithToken(*currentMove, t);
						currentTempVariation->moves.push_back(currentMove);
						
						// Reset the current move pointer to NULL
						prevMove = currentMove;
						currentMove = nullptr;
						break;
					}
						
					default: {
						throw parse_error("Invalid token encountered: " + t.contents + " " + std::to_string(__LINE__));
						break;
					}
				}
				
				break;
			}
				
			case MoveTextReadStateM4: {
				switch (t.type) {
					case PGNTokenizer::TokenWhiteSpace: {
						// Do nothing
						break;
					}
						
					case PGNTokenizer::TokenDot: {
						// Do nothing
						break;
					}
						
					case PGNTokenizer::TokenGenericMove: {
						currentState = MoveTextReadStateM5;
						// Insert a tempMove into the current variation
						currentMove = std::make_shared<PGNTokenizer::TempMove>();
						fillTempMoveWithToken(*currentMove, t);
						currentTempVariation->moves.push_back(currentMove);
						
						// Reset the current move pointer to NULL
						prevMove = currentMove;
						currentMove = nullptr;
						break;
					}
						
					default: {
						throw parse_error("Invalid token encountered: " + t.contents + " " + std::to_string(__LINE__));
						break;
					}
				}
				
				break;
			}
				
			case MoveTextReadStateM5: {
				switch (t.type) {
					case PGNTokenizer::TokenWhiteSpace: {
						// Do nothing
						break;
					}
						
					case PGNTokenizer::TokenTextAnnotation: {
						if (prevMove->textAnnotation.length() > 0) {
							prevMove->textAnnotation += " ";
						}
						prevMove->textAnnotation += t.contents;
						break;
					}
						
					case PGNTokenizer::TokenNAG: {
						prevMove->NAGs.push_back(std::atoi(t.contents.c_str()));
						break;
					}
						
					case PGNTokenizer::TokenMoveNumber: {
						currentState = MoveTextReadStateM4;
						break;
					}
						
					case PGNTokenizer::TokenGenericMove: {
						// Insert a tempMove into the current variation
						currentMove = std::make_shared<PGNTokenizer::TempMove>();
						fillTempMoveWithToken(*currentMove, t);
						currentTempVariation->moves.push_back(currentMove);
						
						// Reset the current move pointer to NULL
						prevMove = currentMove;
						currentMove = nullptr;

						break;
					}
						
					case PGNTokenizer::TokenVariationBegin: {
						currentState = MoveTextReadStateM3;
						// Push the current variation to the stack and allocate a new variation
						RAVStack.push(std::make_tuple(currentTempVariation, prevMove));
						// Clear the current temp variation.
						// Add the new temp variation to the previous move's variations
						// Clear the previous move.
						currentTempVariation = std::make_shared<PGNTokenizer::TempVariation>();
						prevMove->variations.push_back(currentTempVariation);
						prevMove = nullptr;
						break;
					}
						
					case PGNTokenizer::TokenVariationEnd: {
						// Return to m2 in case the variation is
						// at recursion level 1
						if (RAVStack.size() == 1) {
							currentState = MoveTextReadStateM2;
						} else {
							currentState = MoveTextReadStateM5;
						}
						
						// Restore the currentTemp and previous move variables from stack
						std::tuple<decltype(currentTempVariation), decltype(prevMove)> toRestore = RAVStack.top();
						currentTempVariation = std::get<0>(toRestore);
						prevMove = std::get<1>(toRestore);
						// Remove the top element from the stack
						RAVStack.pop();
						break;
					}
						
					default: {
						throw parse_error("Invalid token encountered: " + t.contents + " " + std::to_string(__LINE__));
						break;
					}
				}
				
				break;
			}
				
			case MoveTextReadStateFinal: {
				break;
			}
		}
		
		if (currentState == MoveTextReadStateFinal) {
			break;
		}
		
		i += t.charactersConsumed;
	}
	
	if (currentState != MoveTextReadStateFinal) {
		throw parse_error("Move text contains unrecognizable text: " + std::string({*i}) + " " + std::to_string(__LINE__));
	}
	
	/*-------------------------- Legality Checking for a variation -----------------------------*/
	sfc::cfw::GameState startingGameState;
	if (!(this->meta.count("FEN"))) {
		startingGameState = sfc::cfw::GameState("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR", sfc::cfw::ColorWhite, "KQkq");
	} else {
		// Decipher position from FEN
		std::stringstream fenStream(this->meta["FEN"]);
		std::string currentToken;

		std::string piecePlacement;
		sfc::cfw::Color sideToPlay;
		std::string castlingOptions;
		sfc::cfw::Square enPassantTarget;
		
		int idx = 0;
		while (std::getline(fenStream, currentToken, ' ')) {
			switch (idx) {
				case 0: {	// Piece placement
					piecePlacement = currentToken;
					break;
				}
				
				case 1: {	// Side to play
					sideToPlay = (currentToken == "w")?sfc::cfw::ColorWhite:sfc::cfw::ColorBlack;
					break;
				}
					
				case 2: {	// Castling options
					if (currentToken.find("K") != std::string::npos) { castlingOptions += "K"; }
					else { castlingOptions += "-"; }
					if (currentToken.find("Q") != std::string::npos) { castlingOptions += "Q"; }
					else { castlingOptions += "-"; }
					if (currentToken.find("k") != std::string::npos) { castlingOptions += "k"; }
					else { castlingOptions += "-"; }
					if (currentToken.find("q") != std::string::npos) { castlingOptions += "q"; }
					else { castlingOptions += "-"; }
					break;
				}
					
				case 3: {	// Enpassant Target
					if (currentToken == "-") { enPassantTarget = 0; }
					else { enPassantTarget = sfc::cfw::Square(currentToken); }
					break;
				}
					
				case 4: {	// Halfmove clock
					// Ignore
					break;
				}
					
				case 5: {	// Full Move Number
					this->firstMoveNumber = atoi(currentToken.c_str());
					break;
				}
			}
			
			idx++;
		}
		
		startingGameState = sfc::cfw::GameState(piecePlacement, sideToPlay, castlingOptions, enPassantTarget);
	}

	std::shared_ptr<PGNVariation> variation = PGNVariationFactory::legalVariation(*currentTempVariation, startingGameState);
			
	this->mainVariation = std::move(*variation);
	__moveTextParsed = true;
}