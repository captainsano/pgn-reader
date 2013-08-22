//
//  PGNException.h
//  PGN Reader
//
//  Created by Santhosbaala RS on 22/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#ifndef PGN_Reader_PGNException_h
#define PGN_Reader_PGNException_h

#include <exception>

class illegal_move : public std::exception {
	std::string reason;
public:
	illegal_move(const std::string & aReason) : reason(aReason) { }
	
	const char* what() const noexcept override {
		return this->reason.c_str();
	}
};

#endif
