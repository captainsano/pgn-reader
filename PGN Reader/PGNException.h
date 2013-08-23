//
//  PGNException.h
//  PGN Reader
//
//  Created by Santhosbaala RS on 22/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#ifndef PGN_Reader_PGNException_h
#define PGN_Reader_PGNException_h

#include <string>
#include <exception>

struct illegal_move : public std::runtime_error {
	illegal_move(const std::string & __s) : runtime_error(__s) { }
	illegal_move(const char * __s) : runtime_error(__s) { }
};

struct parse_error : public std::runtime_error {
	parse_error(const std::string & __s) : runtime_error(__s) { }
	parse_error(const char * __s) : runtime_error(__s) { }
};

#endif
