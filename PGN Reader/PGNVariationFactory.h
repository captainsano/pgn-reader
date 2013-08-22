//
//  PGNVariationFactory.h
//  PGN Reader
//
//  Created by Santhosbaala RS on 21/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#ifndef PGN_Reader_PGNVariationFactory_h
#define PGN_Reader_PGNVariationFactory_h

#include <memory>
#include <vector>
#include "GameState.h"
#include "PGNVariation.h"
#include "PGNTokenizer.h"	// For TempMove

namespace PGNVariationFactory {	
	/*
	 * Generate a legal PGNVariation out of TempVariation.
	 * This function is recursive! :D
	 */
	std::shared_ptr<PGNVariation> legalVariation(const PGNTokenizer::TempVariation & tempVariation, const sfc::cfw::GameState & initGameState);
}

#endif