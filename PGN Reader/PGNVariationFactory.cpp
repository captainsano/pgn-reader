//
//  PGNVariationFactory.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 22/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include <iostream>

#include "PGNVariationFactory.h"
#include "MoveFactory.h"
#include "PGNMove.h"
#include "PGNException.h"

std::shared_ptr<PGNVariation> PGNVariationFactory::legalVariation(const PGNTokenizer::TempVariation & tempVariation, const sfc::cfw::GameState & initGameState) {
	std::shared_ptr<PGNVariation> toReturn = std::make_shared<PGNVariation>(tempVariation.firstComment);
	
	sfc::cfw::GameState currentGameState = initGameState;
	// look for moves based on the available information
	for (auto & m : tempVariation.moves) {
		std::shared_ptr<sfc::cfw::Move> internalMove = nullptr;
		sfc::cfw::Piece pieceMoved = sfc::cfw::makePiece(m->pieceMoved, currentGameState.getSideToMove());
		
		// Adjust castling rank if the piece is a piece is black king
		if (pieceMoved == sfc::cfw::PieceBKing && (m->fromRank == 0 && m->fromFile == 4)) {
			if (m->toRank == 0 && m->toFile == 7) {	// KSide castling
				m->fromRank = m->toRank = 7;
			} else if (m->toRank == 0 && m->toFile == 0) { // QSide castling
				m->fromRank = m->toRank = 7;
			}
		}
		
		if (m->fromFile != SHRT_MAX && m->fromRank != SHRT_MAX) {
			internalMove = sfc::cfw::MoveFactory::legalMove(std::make_shared<sfc::cfw::GameState>(currentGameState),
															sfc::cfw::Square(m->fromFile, m->fromRank),
															sfc::cfw::Square(m->toFile, m->toRank),
															m->promotedPiece);
		} else if (m->fromFile != SHRT_MAX) {
			// Search through all the ranks for the piece and check if the move is legal
			for (unsigned short rank = 0; rank < 8; rank++) {
				sfc::cfw::Square fromSquare(m->fromFile, rank);
				if ((*(currentGameState.getPosition()))[fromSquare] == pieceMoved) {
					internalMove = internalMove = sfc::cfw::MoveFactory::legalMove(std::make_shared<sfc::cfw::GameState>(currentGameState),
																				   fromSquare,
																				   sfc::cfw::Square(m->toFile, m->toRank),
																				   m->promotedPiece);
					if (internalMove != nullptr) {
						break;
					}
				}
			}
		} else if (m->fromRank != SHRT_MAX) {
			// Search through all the files for the piece and check if the move is legal
			for (unsigned short file = 0; file < 8; file++) {
				sfc::cfw::Square fromSquare(file, m->fromRank);
				if ((*(currentGameState.getPosition()))[fromSquare] == pieceMoved) {
					internalMove = internalMove = sfc::cfw::MoveFactory::legalMove(std::make_shared<sfc::cfw::GameState>(currentGameState),
																				   fromSquare,
																				   sfc::cfw::Square(m->toFile, m->toRank),
																				   m->promotedPiece);
					if (internalMove != nullptr) {
						break;
					}
				}
			}
		} else {
			// Search through all the squares for the piece and check if the move is legal
			for (unsigned int fromSquare = 0; fromSquare < 64; fromSquare++) {
				if ((*(currentGameState.getPosition()))[fromSquare] == pieceMoved) {
					internalMove = internalMove = sfc::cfw::MoveFactory::legalMove(std::make_shared<sfc::cfw::GameState>(currentGameState),
																				   fromSquare,
																				   sfc::cfw::Square(m->toFile, m->toRank),
																				   m->promotedPiece);
					if (internalMove != nullptr) {
						break;
					}
				}
			}
		}
		
		if (internalMove == nullptr) {
			std::string errorString = "Illegal Move: ";
			errorString += std::to_string(pieceMoved) + " ";
			errorString += "(" + std::to_string(m->fromFile) + ", " + std::to_string(m->fromRank) + ") -> ";
			errorString += "(" + std::to_string(m->toFile) + ", " + std::to_string(m->toRank) + ")";
			throw illegal_move(errorString);
		}
		
		std::shared_ptr<PGNMove> currentMove = std::make_shared<PGNMove>(*internalMove);
		
		// Add variations
		for (auto v : m->variations) {
			currentMove->addVariation(*(legalVariation(*v, currentGameState)));
		}
		
		// Add NAGs
		for (auto n : m->NAGs) {
			currentMove->addNAG(n);
		}
		
		// Add text comment
		currentMove->appendTextAnnotation(m->textAnnotation);
		
		// Add this move to the main variation
		toReturn->addMove(*currentMove);
		
		// Update the current game state
		currentGameState = *(currentMove->getGameStateAfterMove());
	}
	
	return toReturn;
}