//
//  PGNFile.h
//  PGN Reader
//
//  Created by Santhosbaala RS on 11/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#ifndef __PGN_Reader__PGNFile__
#define __PGN_Reader__PGNFile__

#include <fstream>
#include <sstream>
#include <tuple>
#include <vector>

class PGNFile {
	std::ifstream inputFileStream;
	// Tuple holds the starting and ending location of
	// the games in the data stream.
	std::vector<std::tuple<std::ifstream::pos_type, std::ifstream::pos_type>> gamePointers;
	
	void setGamePointers();
public:
	PGNFile() = default;
	PGNFile(std::string fileName);
	
	unsigned int getGameCount();
};

#endif /* defined(__PGN_Reader__PGNFile__) */
