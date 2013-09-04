//
//  PGNTokenizer.h
//  PGN Reader
//
//  Created by Santhosbaala RS on 22/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#ifndef PGN_Reader_PGNTokenizer_h
#define PGN_Reader_PGNTokenizer_h

#include <vector>
#include <string>
#include <limits.h>
#include "Piece.h"

namespace PGNTokenizer {
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
	
	
	struct TempVariation;	// Forward Declaration
	
	struct TempMove {
		sfc::cfw::GenericPiece pieceMoved;
		
		unsigned short fromFile = SHRT_MAX;
		unsigned short fromRank = SHRT_MAX;
		unsigned short toFile	= SHRT_MAX;
		unsigned short toRank	= SHRT_MAX;
		// All set to 0s indicate a NULL move
		// SHRT_MAX indicates that the field has not yet been set.
		
		sfc::cfw::PromotablePiece promotedPiece = sfc::cfw::PromotablePieceNone;
		
		std::string textAnnotation;
		std::vector<unsigned int> NAGs;
		std::vector<std::shared_ptr<TempVariation>> variations;
		// TODO: Include commands within annotation
		
		TempMove() = default;
	};
	
	struct TempVariation {
		std::string firstComment;
		std::vector<std::shared_ptr<TempMove>> moves;
		
		TempVariation() = default;
	};
	
	Token nextToken(std::string::const_iterator begin, std::string::const_iterator end);
	void fillTempMoveWithToken(TempMove & move, const Token & t);
}

#endif
