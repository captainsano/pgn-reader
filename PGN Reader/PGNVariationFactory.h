//
//  PGNVariationFactory.h
//  PGN Reader
//
//  Created by Santhosbaala RS on 21/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#ifndef PGN_Reader_PGNVariationFactory_h
#define PGN_Reader_PGNVariationFactory_h

#include <vector>
#include "PGNVariation.h"

namespace PGNVariationFactory {
	std::shared_ptr<PGNVariation> legalVariation();
}

#endif
