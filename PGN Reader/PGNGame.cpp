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

#pragma mark - Parser Data Structures and utility method declarations
/*-------- Methods for used for internal parsing, not to be exposed outside ---------*/
enum TokenType {
	TokenInvalid,
	TokenWhiteSpace,
	TokenDot,
	TokenGameTermination,
	TokenNAG,
	TokenTextAnnotation,
	TokenVariationBegin,
	TokenVariationEnd,
	TokenMoveNumber,
	// Move Tokens
	TokenGenericMove,
};

enum TokenSubType {
	TokenSubTypeNone,
	// Move Sub-Types
	TokenSubTypeMoveNullMove,
	TokenSubTypeMovePawn,
	TokenSubTypeMovePawnCapture,
	TokenSubTypeMovePawnPromotion,
	TokenSubTypeMovePawnCapturePromotion,
	TokenSubTypeMovePiece,
	TokenSubTypeMovePieceFromFile,
	TokenSubTypeMovePieceFromRank,
	TokenSubTypeMovePieceFromSquare,
	TokenSubTypeMoveQueenSideCastling,
	TokenSubTypeMoveKingSideCastling,
};

struct Token {
	TokenType		type	= TokenInvalid;
	TokenSubType	subType	= TokenSubTypeNone;			// Used only for moves
	std::string		contents = "";
	unsigned int	charactersConsumed = 0;
	
	Token() = default;
};

Token nextToken(std::string::const_iterator begin, std::string::const_iterator end);

// The information in temp move will be filled/validated by ChessFramework later
struct TempMove {
	Token moveToken;	// The token that is representing the move
	
	unsigned short fromFile = SHRT_MAX;
	unsigned short fromRank = SHRT_MAX;
	unsigned short toFile	= SHRT_MAX;
	unsigned short toRank	= SHRT_MAX;
	// All set to 0s indicate a NULL move
	// SHRT_MAX indicates that the field has not yet been set.
	
	char promotedToPiece;	// 'Q', 'R', 'N', 'B'
	
	std::string textAnnotationAfter;
	std::string textAnnotationBefore;
	std::vector<unsigned int> NAGs;
	std::vector<std::vector<TempMove>> variations { };
	// TODO Include commands withing annotation
	
	TempMove() = default;
};

#pragma mark - Public method implementation

