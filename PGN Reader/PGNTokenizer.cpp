//
//  PGNTokenizer.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 22/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include "PGNTokenizer.h"
#include "PGNException.h"
#include <regex>

PGNTokenizer::Token PGNTokenizer::nextToken(std::string::const_iterator begin, std::string::const_iterator end) {
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
					throw parse_error("Game termination token '0-1' incomplete");
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
					throw parse_error("Game termination token '1-0' incomplete");
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
					throw parse_error("Game termination token '1/2-1/2' incomplete");
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
				throw parse_error("NAG token incomplete");
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
				throw parse_error("Null move token incomplete");
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
				throw parse_error("Move token castling incomplete");
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
				throw parse_error("Pawn move token incomplete");
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
				throw parse_error("King move token should not contain fromSquare");
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
				throw parse_error("King move token incomplete");
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

void PGNTokenizer::fillTempMoveWithToken(TempMove & move, const Token & t) {
	if (t.type != TokenGenericMove) {
		throw parse_error("Supplied token should be a move");
	}
	
	switch (t.subType) {
		case TokenSubTypeMoveNullMove: {
			// Do nothing
			break;
		}
			
		case TokenSubTypeMovePawn: {
			move.pieceMoved = sfc::cfw::GenericPiecePawn;
			move.fromFile = static_cast<unsigned short>(t.contents[0] - 'a');
			move.toFile = static_cast<unsigned short>(t.contents[0] - 'a');
			move.toRank = static_cast<unsigned short>(t.contents[1] - '1');
			break;
		}
			
		case TokenSubTypeMovePawnCapture: {
			move.pieceMoved = sfc::cfw::GenericPiecePawn;
			move.fromFile = static_cast<unsigned short>(t.contents[0] - 'a');
			move.toFile = static_cast<unsigned short>(t.contents[1] - 'a');
			move.toRank = static_cast<unsigned short>(t.contents[2] - '1');
			break;
		}
			
		case TokenSubTypeMovePawnPromotion: {
			move.pieceMoved = sfc::cfw::GenericPiecePawn;
			move.fromFile = static_cast<unsigned short>(t.contents[0] - 'a');
			move.toFile = static_cast<unsigned short>(t.contents[0] - 'a');
			move.toRank = static_cast<unsigned short>(t.contents[1] - '1');
			move.promotedPiece = sfc::cfw::makePromotablePiece(t.contents[2]);
			break;
		}
			
		case TokenSubTypeMovePawnCapturePromotion: {
			move.pieceMoved = sfc::cfw::GenericPiecePawn;
			move.fromFile = static_cast<unsigned short>(t.contents[0] - 'a');
			move.toFile = static_cast<unsigned short>(t.contents[1] - 'a');
			move.toRank = static_cast<unsigned short>(t.contents[2] - '1');
			move.promotedPiece = sfc::cfw::makePromotablePiece(t.contents[3]);
			break;
		}
			
		case TokenSubTypeMovePiece: {
			move.pieceMoved = sfc::cfw::makeGenericPiece(t.contents[0]);
			move.toFile = static_cast<unsigned short>(t.contents[1] - 'a');
			move.toRank = static_cast<unsigned short>(t.contents[2] - '1');
			break;
		}
			
		case TokenSubTypeMovePieceFromFile: {
			move.pieceMoved = sfc::cfw::makeGenericPiece(t.contents[0]);
			move.fromFile = static_cast<unsigned short>(t.contents[1] - 'a');
			move.toFile = static_cast<unsigned short>(t.contents[2] - 'a');
			move.toRank = static_cast<unsigned short>(t.contents[3] - '1');
			break;
		}
			
		case TokenSubTypeMovePieceFromRank: {
			move.pieceMoved = sfc::cfw::makeGenericPiece(t.contents[0]);
			move.fromRank = static_cast<unsigned short>(t.contents[1] - '1');
			move.toFile = static_cast<unsigned short>(t.contents[2] - 'a');
			move.toRank = static_cast<unsigned short>(t.contents[3] - '1');
			break;
		}
			
		case TokenSubTypeMovePieceFromSquare: {
			move.pieceMoved = sfc::cfw::makeGenericPiece(t.contents[0]);
			move.fromFile = static_cast<unsigned short>(t.contents[1] - 'a');
			move.fromRank = static_cast<unsigned short>(t.contents[2] - '1');
			move.toFile = static_cast<unsigned short>(t.contents[3] - 'a');
			move.toRank = static_cast<unsigned short>(t.contents[4] - '1');
			break;
		}
			
			// Castling for both white and black goes e1-h1 (O-O) or e1-a1 (O-O-O)
			// The color is decided later
			// No chess960 support for castling
		case TokenSubTypeMoveKingSideCastling: {
			move.pieceMoved = sfc::cfw::GenericPieceKing;
			move.fromFile = 4;
			move.fromRank = 0;
			move.toFile = 7;
			move.toRank = 0;
			break;
		}
			
		case TokenSubTypeMoveQueenSideCastling: {
			move.pieceMoved = sfc::cfw::GenericPieceKing;
			move.fromFile = 4;
			move.fromRank = 0;
			move.toFile = 0;
			move.toRank = 0;
			break;
		}
			
		default:
			throw parse_error("Token sub-type is not related to move");
			break;
	}
}