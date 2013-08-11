//
//  PGNFile.cpp
//  PGN Reader
//
//  Created by Santhosbaala RS on 11/08/13.
//  Copyright (c) 2013 64cloud. All rights reserved.
//

#include "PGNFile.h"

#include <iostream>
#include <exception>

PGNFile::PGNFile(std::string fileName) {
	inputFileStream.open(fileName);
	if (!inputFileStream.good()) {
		throw std::invalid_argument("The file does not exist or not readable");
	}
	
	setGamePointers();
}

void PGNFile::setGamePointers() {
	// Function should go through the file and find where the game begins/ends, store it inside a tuple
	
	enum {
		ReadingMeta,
		ReadingTextCommentary,
		ReadingMoveText,
	} readingStatus = ReadingMeta;
	
	std::ifstream::pos_type currentGameBegin = 0, currentGameEnd = 0;
	
	int recursionLevel = 0;
	while (inputFileStream.good()) {
		char ch = inputFileStream.get();
		
		// Skip whitespaces
		if (ch == '\n' ||
			ch == '\r' ||
			ch == '\t' ||
			ch == '\v' ||
			ch == ' ' ||
			ch == EOF) {
			continue;
		}
		
		switch (readingStatus) {
			case ReadingMeta: {
				switch (ch) {
					case '[': {
						currentGameBegin = inputFileStream.tellg();
						readingStatus = ReadingMoveText;
	
						// Consume characters till a blank line is reached
						std::string line;
						while (inputFileStream.good()) {
							getline(inputFileStream, line);
							
							if (line.length() == 0 || line[0] == '\r' || line[0] == '\n') {
								break;
							}
						}
						
						break;
					}
						
					default: {
						// Invalid character encountered
						throw std::invalid_argument("Invalid character encountered while searching for meta data");
						break;
					}
				}
				
				break;
			}
				
			case ReadingMoveText: {
				switch (ch) {
					// Skip text commentary
					case '{': {
						readingStatus = ReadingTextCommentary;
						break;
					}
						
					default: {
						// Look for termination markers.
						switch (ch) {
							case '*': {
								currentGameEnd = inputFileStream.tellg();
								break;
							}
								
							// See if the next few characters read 1-0 or 1/2-1/2
							case '1': {
								if (inputFileStream.peek() == '-') {
									char next1 = inputFileStream.get(), next2;
									if (inputFileStream.good()) {
										next2 = inputFileStream.get();
										if (next2 == '0') {
											currentGameEnd = inputFileStream.tellg();
										} else {
											inputFileStream.putback(next1);
											inputFileStream.putback(next2);
										}
									}
								} else if (inputFileStream.peek() == '/') {
									char next1 = inputFileStream.get(), next2, next3, next4, next5, next6;
									if (inputFileStream.good()) { next2 = inputFileStream.get(); }
									else {
										inputFileStream.putback(next1);
										break;
									}
									if (inputFileStream.good()) { next3 = inputFileStream.get(); }
									else {
										inputFileStream.putback(next1); inputFileStream.putback(next2);
										break;
									}
									if (inputFileStream.good()) { next4 = inputFileStream.get(); }
									else {
										inputFileStream.putback(next1); inputFileStream.putback(next2);
										inputFileStream.putback(next3);
										break;
									}
									if (inputFileStream.good()) { next5 = inputFileStream.get(); }
									else {
										inputFileStream.putback(next1); inputFileStream.putback(next2);
										inputFileStream.putback(next3); inputFileStream.putback(next4);
										break;
									}
									if (inputFileStream.good()) { next6 = inputFileStream.get(); }
									else {
										inputFileStream.putback(next1); inputFileStream.putback(next2);
										inputFileStream.putback(next3); inputFileStream.putback(next4);
										inputFileStream.putback(next5);
										break;
									}
									
									if (next2 == '2' &&
										next3 == '-' &&
										next4 == '1' &&
										next5 == '/' &&
										next6 == '2') {
										currentGameEnd = inputFileStream.tellg();
									} else {
										inputFileStream.putback(next1); inputFileStream.putback(next2);
										inputFileStream.putback(next3); inputFileStream.putback(next4);
										inputFileStream.putback(next5); inputFileStream.putback(next6);
									}
								}
								
								break;
							}
								
							// See if the next few characters read 0-1
							case '0': {
								if (inputFileStream.peek() == '-') {
									char next1 = inputFileStream.get(), next2;
									if (inputFileStream.good()) {
										next2 = inputFileStream.get();
										if (next2 == '1') {
											currentGameEnd = inputFileStream.tellg();
										} else {
											inputFileStream.putback(next1);
											inputFileStream.putback(next2);
										}
									}
								}
								
								break;
							}
						}
						
						// Push the current <begin, end> tuple
						if (currentGameEnd != 0) {
							gamePointers.push_back(std::make_tuple(currentGameBegin, currentGameEnd));
							currentGameBegin = 0;
							currentGameEnd = 0;
							readingStatus = ReadingMeta;
						}
					}
				}
				
				break;
			}
				
			case ReadingTextCommentary: {
				if (ch == '{') { recursionLevel++; }
				if (ch == '}') { recursionLevel--; }
				if (recursionLevel == 0) {
					readingStatus = ReadingMoveText;
				}
				
				break;
			}
		}
	}
	
	// Check the pointers for begin and end were properly set - Invalid state
	if (currentGameBegin != 0 && currentGameEnd == 0) {
		throw std::invalid_argument("The game termination string was not found for a game");
	}
}

unsigned int PGNFile::getGameCount() {
	return (unsigned int)gamePointers.size();
}