PGNGame::PGNGame(const std::string & pgnString) {
	if (pgnString.size() == 0) {
		throw std::invalid_argument("The given pgnString is blank");
	}
	
	gameString = std::regex_replace(pgnString, std::regex("(\r\n)|(\n\r)"), "\n");
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
			moveTextSectionBeginOffset = static_cast<unsigned int>(i - gameString.cbegin() + 1);
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
	
	enum {
		MoveTextReadStateInit,
		MoveTextReadStateM1,
		MoveTextReadStateM2,
		MoveTextReadStateM3,
		MoveTextReadStateM4,
		MoveTextReadStateM5,
		MoveTextReadStateFinal,
	} currentState = MoveTextReadStateInit;
	
	TempMove currentMove;
	std::string currentAnnotation = "";
	std::vector<TempMove> currentTempVariation;
	
	// Variation stack to recurse across RAVs - Stack is empty while parsing main line
	// current variation, move and annotation.
	std::stack<std::tuple<std::vector<TempMove>, TempMove, std::string>> RAVStack;
		
	for (auto i = gameString.cbegin() + moveTextSectionBeginOffset; i != gameString.cend(); ) {
		Token t = nextToken(i, gameString.cend());
		
		switch (currentState) {
			case MoveTextReadStateInit: {
				switch (t.type) {
					case TokenWhiteSpace: {
						
						break;
					}
					
					case TokenTextAnnotation: {
						
						break;
					}
						
					case TokenMoveNumber: {
						currentState = MoveTextReadStateM1;
						break;
					}
					
					case TokenGenericMove: {
						currentState = MoveTextReadStateM2;
						break;
					}
						
					case TokenGameTermination: {
						currentState = MoveTextReadStateFinal;
						break;
					}
						
					default: {
						throw std::out_of_range("Invalid token encountered");
						break;
					}
				}
				
				break;
			}
				
			case MoveTextReadStateM1: {
				switch (t.type) {
					case TokenWhiteSpace: {
						
						break;
					}
						
					case TokenDot: {
						
						break;
					}
						
					case TokenGenericMove: {
						currentState = MoveTextReadStateM2;
						break;
					}
						
					default: {
						throw std::out_of_range("Invalid token encountered");
						break;
					}
				}
				
				break;
			}
				
			case MoveTextReadStateM2: {
				switch (t.type) {
					case TokenWhiteSpace: {
						
						break;
					}
											
					case TokenTextAnnotation: {
						
						break;
					}
						
					case TokenNAG: {
						
						break;
					}
						
					case TokenMoveNumber: {
						currentState = MoveTextReadStateM1;
						break;
					}
						
					case TokenGenericMove: {
						
						break;
					}
						
					case TokenVariationBegin: {
						currentState = MoveTextReadStateM3;
						break;
					}
						
					case TokenGameTermination: {
						currentState = MoveTextReadStateFinal;
						break;
					}
						
					default: {
						throw std::out_of_range("Invalid token encountered");
						break;
					}
				}
				
				break;
			}
				
			case MoveTextReadStateM3: {
				switch (t.type) {
					case TokenWhiteSpace: {
						
						break;
					}

					case TokenTextAnnotation: {
						
						break;
					}
						
					case TokenMoveNumber: {
						currentState = MoveTextReadStateM4;
						break;
					}
						
					case TokenGenericMove: {
						currentState = MoveTextReadStateM5;
						break;
					}
						
					default: {
						throw std::out_of_range("Invalid token encountered");
						break;
					}
				}
				
				break;
			}
				
			case MoveTextReadStateM4: {
				switch (t.type) {
					case TokenWhiteSpace: {
						
						break;
					}
						
					case TokenDot: {
						
						break;
					}
						
					case TokenGenericMove: {
						currentState = MoveTextReadStateM5;
						break;
					}
						
					default: {
						throw std::out_of_range("Invalid token encountered");
						break;
					}
				}
				
				break;
			}
				
			case MoveTextReadStateM5: {
				switch (t.type) {
					case TokenWhiteSpace: {
						
						break;
					}
						
					case TokenTextAnnotation: {
						
						break;
					}
						
					case TokenNAG: {
						
						break;
					}
						
					case TokenGenericMove: {
						
						break;
					}
						
					case TokenVariationBegin: {
						currentState = MoveTextReadStateM3;
						
						break;
					}
						
					case TokenVariationEnd: {
						// Return to m2 in case the variation is
						// at recursion level 1
						if (RAVStack.size() == 1) {
							currentState = MoveTextReadStateM2;
						} else {
							currentState = MoveTextReadStateM3;
						}
						break;
					}
						
					default: {
						throw std::out_of_range("Invalid token encountered");
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
			// Set the current annotation to the last move if any.
			if (currentAnnotation.length() > 0) {
				this->orphanedComment = currentAnnotation;
			}
			break;
		}
		
		i += t.charactersConsumed;
	}
	
	if (currentState != MoveTextReadStateFinal) {
		throw std::invalid_argument("Move text contains unrecognizable text");
	}
	
	
	__moveTextParsed = true;
}

/*---------------------- Internal parser implementation -----------------------*/

Token nextToken(std::string::const_iterator begin, std::string::const_iterator end) {
	if (begin == end) {
		return Token();
	}
	
	auto i = begin;
	Token toReturn;
	
	// Try to recognize the tokens using the first character.
	switch (*i) {
		/*----------------- Long Recurring Strings -------------------*/
		case ' ':
		case '\r':
		case '\n':
		case '\t':
		case '\v': {
			toReturn.type = TokenWhiteSpace;
			while (i != end) {
				if (*i == ' ' || *i == '\r' || *i == '\n' || *i == '\t' || *i == '\v') {
					toReturn.contents += *i;
					toReturn.charactersConsumed++;
					i++;
				} else {
					break;
				}
			}
			
			break;
		}
			
		case '{': {
			toReturn.type = TokenTextAnnotation;
			i++;	// for '{'
			toReturn.charactersConsumed++;
			while (i != end) {
				if (*i == '}') {
					toReturn.charactersConsumed++;	// for '}'
					break;
				}
				
				toReturn.contents += *i;
				toReturn.charactersConsumed++;
				i++;
			}
			
			break;
		}
			
		case '.': {
			toReturn.type = TokenDot;
			while (i != end) {
				if (*i != '.') {
					break;
				}
				
				toReturn.contents += *i;
				toReturn.charactersConsumed++;
				i++;
			}
			break;
		}
			
		/*------------------ Variation Begin and End ------------------*/
		case '(': {
			toReturn.type = TokenVariationBegin;
			toReturn.contents = "(";
			toReturn.charactersConsumed = 1;
			break;
		}
			
		case ')': {
			toReturn.type = TokenVariationEnd;
			toReturn.contents = ")";
			toReturn.charactersConsumed = 1;
			break;
		}
			
		/*--------------------- Move Number and Game termination -----------------------*/
		case '0': {
			if ((i+1) != end && *(i+1) == '-') {
				if ((i+2) != end && *(i+2) == '1') {
					toReturn.type = TokenGameTermination;
					toReturn.contents = "0-1";
					toReturn.charactersConsumed = 3;
					break;
				} else {
					// Incomplete token '0-1'
					throw std::invalid_argument("Game termination token '0-1' incomplete");
				}
			}
			// Else fall through.
		}
			
		case '1': {
			if ((i+1) != end && *(i+1) == '-') {
				if ((i+2) != end && *(i+2) == '0') {
					toReturn.type = TokenGameTermination;
					toReturn.contents = "1-0";
					toReturn.charactersConsumed = 3;
					break;
				} else {
					// Incomplete token '1-0'
					throw std::invalid_argument("Game termination token '1-0' incomplete");
				}
			}
			
			if ((i+1) != end && *(i+1) == '/') {
				if ((i+6) != end && *(i+2) == '2' &&
					*(i+3) == '-' && *(i+4) == '1' &&
					*(i+5) == '/' && *(i+6) == '2') {
					toReturn.type = TokenGameTermination;
					toReturn.contents = "1/2-1/2";
					toReturn.charactersConsumed = 7;
					break;
				} else {
					// Incomplete token '1/2-1/2'
					throw std::invalid_argument("Game termination token '1/2-1/2' incomplete");
				}
			}
			// Else fall through
		}
		// '0' and '1' should fall through in case they don't indicate game termination
		case '2'...'9': {
			toReturn.type = TokenMoveNumber;
			while (i != end) {
				if (!std::isdigit(*i)) {
					break;
				}
				
				toReturn.contents += *i;
				toReturn.charactersConsumed++;
				i++;
			}
			break;
		}
			
		case '*': {
			toReturn.type = TokenGameTermination;
			toReturn.contents = "*";
			toReturn.charactersConsumed = 1;
			break;
		}
			
		/*-------------------- NAGs -----------------------*/
		case '$': {
			toReturn.type = TokenNAG;
			i++;
			toReturn.charactersConsumed++;	// For '$'
			if (i == end) {
				throw std::invalid_argument("NAG token incomplete");
			}
			
			while (i != end) {
				if (!std::isdigit(*i)) {
					break;
				}
				
				toReturn.contents += *i;
				toReturn.charactersConsumed++;
				i++;
			}
			
			break;
		}
		
		// Suffixes but to be returned as NAGs
		case '!': {
			toReturn.type = TokenNAG;
			toReturn.contents = "1";	// NAG code for good move (!)
			toReturn.charactersConsumed++;
			
			if ((i+1) != end && *(i+1) == '!') {
				toReturn.contents = "3";	// NAG code for excellent move (!!)
				toReturn.charactersConsumed++;
			}
			
			if ((i+1) != end && *(i+1) == '?') {
				toReturn.contents = "5";
				toReturn.charactersConsumed++;	// NAG code for speculative/interesting move (!?)
			}
			
			break;
		}
			
		case '?': {
			toReturn.type = TokenNAG;
			toReturn.contents = "2";	// NAG code for bad move (?)
			toReturn.charactersConsumed++;
			
			if ((i+1) != end && *(i+1) == '?') {
				toReturn.contents = "4";
				toReturn.charactersConsumed++;	// NAG code for blunder (??)
			}
			
			if ((i+1) != end && *(i+1) == '!') {
				toReturn.contents = "6";	// NAG code for questionable/dubious move (?!)
				toReturn.charactersConsumed++;
			}
			
			break;
		}
			
		/*----------------- Move suffix -------------------*/
		case '#':
		case '+': {
			// Should be considered as white space
			toReturn.type = TokenWhiteSpace;
			toReturn.contents = ' ';
			toReturn.charactersConsumed = 1;
			break;
		}
			
		/*----------------- Move Tokens -------------------*/
		case '-': {
			toReturn.type = TokenGenericMove;
								
			if ((i+1) != end && *(i+1) == '-') {
				toReturn.subType = TokenSubTypeMoveNullMove;
				toReturn.contents = "--";
				toReturn.charactersConsumed = 2;
			} else {
				throw std::invalid_argument("Null move token incomplete");
			}
			
			break;
		}
		
		case 'O': {
			toReturn.type = TokenGenericMove;
			
			// Extract the next 5 characters, including first 'O'
			while (i != end && toReturn.charactersConsumed <= 5) {
				toReturn.contents += *i;
				toReturn.charactersConsumed++;
				i++;
			}
			
			if (std::regex_search(toReturn.contents, std::regex("O-O-O"))) {
				toReturn.subType = TokenSubTypeMoveQueenSideCastling;
				toReturn.contents = "O-O-O";
				toReturn.charactersConsumed = 5;
				
				break;	// Done
			}
			
			// At this point the token sub type should have been set, otherwise check for short castling.
			if (std::regex_search(toReturn.contents, std::regex("O-O"))) {
				toReturn.subType = TokenSubTypeMoveKingSideCastling;
				toReturn.contents = "O-O";
				toReturn.charactersConsumed = 3;
				
				break;	// Done
			}
			
			if (toReturn.subType == TokenSubTypeNone) {
				throw std::invalid_argument("Move token castling incomplete");
			}
			
			break;
		}
		
		case 'a'...'h': {
			// Extract the next 6 characters, including 'a-h'
			toReturn.type = TokenGenericMove;
			
			while (i != end && toReturn.charactersConsumed <= 6) {
				toReturn.contents += *i;
				toReturn.charactersConsumed++;
				i++;
			}
			
			std::smatch matchedString;
			
			// Match pawn promotion with capture. (Eg: bxa8=Q or ba8Q)
			if (std::regex_search(toReturn.contents, matchedString, std::regex("^[a-h]x?[a-h][18]=?[QRNB]"))) {
				toReturn.subType = TokenSubTypeMovePawnCapturePromotion;
				toReturn.contents = matchedString[0];
				toReturn.charactersConsumed = 6;
				
				// Erase capture character 'x' if necessary
				if (toReturn.contents.find('x') == std::string::npos) {
					toReturn.charactersConsumed--;
				} else {
					toReturn.contents.erase(1, 1);	// Erase 'x' character at index 1.
				}
				
				// Erase promotion equals character '=' if necessary
				if (toReturn.contents.find('=') == std::string::npos) {
					toReturn.charactersConsumed--;
				} else {
					toReturn.contents.erase(3, 1); // Erase '=' at index 3.
					// Note index is 3 not 4, because 'x' was erased already
				}
				
				break;	// Done processing
			}
			
			// Match pawn promotion. (Eg: a8Q or b8=Q)
			if (std::regex_search(toReturn.contents, matchedString, std::regex("^[a-h][18]=?[QRNB]"))) {
				toReturn.subType = TokenSubTypeMovePawnPromotion;
				toReturn.contents = matchedString[0];
				toReturn.charactersConsumed = 4;
								
				// Erase promotion equals character '=' if necessary
				if (toReturn.contents.find('=') == std::string::npos) {
					toReturn.charactersConsumed--;
				} else {
					toReturn.contents.erase(2, 1); // Erase '=' at index 2.
				}
				
				break;	// Done processing
			}
			
			// Match pawn capture (Eg: axb4 or ab4)
			if (std::regex_search(toReturn.contents, matchedString, std::regex("^[a-h]x?[a-h][2-7]"))) {
				toReturn.subType = TokenSubTypeMovePawnCapture;
				toReturn.contents = matchedString[0];
				toReturn.charactersConsumed = 4;
				
				// Erase capture character 'x' if necessary
				if (toReturn.contents.find('x') == std::string::npos) {
					toReturn.charactersConsumed--;
				} else {
					toReturn.contents.erase(1, 1);	// Erase 'x' character at index 1.
				}
				
				break;	// Done processing
			}
			
			// Match pawn normal move (Eg: e4 e5)
			if (std::regex_search(toReturn.contents, matchedString, std::regex("^[a-h][2-7]"))) {
				toReturn.subType = TokenSubTypeMovePawn;
				toReturn.contents = matchedString[0];
				toReturn.charactersConsumed = 2;
								
				break;	// Done processing
			}
			
			// Something wrong in case sub-type has not been set
			if (toReturn.subType == TokenSubTypeNone) {
				throw std::invalid_argument("Pawn move token incomplete");
			}
			
			break;
		}
			
		case 'K': {
			// Extract next 6 characters
			toReturn.type = TokenGenericMove;
			
			while (i != end && toReturn.charactersConsumed <= 6) {
				toReturn.contents += *i;
				toReturn.charactersConsumed++;
				i++;
			}
			
			std::smatch matchedString;
			
			// Try to match king move with square disambiguation so as to declare the token invalid
			if (std::regex_search(toReturn.contents, std::regex("^K[a-h][1-8]x?[a-h][1-8]"))) {
				throw std::invalid_argument("King move token should not contain fromSquare");
			}
			
			// Match normal king move
			if (std::regex_search(toReturn.contents, matchedString, std::regex("^Kx?[a-h][1-8]"))) {
				toReturn.subType = TokenSubTypeMovePiece;
				toReturn.contents = matchedString[0];
				toReturn.charactersConsumed = 4;
				
				// Erase capture character 'x' if necessary
				if (toReturn.contents.find('x') == std::string::npos) {
					toReturn.charactersConsumed--;
				} else {
					toReturn.contents.erase(1, 1);	// Erase 'x' character at index 1.
				}
			} else {
				throw std::invalid_argument("King move token incomplete");
			}
			
			break;
		}
			
		case 'Q':
		case 'R':
		case 'B':
		case 'N': {
			// Extract the next 6 characters
			toReturn.type = TokenGenericMove;
			
			while (i != end && toReturn.charactersConsumed <= 6) {
				toReturn.contents += *i;
				toReturn.charactersConsumed++;
				i++;
			}
			
			std::smatch matchedString;
			
			// Match move with fromSquare given. (Eg: Nb3xa5 or Nb3a5)
			if (std::regex_search(toReturn.contents, matchedString, std::regex("^[QRBN][a-h][1-8]x?[a-h][1-8]"))) {
				toReturn.subType = TokenSubTypeMovePieceFromSquare;
				toReturn.contents = matchedString[0];
				toReturn.charactersConsumed = 6;
				
				// Erase capture character 'x' if necessary
				if (toReturn.contents.find('x') == std::string::npos) {
					toReturn.charactersConsumed--;
				} else {
					toReturn.contents.erase(3, 1);	// Erase 'x' character at index 1.
				}
				
				break;	// Done processing
			}
			
			// Match move with from file given. (Eg: Nbxa5 or Nba5)
			if (std::regex_search(toReturn.contents, matchedString, std::regex("^[QRBN][a-h]x?[a-h][1-8]"))) {
				toReturn.subType = TokenSubTypeMovePieceFromFile;
				toReturn.contents = matchedString[0];
				toReturn.charactersConsumed = 5;
				
				// Erase capture character 'x' if necessary
				if (toReturn.contents.find('x') == std::string::npos) {
					toReturn.charactersConsumed--;
				} else {
					toReturn.contents.erase(2, 1);	// Erase 'x' character at index 1.
				}
				
				break;	// Done processing
			}
			
			// Match move with from rank given. (Eg: N3xa5 or N3a5)
			if (std::regex_search(toReturn.contents, matchedString, std::regex("^[QRBN][1-8]x?[a-h][1-8]"))) {
				toReturn.subType = TokenSubTypeMovePieceFromRank;
				toReturn.contents = matchedString[0];
				toReturn.charactersConsumed = 5;
				
				// Erase capture character 'x' if necessary
				if (toReturn.contents.find('x') == std::string::npos) {
					toReturn.charactersConsumed--;
				} else {
					toReturn.contents.erase(2, 1);	// Erase 'x' character at index 1.
				}
				
				break;	// Done processing
			}
			
			// Match normal move. (Eg: Nxa5 or Na5)
			if (std::regex_search(toReturn.contents, matchedString, std::regex("^[QRBN]x?[a-h][1-8]"))) {
				toReturn.subType = TokenSubTypeMovePiece;
				toReturn.contents = matchedString[0];
				toReturn.charactersConsumed = 4;
				
				// Erase capture character 'x' if necessary
				if (toReturn.contents.find('x') == std::string::npos) {
					toReturn.charactersConsumed--;
				} else {
					toReturn.contents.erase(1, 1);	// Erase 'x' character at index 1.
				}
				
				break;	// Done processing
			}
			
			break;
		}
	}
	
	return toReturn;
}