//
//  PGNMove.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 21/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include "PGNMove.h"
#include "PGNVariation.h"

PGNMove::PGNMove(const Move & aMove, std::vector<PGNVariation> aVariations, std::vector<unsigned int> aNAGs, const std::string & aTextAnnotation) : Move(aMove) {
	// Check if all the variations have the same before game state
	for (auto v : aVariations) {
		this->addVariation(v);
	}
	
	for (auto n : aNAGs) {
		this->addNAG(n);
	}
	
	this->textAnnotation = aTextAnnotation;
}

void PGNMove::addVariation(const PGNVariation & aVariation) {
	// Prevent addition of a variation in case it exists and prevent addition in case
	// the before game state is not the same as this move
	if (aVariation.size() == 0) {
		throw std::invalid_argument("The new variation is empty");
	}
	
	if (*(aVariation[0].getGameStateBeforeMove()) == *(this->getGameStateBeforeMove())) {
		this->variations.push_back(aVariation);
	} else {
		throw std::invalid_argument("The new variation's game state before move does not match");
	}
}

void PGNMove::addNAG(const unsigned int & aNAG) {
	for (auto n : this->NAGs) {
		if (n == aNAG) return;
	}
	
	this->NAGs.push_back(aNAG);
}

void PGNMove::appendTextAnnotation(const std::string & aTextAnnotation) {
	if (this->textAnnotation.length() > 0) {
		this->textAnnotation += " ";
	}
	
	this->textAnnotation += aTextAnnotation;
}

std::string PGNMove::getSANString() const {
	std::string toReturn;
	
	if (this->getCastlingType() == sfc::cfw::CastlingTypeKSide) {
		toReturn = "O-O";
	} else if (this->getCastlingType() == sfc::cfw::CastlingTypeQSide) {
		toReturn = "O-O-O";
	}
	
	switch (sfc::cfw::getGenericPiece(this->getPieceMoved())) {
		case sfc::cfw::GenericPieceKing: {
			if (this->getCastlingType() == sfc::cfw::CastlingTypeNone) {
				toReturn = "K";
			}
			break;
		}
		case sfc::cfw::GenericPieceQueen: toReturn = "Q"; break;
		case sfc::cfw::GenericPieceRook: toReturn = "R"; break;
		case sfc::cfw::GenericPieceBishop: toReturn = "B"; break;
		case sfc::cfw::GenericPieceKnight: toReturn = "N"; break;
			
		case sfc::cfw::GenericPieceNone:
		case sfc::cfw::GenericPiecePawn:
		default: break;
	}
	
	// Disambiguation string
	std::string disambiguationString;
	bool commonRank = false, commonFile = false, onlyToSquareIsCommon = false;
	
	//	std::vector<std::shared_ptr<Move>> allLegalMoves(std::shared_ptr<GameState> beforeGameState, const Piece pieceType = PieceNone);

	
	for (auto otherMove : sfc::cfw::MoveFactory::allLegalMoves(this->getGameStateBeforeMove(), this->getPieceMoved())) {
		if (otherMove->getFromSquare() != this->getFromSquare() &&
			otherMove->getPieceMoved() == this->getPieceMoved() &&
			otherMove->getToSquare() == this->getToSquare()) {
			
			// Check if the pieces are in the same file, then use rank for disambiguation
			if (otherMove->getFromSquare().getFile() == this->getFromSquare().getFile()) {
				commonFile = true;
			}
			
			// Check if the pieces are in the same rank, then use file for disambiguation
			if (otherMove->getFromSquare().getRank() == this->getFromSquare().getRank()) {
				commonRank = true;
			}
			
			// If both file and rank are not common, then file/rank can be used. SAN advices file
			onlyToSquareIsCommon = true;
		}
	}
	
	if (commonRank && commonFile) {
		disambiguationString += this->getFromSquare().getLabel();
	} else if (commonRank) {
		disambiguationString += this->getFromSquare().getLabel()[0];	// Mark the file
	} else if (commonFile) {
		disambiguationString += this->getFromSquare().getLabel()[1];	// Mark the rank
	} else if (onlyToSquareIsCommon) {
		disambiguationString += this->getFromSquare().getLabel()[0];	// Use file
	}
	
	if (disambiguationString.length() > 0 && getGenericPiece(this->getPieceMoved()) != sfc::cfw::GenericPiecePawn) {
		toReturn += disambiguationString;
	}
	
	// Capture character
	if (this->getCastlingType() == sfc::cfw::CastlingTypeNone) {
		if (this->getCapturedPiece() != sfc::cfw::PieceNone) {
			// If the capture was by a pawn, append the file
			if (getGenericPiece(this->getPieceMoved()) == sfc::cfw::GenericPiecePawn) {
				toReturn += this->getFromSquare().getLabel()[0];
			}
			toReturn += "x";
		}
	}
	
	// toSquare
	if (this->getCastlingType() == sfc::cfw::CastlingTypeNone) {
		toReturn += this->getToSquare().getLabel();
	}
	
	// Promotion
	if (getGenericPiece(this->getPieceMoved()) == sfc::cfw::GenericPiecePawn &&
		(this->getToSquare().getRank() == 7 || this->getToSquare().getRank() == 0)) {
		toReturn += '=';
		switch (this->getPromotedToPiece()) {
			case sfc::cfw::PromotablePieceQueen:	toReturn += 'Q'; break;
			case sfc::cfw::PromotablePieceRook:	toReturn += 'R'; break;
			case sfc::cfw::PromotablePieceBishop: toReturn += 'B'; break;
			case sfc::cfw::PromotablePieceKnight: toReturn += 'N'; break;
			default: break;
		}
	}
	
	// Check/Checkmate
	if (getPieceColor(this->getPieceMoved()) == sfc::cfw::ColorWhite) {
		switch (this->getGameStateAfterMove()->getBlackKingStatus()) {
			case sfc::cfw::KingStatusCheck: toReturn += "+"; break;
			case sfc::cfw::KingStatusCheckMate: toReturn += "#"; break;
			case sfc::cfw::KingStatusNormal:
			case sfc::cfw::KingStatusStaleMate:
				break;
		}
	} else {
		switch (this->getGameStateAfterMove()->getWhiteKingStatus()) {
			case sfc::cfw::KingStatusCheck: toReturn += "+"; break;
			case sfc::cfw::KingStatusCheckMate: toReturn += "#"; break;
			case sfc::cfw::KingStatusNormal:
			case sfc::cfw::KingStatusStaleMate:
				break;
		}
	}
	
	return toReturn;
}